#include "core/graphic/vulkan_manager.h"
#include "service/log.h"
#include <cstring>
#include <memory>

using namespace eff;
using namespace std;



VulkanManager::VulkanManager(const WindowCreateInfo& windowInfo,const InstanceCreateInfo& instanceInfo){

    window = move(unique_ptr<Window>(new Window(windowInfo)));
    instance = move(unique_ptr<VulkanInstance>(new VulkanInstance(instanceInfo)));

    window->setSurface(instance->getInstance());
}


VulkanManager::~VulkanManager(){
    window->surface.reset(nullptr);
    instance.reset(nullptr);
    window.reset(nullptr);
}


VulkanManager::Window::Window(const WindowCreateInfo& _info):info(_info){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if(info.isResizable)
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    if(info.size == glm::vec2({0,0})){

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
        
        if(info.resolution == glm::vec2({0,0}))
            this->g_window = glfwCreateWindow(videoMode->width, videoMode->height, info.name.c_str(), monitor, NULL);
        else    
            this->g_window = glfwCreateWindow(info.resolution.x, info.resolution.y, info.name.c_str(), monitor, NULL);

    }else{
    
        if(info.isResizable)
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
        this->g_window = glfwCreateWindow(info.size.x, info.size.y, info.name.c_str() , nullptr, nullptr);
    }
}


void VulkanManager::Window::setSurface(VkInstance& _surfaceInstance){
    surface.reset( new Surface(_surfaceInstance, *this));
}


VulkanManager::Window::~Window(){
    glfwDestroyWindow(g_window);
    glfwTerminate();
}


VulkanManager::Window::Surface::Surface(VkInstance& instance, Window& window):parentInstance(instance){
    if (glfwCreateWindowSurface(instance, window.g_window, nullptr, &vk_surface) != VK_SUCCESS)
            throw Log::Exception("failed to create window surface! window name: " + window.info.name, true);
}


VulkanManager::Window::Surface::~Surface(){
    vkDestroySurfaceKHR(parentInstance, vk_surface, nullptr);
}


VulkanManager::VulkanInstance::VulkanInstance(const InstanceCreateInfo& instanceInfo){
#ifdef DEBUG_MODE
    validation_layer = move(unique_ptr<ValidationLayer>( new ValidationLayer(instanceInfo.filename_ValidationLayerList)));
#endif

        if(validation_layer != nullptr){
            string error_layer = "none";
            if(!validation_layer->isSupport(error_layer))
                throw Log::Exception("validation layer not avaiable: " + error_layer, true);
            else
                Log::WriteTo().log(" Validation layers was pluged");
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = instanceInfo.aplicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(0,0,0);
        appInfo.pEngineName = "effort_engine";
        appInfo.engineVersion = VK_MAKE_VERSION(0,0,0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        auto extensions = getRequiredExtensions();

        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (validation_layer != nullptr) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layer->getLayersList().size());
            createInfo.ppEnabledLayerNames = validation_layer->getLayersList().data();
        } else 
            createInfo.enabledLayerCount = 0;
        
        if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
            throw eff::Log::Exception("can`t create VkInstance", true);

        if(validation_layer != nullptr)
            if(validation_layer->setInstanceCallback(instance) != VK_SUCCESS)
                throw eff::Log::Exception("can`t create callback of instance", true);
}


VulkanManager::VulkanInstance::~VulkanInstance(){
    validation_layer->deleteInstanceCallback(instance);
    vkDestroyInstance(instance, nullptr);
}


VulkanManager::VulkanInstance::ValidationLayer::ValidationLayer(string filename_VLlist){
    //TODO implement read Validation layer from list "ValidationLayers.data" to 
    //TODO get type of errors want to get
    flagsCallback = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
}

VkResult VulkanManager::VulkanInstance::ValidationLayer::setInstanceCallback(VkInstance& instance){

    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = flagsCallback;
    createInfo.pfnCallback = debugCallback;

    return CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback);
}


VkResult VulkanManager::VulkanInstance::ValidationLayer::CreateDebugReportCallbackEXT(
    VkInstance& instance, 
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, 
    VkDebugReportCallbackEXT* pCallback) 
{
    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pCallback);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;      
}


void VulkanManager::VulkanInstance::ValidationLayer::deleteInstanceCallback(VkInstance& instance){
    auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr)
            func(instance, callback, nullptr);
}


bool VulkanManager::VulkanInstance::ValidationLayer::isSupport(string& error_layer){
    
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    bool layerFound = 0;

    for (const char* layerName : validationLayersList) {
        layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
                error_layer = layerName;
                return false;
        }
    }

    return true;
}


const vector<const char*>& VulkanManager::VulkanInstance::ValidationLayer::getLayersList(){
    return this->validationLayersList;
}


vector<const char*> VulkanManager::VulkanInstance::getRequiredExtensions(){
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (validation_layer != nullptr) 
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    return extensions;        
}


VkInstance& VulkanManager::VulkanInstance::getInstance(){
    return this->instance;
}


VKAPI_ATTR VkBool32 VKAPI_CALL VulkanManager::VulkanInstance::ValidationLayer::debugCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t msgCode,
    const char* layerPrefix,
    const char* msg,
    void* userData)
{
    Log::WriteTo("log.log").log("was triggered vulkan debagCallback. type: " + string(layerPrefix),true);

    auto getInfo = [&]()->string{
        string info = "";
        info +=   ("\t\tobject type: " + to_string(objType));
        info += ("\n\t\tmessage: " + string(msg));
        return info;
    };

    switch(flags){
        case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
            Log::WriteTo().log(" vulkan: \n" + getInfo(), true);
            break;
        case VK_DEBUG_REPORT_WARNING_BIT_EXT:
            Log::WriteTo().warning(" vulkan: \n" + getInfo(),true);
            break;
        case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
            Log::WriteTo().warning(" vulkan: non optimal use.\n" + getInfo(),true);
            break;
        case VK_DEBUG_REPORT_ERROR_BIT_EXT:
            throw Log::Exception("vulkan: debugCallback\n" + getInfo(),true);
            break;
        case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
            Log::WriteTo().log(" vulkan: \n" + getInfo(), true);
            break;
        default:
            throw Log::Exception("underfined type of error: debugCallback\n" + getInfo(),true);
    }

    return VK_FALSE;
}

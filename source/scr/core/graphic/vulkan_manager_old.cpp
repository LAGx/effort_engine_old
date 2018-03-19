#include "core/graphic/vulkan_manager_old.h"
#include "service/log.h"
#include <cstring>
#include <memory>
#include <algorithm>
#include <map>
#include <set>
#include <list>

using namespace eff_old;
using namespace std;



VulkanManager::VulkanManager(const WindowCreateInfo& windowInfo,const InstanceCreateInfo& instanceInfo){

    window = move(unique_ptr<Window>(new Window(windowInfo)));
    instance = move(unique_ptr<VulkanInstance>(new VulkanInstance(instanceInfo)));
    window->setSurface(instance->getInstance());
    physicalDevice = move(unique_ptr<VulkanPhysicalDevice>(new VulkanPhysicalDevice(*instance.get())));
    logicalDevice = move(unique_ptr<VulkanLogicalDevice>(new VulkanLogicalDevice(this)));


}


VulkanManager::~VulkanManager(){
    logicalDevice.reset();
    physicalDevice.reset(nullptr);
    window->surface.reset(nullptr);
    instance.reset(nullptr);
    window.reset(nullptr);
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


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
            throw eff::Log::Exception("failed to create window surface! window name: " + window.info.name, true);
}


VulkanManager::Window::Surface::~Surface(){
    vkDestroySurfaceKHR(parentInstance, vk_surface, nullptr);
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


VulkanManager::VulkanInstance::VulkanInstance(const InstanceCreateInfo& instanceInfo){
#ifdef DEBUG_MODE
    validation_layer = move(unique_ptr<ValidationLayer>( new ValidationLayer(instanceInfo.filename_ValidationLayerList)));
#endif

        if(validation_layer != nullptr){
            string error_layer = "none";
            if(!validation_layer->isSupport(error_layer))
                throw eff::Log::Exception("validation layer not avaiable: " + error_layer, true);
            else
                eff::Log::WriteTo().log(" Validation layers was pluged");
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
    //flagsCallback = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
    flagsCallback = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;

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


bool VulkanManager::VulkanInstance::ValidationLayer::isSupport(string& error_layer) const{
    
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
    eff::Log::WriteTo("log.log").log("was triggered vulkan debagCallback. type: " + string(layerPrefix),true);

    auto getInfo = [&]()->string{
        string info = "";
        info +=   ("\t\tobject type: " + to_string(objType));
        info += ("\n\t\tmessage: " + string(msg));
        return info;
    };

    switch(flags){
        case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
            eff::Log::WriteTo().log(" vulkan: \n" + getInfo(), true);
            break;
        case VK_DEBUG_REPORT_WARNING_BIT_EXT:
            eff::Log::WriteTo().warning(" vulkan: \n" + getInfo(),true);
            break;
        case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
            eff::Log::WriteTo().warning(" vulkan: non optimal use.\n" + getInfo(),true);
            break;
        case VK_DEBUG_REPORT_ERROR_BIT_EXT:
            throw eff::Log::Exception("vulkan: debugCallback\n" + getInfo(),true);
            break;
        case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
            eff::Log::WriteTo().log(" vulkan: \n" + getInfo(), true);
            break;
        default:
            throw eff::Log::Exception("underfined type of error: debugCallback\n" + getInfo(),true);
    }

    return VK_FALSE;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


VulkanManager::VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanInstance& instance){

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.getInstance(), &deviceCount, nullptr);

    if (deviceCount == 0)
        throw eff::Log::Exception("failed to find GPUs with Vulkan support!");

    vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance.getInstance(), &deviceCount, devices.data());

    physicalDevice = *max_element(devices.begin(), devices.end(),[this](const VkPhysicalDevice& dv1, const VkPhysicalDevice& dv2){
        return rateDevice(dv1) < rateDevice(dv2);
    });

    if (rateDevice(physicalDevice) == 0) 
        throw eff::Log::Exception("failed to find a suitable GPU!");

}



VulkanManager::VulkanPhysicalDevice::rateDevicePoints VulkanManager::VulkanPhysicalDevice::rateDevice(const VkPhysicalDevice& device) const{

    rateDevicePoints ratePoints = 0;


//minimum requaied
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    bool isMinimumRequaiedSupport = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                                                                          deviceFeatures.geometryShader &&
                                                                          isExtentionSupported();

    if(isMinimumRequaiedSupport)
        ratePoints += 1;
    else
        return 0;


//rate system
    ratePoints += deviceProperties.limits.maxImageDimension2D;
    

    return ratePoints;
}


VkPhysicalDevice& VulkanManager::VulkanPhysicalDevice::getVkPhysicalDevice(){
    return physicalDevice;
}


bool VulkanManager::VulkanPhysicalDevice::isExtentionSupported() const{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    set<string> requiredExtensions(extentions.begin(), extentions.end());

    for (const auto& extension : availableExtensions) 
        requiredExtensions.erase(extension.extensionName);
    

    return requiredExtensions.empty();
}


const vector<const char*>& VulkanManager::VulkanPhysicalDevice::getExtentions() const{
    return extentions;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


VulkanManager::VulkanLogicalDevice::VulkanLogicalDevice(VulkanManager* _vkManager):queueFamilyes(_vkManager){
        main_vkManager = _vkManager;

        vector<VkDeviceQueueCreateInfo> queueCreateInfos = queueFamilyes.queueCreateInfos();

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        VkPhysicalDeviceFeatures deviceFeatures = {};
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(main_vkManager->physicalDevice->getExtentions().size());
        createInfo.ppEnabledExtensionNames = main_vkManager->physicalDevice->getExtentions().data();

        if (main_vkManager->instance->validation_layer != nullptr) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(main_vkManager->instance->validation_layer->getLayersList().size());
            createInfo.ppEnabledLayerNames = main_vkManager->instance->validation_layer->getLayersList().data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(main_vkManager->physicalDevice->getVkPhysicalDevice(), &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) 
            throw eff::Log::Exception("failed to create logical device!");
        
        queueFamilyes.getLogicalDeviceQueues(*this);
}


VulkanManager::VulkanLogicalDevice::QueueFamilyes::QueueFamilyes(VulkanManager* _vkManager):main_vkManager(_vkManager){
    findQueueFamilies();
}


void VulkanManager::VulkanLogicalDevice::QueueFamilyes::getLogicalDeviceQueues(VulkanLogicalDevice& _logicalDevice){
    for(auto& el: listQueue)
        vkGetDeviceQueue(_logicalDevice.logicalDevice, el.second.index, 0, &el.second.queue);
}

void VulkanManager::VulkanLogicalDevice::QueueFamilyes::findQueueFamilies(){

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(main_vkManager->physicalDevice->getVkPhysicalDevice(), &queueFamilyCount, nullptr);

    vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(main_vkManager->physicalDevice->getVkPhysicalDevice(), &queueFamilyCount, queueFamilies.data());

    int i = 0;

    for (const auto& queueFamily : queueFamilies) {
 
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            listQueue[EffQueueType::EFF_GRAPHIC_QUEUE].index = i;
    
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(main_vkManager->physicalDevice->getVkPhysicalDevice(), i, main_vkManager->window->surface->vk_surface, &presentSupport);

        if (queueFamily.queueCount > 0 && presentSupport) 
            listQueue[EffQueueType::EFF_PRESENT_QUEUE].index = i;

        i++;
    }

    for(auto& el: listQueue)
        if(el.second.index == -1)
            eff::Log::WriteTo("log.log").warning("not found queue: " + to_string((int)el.first), true);

}


VulkanManager::VulkanLogicalDevice::QueueFamilyes::Queue& VulkanManager::VulkanLogicalDevice::QueueFamilyes::operator[](const EffQueueType& type) {
    return listQueue[type];
}


VulkanManager::VulkanLogicalDevice::~VulkanLogicalDevice(){
    vkDestroyDevice(logicalDevice, nullptr);
}


vector<VkDeviceQueueCreateInfo> VulkanManager::VulkanLogicalDevice::QueueFamilyes::queueCreateInfos(){
    vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    //get vector of queues with dirrefent indexes
    vector<Queue> true_Queue_list;
    vector<int> was_idexes;

    for(const auto& el: listQueue)
        if(find(was_idexes.begin(), was_idexes.end(), el.second.index) == was_idexes.end()){
            was_idexes.push_back(el.second.index);
            true_Queue_list.push_back(el.second);
        }
    
    
    for(auto& el: true_Queue_list) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = true_Queue_list[0].index;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &true_Queue_list[0].priority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    return queueCreateInfos;
}
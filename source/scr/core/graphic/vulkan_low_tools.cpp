#include "core/graphic/vulkan_low_tools.h"
#include "core/graphic/glfw_low_tools.h"
#include "service/log.h"
#include "service/fileServing.h"
#include <set>

using namespace std;
using namespace eff;
using namespace eff_low;





//             SURFACE 


Surface::Surface(VkInstance& instance, GLFWwindow* window):parentInstance(instance){
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
            throw Log::Exception("failed to create surface!", true);
}



Surface::~Surface(){
    vkDestroySurfaceKHR(parentInstance, surface, nullptr);
}


//              VALIDATION LAYERS


ValidationLayers::ValidationLayers(const JsonSettings_SafeFileName& validationLayersFile){
    json js = JsonFile_fast::getFile(validationLayersFile);
    if(!js["isValidationLayersOn"])
        isHaveToExist_ = false;

    vector<string> string_layers = js["validationLayers"];
    for(auto ly: string_layers){
        char* ch_la = new char[ly.length()+1];
        strcpy(ch_la, ly.c_str());
        validationLayersList.push_back(ch_la);
    }

    //getting DEBUG_REPORT modes
    vector<string> debug_modes = js["flagsCallback"]; 
    set<VkDebugReportFlagsEXT> collect_flags;


    for(auto mode: debug_modes){
        if(mode == "VK_DEBUG_REPORT_INFORMATION_BIT_EXT")
            collect_flags.insert(VK_DEBUG_REPORT_INFORMATION_BIT_EXT);

        else if(mode == "VK_DEBUG_REPORT_WARNING_BIT_EXT")
            collect_flags.insert(VK_DEBUG_REPORT_WARNING_BIT_EXT);

        else if(mode == "VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT")
            collect_flags.insert(VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT);

        else if(mode == "VK_DEBUG_REPORT_ERROR_BIT_EXT")
            collect_flags.insert(VK_DEBUG_REPORT_ERROR_BIT_EXT);

        else if(mode == "VK_DEBUG_REPORT_DEBUG_BIT_EXT")
            collect_flags.insert(VK_DEBUG_REPORT_DEBUG_BIT_EXT);

        else
            throw Log::Exception("no such flagsCallback: " + mode,true);
    }


    for(auto fl: collect_flags)
        flagsCallback = flagsCallback | fl;


    string error_layer = "none";
    if(!isSupportAllLayers(error_layer))
        throw Log::Exception("validation layer not avaiable.\n\t\terror layer: " + error_layer, true);
    else
        Log::WriteTo().log(" Validation layers pluged in");


}



bool ValidationLayers::isSupportAllLayers(string& error_layer) const{
    
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



VkResult ValidationLayers::setInstanceCallback(VkInstance& instance){
    deleteInstanceCallback();

    parentInstance = &instance;
    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = flagsCallback;
    createInfo.pfnCallback = debugCallback;

    return CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback);
}



VKAPI_ATTR VkBool32 VKAPI_CALL  ValidationLayers::debugCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char* layerPrefix,
        const char* msg,
        void* userData
){
    
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



VkResult ValidationLayers::CreateDebugReportCallbackEXT(
        VkInstance &instance,
        const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugReportCallbackEXT *pCallback
){
    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pCallback);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT; 
}



void ValidationLayers::deleteInstanceCallback(){
    if(parentInstance != nullptr){
        auto _vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(*parentInstance, "vkDestroyDebugReportCallbackEXT");
        if (_vkDestroyDebugReportCallbackEXT != nullptr)
            _vkDestroyDebugReportCallbackEXT(*parentInstance, callback, nullptr);
        parentInstance = nullptr;
    }
}



const vector<const char *>& ValidationLayers::getLayersList(){
    return this->validationLayersList;
}



bool ValidationLayers::isHaveToExist() const{
    return isHaveToExist_;
}



ValidationLayers::~ValidationLayers(){
    deleteInstanceCallback();

    for(const char* ly : validationLayersList)
        delete [] ly;
}


//              INSTANCE


Instance::Instance(const CreateInfo& _createInfo){

 #ifdef DEBUG_MODE
    validation_layer = move(unique_ptr<ValidationLayers>(new ValidationLayers(_createInfo.filename_ValidationLayerList)));
    if(!validation_layer->isHaveToExist())
        validation_layer.reset(nullptr);
 #endif

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = _createInfo.aplicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION( (int)_createInfo.aplication_version.x, (int)_createInfo.aplication_version.y, (int)_createInfo.aplication_version.z);
    appInfo.pEngineName = _createInfo.engine_name.c_str();
    appInfo.engineVersion = VK_MAKE_VERSION( (int)_createInfo.engine_version.x, (int)_createInfo.engine_version.y, (int)_createInfo.engine_version.z);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    auto extensions = getRequiredGLFWExtensions();

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



VkInstance& Instance::getInstance(){
    return instance;
}



vector<const char*> Instance::getRequiredGLFWExtensions() const{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (validation_layer != nullptr) 
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    return extensions;        
}



Instance::~Instance(){
    validation_layer.reset(nullptr);
    vkDestroyInstance(instance, nullptr);
}


//               PHISICAL DEVICE


PhysicalDevice::PhysicalDevice(VkInstance& instance){

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0)
        throw eff::Log::Exception("failed to find GPUs with Vulkan support!");

    vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    physicalDevice = *max_element(devices.begin(), devices.end(),[this](const VkPhysicalDevice& dv1, const VkPhysicalDevice& dv2){
        return rateDevice(dv1) < rateDevice(dv2);
    });

    if (rateDevice(physicalDevice) == 0) 
        throw eff::Log::Exception("failed to find a suitable GPU!");

}



PhysicalDevice::rateDevicePoints PhysicalDevice::rateDevice(const VkPhysicalDevice& device) const{

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



VkPhysicalDevice& PhysicalDevice::getVkPhysicalDevice(){
    return physicalDevice;
}



bool PhysicalDevice::isExtentionSupported() const{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    set<string> requiredExtensions(extentions.begin(), extentions.end());

    for (const auto& extension : availableExtensions) 
        requiredExtensions.erase(extension.extensionName);
    

    return requiredExtensions.empty();
}



const vector<const char*>& PhysicalDevice::getExtentions() const{
    return extentions;
}

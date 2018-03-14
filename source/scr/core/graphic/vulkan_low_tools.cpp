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

ValidationLayers::ValidationLayers(VkInstance &instance, const JsonSettings_SafeFileName& validationLayersFile):parentInstance(instance){
    json js = JsonFile_fast::getFile(validationLayersFile);
    if(!js["isValidationLayersOn"])
        isHaveToExist_ = false;

    vector<string> string_layers = js["validationLayers"];
    for(auto ly: string_layers)
        validationLayersList.push_back(ly.c_str());

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

}


bool ValidationLayers::isHaveToExit(){
    return isHaveToExist_;
}


ValidationLayers::~ValidationLayers(){
    auto _vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(parentInstance, "vkDestroyDebugReportCallbackEXT");
    if (_vkDestroyDebugReportCallbackEXT != nullptr)
        _vkDestroyDebugReportCallbackEXT(parentInstance, callback, nullptr);
}

//              INSTANCE



Instance::Instance(const CreateInfo& _createInfo){
 #ifdef DEBUG_MODE
    validation_layer = move(unique_ptr<ValidationLayers>(new ValidationLayers(instance, _createInfo.filename_ValidationLayerList)));
    if(!validation_layer->isHaveToExit())
        validation_layer.reset(nullptr);
 #endif
/*
    if(validation_layer != nullptr){
        string error_layer = "none";
        if(!validation_layer->isSupport(error_layer))
            throw Log::Exception("validation layer not avaiable: " + error_layer, true);
        else
            Log::WriteTo().log(" Validation layers pluged in");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = _createInfo.aplicationName.c_str();
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
*/
}



VkInstance& Instance::getInstance(){
    return instance;
}


vector<const char*> Instance::getRequiredExtensions(){
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


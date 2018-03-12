#include "core/graphic/vulkan_low_tools.h"
#include "core/graphic/glfw_low_tools.h"
#include "service/log.h"


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

ValidationLayers::ValidationLayers(JsonSettings_FileName validationLayersFile, VkInstance &instance):parentInstance(instance){
    
}

ValidationLayers::ValidationLayers(){
    auto _vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (_vkDestroyDebugReportCallbackEXT != nullptr)
        _vkDestroyDebugReportCallbackEXT(instance, callback, nullptr);
}

//              INSTANCE



Instance::Instance(const CreateInfo& CreateInfo){
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



VkInstance& Instance::getInstance(){
    return instance;
}


Instance::~Instance(){
    validation_layer->deleteInstanceCallback(instance);
    vkDestroyInstance(instance, nullptr);
}
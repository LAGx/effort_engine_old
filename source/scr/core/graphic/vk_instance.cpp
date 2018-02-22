#include "core/graphic/vk_instance.h"
#include "service/log.h"
#include <cstring>

using namespace std;
using namespace eff;



VulkanInstance::VulkanInstance(string aplicationName){
        
        if(validation_layer != nullptr){
            string error_layer = "none";
            if(!validation_layer->isSupport(error_layer))
                throw Log::Exception("validation layer not avaiable: " + error_layer, true);
            else
                Log::WriteTo().log(" Validation layers was pluged");
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = aplicationName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(0,0,0);
        appInfo.pEngineName = "effort_engine";
        appInfo.engineVersion = VK_MAKE_VERSION(0,0,0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        auto extensions = getRequiredExtensions  ();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (validation_layer != nullptr) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layer->getLayersList().size());
            createInfo.ppEnabledLayerNames = validation_layer->getLayersList().data();
        } else 
            createInfo.enabledLayerCount = 0;
        
        if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
            throw eff::Log::Exception("can`t create VkInstance", true);
}


VulkanInstance::~VulkanInstance(){
    vkDestroyInstance(instance, nullptr);
}


VulkanInstance::ValidationLayer::ValidationLayer(string filename_VLlist){
    //TODO implement read Validation layer from list "ValidationLayers.data" to 
}


bool VulkanInstance::ValidationLayer::isSupport(string& error_layer){
    
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


const vector<const char*>& VulkanInstance::ValidationLayer::getLayersList(){
    return this->validationLayersList;
}


vector<const char*> VulkanInstance::getRequiredExtensions(){
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (validation_layer != nullptr) 
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    return extensions;
}


const VkInstance& VulkanInstance::getInstance(){
    return this->instance;
}
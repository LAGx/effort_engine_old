#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include "service/fileServing.h"

using namespace std;

/*! 
    @brief namespace of engine
*/
namespace eff
{
/*! 
    @brief namespace for engine developers
    @warning include complex tool, that have to work in complex configuration
    @warning don`t use as part of user interface engine
*/
namespace eff_low
{


/*!
    @breif class of surface
    @warning instance must exist while class exist for correct deleting surface
*/
class Surface{
public:

    Surface(VkInstance &instance, GLFWwindow *window);

    ~Surface();

protected:

    VkInstance &parentInstance;
    VkSurfaceKHR surface;

};



/*!
    @breif class of surface
    @warning instance must exist while class exist for correct deleting layers
*/
class ValidationLayers{
public:

    /*!
        @warning json file have to include:
        "isValidationLayersOn": true,
        "validationLayers": [],
        "flagsCallback":[]     <-- there can be ["VK_DEBUG_REPORT_ERROR_BIT_EXT", 
                                                 "VK_DEBUG_REPORT_WARNING_BIT_EXT", 
                                                 "VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT", 
                                                 "VK_DEBUG_REPORT_ERROR_BIT_EXT", 
                                                 "VK_DEBUG_REPORT_DEBUG_BIT_EXT"]
    
    */
    ValidationLayers(VkInstance &instance, const JsonSettings_SafeFileName& validationLayersFile = "resource/data/VulkanValidationLayers.json");

    bool isSupport(string &error_layer) const;
    const vector<const char *> &getLayersList();

    bool isHaveToExit();

    ~ValidationLayers();

private:

    bool isHaveToExist_ = true;

    VkInstance &parentInstance;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char *layerPrefix,
        const char *msg,
        void *userData
    );

    vector<const char *> validationLayersList = {
      //  "VK_LAYER_LUNARG_standard_validation" //TODO fill vector only with constructor
    };


    VkDebugReportFlagsEXT flagsCallback;

    VkDebugReportCallbackEXT callback;

    VkResult CreateDebugReportCallbackEXT(
        VkInstance &instance,
        const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugReportCallbackEXT *pCallback
    );

};



class Instance{ //TODO
public:
    /*!
        @brief create instance struct

        @param aplicationName {name of application}
        @param filename_ValidationLayerList {path to lua file, that contain list of validation list}
    */
    struct CreateInfo{
        string aplicationName = "effort_engine";
 #ifdef WIN_OS
        JsonSettings_SafeFileName filename_ValidationLayerList = "resource\data\VulkanValidationLayers.json";
 #else
        JsonSettings_SafeFileName filename_ValidationLayerList = "resource/data/VulkanValidationLayers.json";
 #endif
    };

    Instance(const CreateInfo &CreateInfo);

    VkInstance &getInstance();

    ~Instance();

protected:

    VkInstance instance;
    unique_ptr<ValidationLayers> validation_layer = nullptr;

    vector<const char*> getRequiredExtensions();
};


class PhysicalDevice{ //TODO
public:

protected:

};

class QueueFamilyes{ //TODO
public:

protected:
    
    struct Queue{
        int index = -1;
        VkQueue queue = nullptr;
        float priority = 1.0f;
    };

};

class LogicalDevice{ //TODO
public:

protected:

};


};
};
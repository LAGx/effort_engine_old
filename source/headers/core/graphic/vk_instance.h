#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <vector>

using namespace std;

namespace eff{


class VulkanInstance{
public:

    VulkanInstance(string aplicationName = "effort_engine");

    const VkInstance& getInstance();

    ~VulkanInstance();

private:

    VkInstance instance;
    
    vector<const char*> getRequiredExtensions();//WTF???

    struct ValidationLayer{
        ValidationLayer(string filename_VLlist = "ValidationLayers.data");

        bool isSupport(string& error_layer);
        const vector<const char*>& getLayersList();
    
    private:

        vector<const char*> validationLayersList = {
            "VK_LAYER_LUNARG_standard_validation" //TODO fill vector only with constructor
        };
    
    };
    
#ifdef DEBUG_MODE
    unique_ptr<ValidationLayer> validation_layer = unique_ptr<ValidationLayer>(new ValidationLayer());
#elif RELEASE_MODE
    unique_ptr<ValidationLayer> validation_layer = nullptr;
#endif

};





};
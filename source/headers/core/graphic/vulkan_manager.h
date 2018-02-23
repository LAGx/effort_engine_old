#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <glm/vec2.hpp>
#include <vector>


using namespace std;


namespace eff{


class VulkanManager{
public:

    /*!
        @brief create window struct

        @param name {name of window}
        @param size {size of window (by default {0,0} is fullscreen)}
        @param resolution {resolution of window. work on fullscreen mode. {0,0}-default resolution}
        @param isResizable {is can window be resize (don`t work in fullscreen mode)}
        @param surfaceInstance {need VkInstance to create surface}
    */
    struct WindowCreateInfo{
        string name = "effort_engine_window";
        glm::vec2 size = {0,0};
        glm::vec2 resolution = {0,0};
        bool isResizable = true;
    };

    /*!
        @brief create instance struct

        @param aplicationName {name of application}
        @param filename_ValidationLayerList {path to lua file, where contain list of validation list}
    */
    struct InstanceCreateInfo{
        string aplicationName = "effort_engine";
#ifdef WIN_OS
        string filename_ValidationLayerList = "resource\data\ValidationLayers.data";
#else
        string filename_ValidationLayerList = "resource/data/ValidationLayers.data";
#endif
    };



    VulkanManager(const WindowCreateInfo& windowInfo,const InstanceCreateInfo& instanceInfo);



    ~VulkanManager();

private:


    class Window{
    friend VulkanManager;
    public:
        
        void setSurface(VkInstance& _surfaceInstance);

        Window(const VulkanManager::WindowCreateInfo& info);
        
        
        ~Window();


    private:
        

        struct Surface{
            Surface(VkInstance& instance, Window& window);
            VkInstance& parentInstance;
            VkSurfaceKHR vk_surface;
            ~Surface();
        };

        WindowCreateInfo info;

        GLFWwindow* g_window = nullptr;
        unique_ptr<Surface> surface;

    };


    class VulkanInstance{
    friend VulkanManager;
    public:

        VulkanInstance(const InstanceCreateInfo& instanceInfo);

        VkInstance& getInstance();

        ~VulkanInstance();

    private:

        struct ValidationLayer{
            ValidationLayer(string filename_VLlist);

            bool isSupport(string& error_layer);
            const vector<const char*>& getLayersList();
        
        private:

            vector<const char*> validationLayersList = {
                "VK_LAYER_LUNARG_standard_validation" //TODO fill vector only with constructor
            };
        
        };

        VkInstance instance;

        vector<const char*> getRequiredExtensions();

        unique_ptr<ValidationLayer> validation_layer = nullptr;

    };


    unique_ptr<Window> window = nullptr;
    unique_ptr<VulkanInstance> instance = nullptr;



};

};
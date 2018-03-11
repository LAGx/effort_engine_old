#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <glm/vec2.hpp>
#include <vector>
#include <map>

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
        string filename_ValidationLayerList = "resource\data\VulkanSettings.data";
#else
        string filename_ValidationLayerList = "resource/data/VulkanSettings.data";
#endif
    };



    VulkanManager(const WindowCreateInfo& windowInfo,const InstanceCreateInfo& instanceInfo);



    ~VulkanManager();

protected:


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

        class ValidationLayer{
        public:

            ValidationLayer(string filename_VLlist);

            bool isSupport(string& error_layer) const;
            const vector<const char*>& getLayersList();

            VkResult setInstanceCallback(VkInstance& instance);
            void deleteInstanceCallback(VkInstance& instance);

        private:

            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugReportFlagsEXT flags,
                VkDebugReportObjectTypeEXT objType,
                uint64_t obj,
                size_t location,
                int32_t code,
                const char* layerPrefix,
                const char* msg,
                void* userData);

            vector<const char*> validationLayersList = {
                "VK_LAYER_LUNARG_standard_validation" //TODO fill vector only with constructor
            };

            VkDebugReportFlagsEXT flagsCallback;

            VkDebugReportCallbackEXT callback;

            VkResult CreateDebugReportCallbackEXT(
                VkInstance& instance, 
                const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, 
                const VkAllocationCallbacks* pAllocator, 
                VkDebugReportCallbackEXT* pCallback
            );   

        };

        VkInstance instance;

        vector<const char*> getRequiredExtensions();

        unique_ptr<ValidationLayer> validation_layer = nullptr;

    };


    class VulkanPhysicalDevice{
    public:

        VulkanPhysicalDevice(VulkanInstance& instance);

        VkPhysicalDevice& getVkPhysicalDevice();
        
        const vector<const char*>& getExtentions() const;

    private:

        vector<const char*> extentions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };

        bool isExtentionSupported() const;

        //TODO more complex raiting
        typedef size_t rateDevicePoints;
        rateDevicePoints rateDevice(const VkPhysicalDevice& device) const;//< if return 0, then this device don`t suppurt Vulkan

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    };


    class VulkanLogicalDevice{
    public:

        VulkanLogicalDevice(VulkanManager* _vkManager);//TODO what is device features?

        ~VulkanLogicalDevice();

    private:

        

        class QueueFamilyes{
        public:
            

            enum EffQueueType{
                EFF_GRAPHIC_QUEUE = 0,
                EFF_PRESENT_QUEUE
            };


            struct Queue{
                //Queue(const Queue& q);
                
                int index = -1;
                VkQueue queue = nullptr;
                float priority = 1.0f;
            };


            QueueFamilyes(VulkanManager* _vkManager);

            vector<VkDeviceQueueCreateInfo> queueCreateInfos(); 

            Queue& operator[](const EffQueueType& type);

            void getLogicalDeviceQueues(VulkanLogicalDevice& _logicalDevice);

        private:


            VulkanManager* main_vkManager;


            map<EffQueueType, Queue> listQueue = {  {EFF_GRAPHIC_QUEUE, Queue()},
                                                    {EFF_PRESENT_QUEUE, Queue()}};

            void findQueueFamilies(); //set indexs


        };

        VulkanManager* main_vkManager;
        QueueFamilyes queueFamilyes;
        VkDevice logicalDevice;

    };


    unique_ptr<Window>                 window           = nullptr;
    unique_ptr<VulkanInstance>         instance         = nullptr;
    unique_ptr<VulkanPhysicalDevice>   physicalDevice   = nullptr;
    unique_ptr<VulkanLogicalDevice>    logicalDevice    = nullptr;

};

};
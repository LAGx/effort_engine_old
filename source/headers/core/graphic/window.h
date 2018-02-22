#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <glm/vec2.hpp>
#include <memory>

using namespace std;

namespace eff{


class Window{
public:

    /*!
        @brief create window

        @param name {name of window}
        @param size {size of window (by default {0,0} is fullscreen)}
        @param resolution {resolution of window. work on fullscreen mode. {0,0}-default resolution}
        @param isResizable {is can window be resize (don`t work in fullscreen mode)}
        @param surfaceInstance {need VkInstance to create surface}
    */
    struct WindowCreateInfo{
        WindowCreateInfo(const VkInstance& srVk_instance);
        const VkInstance& surfaceInstance;
        
        string name = "effort_engine_window";
        glm::vec2 size = {0,0};
        glm::vec2 resolution = {0,0};
        bool isResizable = true;
    };
    

    Window(const WindowCreateInfo& info);
    
    
    ~Window();


private:
    

    struct Surface{
        Surface(const VkInstance& instance, Window& window);
        
        const VkInstance& parentInstance;
        VkSurfaceKHR vk_surface;
        ~Surface();
    };
    

    WindowCreateInfo info;

    GLFWwindow* g_window = nullptr;
    unique_ptr<Surface> surface;

};

};
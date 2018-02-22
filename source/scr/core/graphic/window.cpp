#include "core/graphic/window.h"
#include "service/log.h"

using namespace eff;
using namespace std;


Window::Window(const WindowCreateInfo& _info):info(_info){
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
            this->g_window = unique_ptr<GLFWwindow>(glfwCreateWindow(videoMode->width, videoMode->height, info.name.c_str(), monitor, NULL));
        else    
            this->g_window = unique_ptr<GLFWwindow>(glfwCreateWindow(info.resolution.x, info.resolution.y, info.name.c_str(), monitor, NULL));

    }else{
    
        if(info.isResizable)
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
        this->g_window = unique_ptr<GLFWwindow>(glfwCreateWindow(info.size.x, info.size.y, info.name.c_str() , nullptr, nullptr));
    }

    surface = unique_ptr<Surface>( new Surface(info.surfaceInstance, *this));
}


Window::~Window(){
    surface.reset();
    glfwDestroyWindow(g_window.get());
    glfwTerminate();
}


Window::Surface::Surface(const VkInstance& instance,const Window& window):parentInstance(instance){
    if (glfwCreateWindowSurface(instance, window.g_window.get(), nullptr, &vk_surface) != VK_SUCCESS)
            throw Log::Exception("failed to create window surface! window name: " + window.info.name, true);
}


Window::Surface::~Surface(){
    vkDestroySurfaceKHR(parentInstance, vk_surface, nullptr);
}
#include "core/graphic/glfw_low_tools.h"


using namespace std;
using namespace eff;
using namespace eff_low;


Window::Window(const CreateInfo& createInfo){
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
            this->g_window = glfwCreateWindow(videoMode->width, videoMode->height, info.name.c_str(), monitor, NULL);
        else    
            this->g_window = glfwCreateWindow(info.resolution.x, info.resolution.y, info.name.c_str(), monitor, NULL);

    }else{
    
        if(info.isResizable)
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
        this->g_window = glfwCreateWindow(info.size.x, info.size.y, info.name.c_str() , nullptr, nullptr);
    }
}


const Window::CreateInfo& Window::getInfo(){
    return info;
}


void Window::setSurfaceInstance(VkInstance& surfaceInstance){
    surface.reset(new Surface(surfaceInstance, g_window));
}


Window::~Window(){
    glfwDestroyWindow(g_window);
    glfwTerminate();
}



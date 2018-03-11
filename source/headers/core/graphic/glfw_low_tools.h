#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "core/graphic/vulkan_low_tools.h"
#include <string>
#include <glm/vec2.hpp>
#include <memory>

using namespace std;


namespace eff{
namespace eff_low{


/// class of glfw window and vulkan
class Window{
public:


    /*!
        @brief create window struct

        @param name {name of window}
        @param size {size of window (by default {0,0} is fullscreen)}
        @param resolution {resolution of window. work on fullscreen mode. {0,0}-default resolution}
        @param isResizable {is can window be resize (don`t work in fullscreen mode)}
    */
    struct CreateInfo{
        string name = "effort_engine_window";
        glm::vec2 size = {0,0};
        glm::vec2 resolution = {0,0};
        bool isResizable = true;
    };



    Window(const CreateInfo& createInfo);


    /*!
        @brief must to be using after constructor.
        delete old surface and create new based on new instance.
        @warning old instance must exist for correct deleting old surface
    */
    void setSurfaceInstance(VkInstance& surfaceInstance);


    /*! 
        @brief get structure that used for init class
    */
    const CreateInfo& getInfo();

    ~Window();

protected:


    CreateInfo info;
    GLFWwindow* g_window = nullptr;
    unique_ptr<Surface> surface = nullptr;


};



};
};
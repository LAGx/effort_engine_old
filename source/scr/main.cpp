#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>

#include "core/graphic/vulkan_manager.h"
#include "service/log.h"

using namespace eff;
using namespace std;


int main(){
    Log::WriteTo("error.log").clear();
    Log::WriteTo("log.log").clear();


    VulkanManager::WindowCreateInfo windowInfo;
    windowInfo.isResizable = true;
    windowInfo.size = glm::vec2({900,500});
    
    VulkanManager::InstanceCreateInfo instanceInfo;

    VulkanManager vk_manager(windowInfo, instanceInfo);


    return EXIT_SUCCESS;
}
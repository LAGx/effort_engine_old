#pragma once

#include "core/graphic/glfw_low_tools.h"
#include "core/graphic/vulkan_low_tools.h"

using namespace eff::eff_low;
using namespace std;

namespace eff{


/*!
    @brief tool for high level control vulkan

    @warning it`s have to be only onc object
    @warning object is very havy
*/
class VulkanManager{
public:

    typedef eff_low::Window::CreateInfo WindowCreateInfo;
    typedef eff_low::Instance::CreateInfo InstanceCreateInfo;


    VulkanManager(const WindowCreateInfo& windowCreateInfo, const InstanceCreateInfo& instanceCreateInfo);

    ~VulkanManager();

protected:

    unique_ptr<Window>           window           = nullptr;
    unique_ptr<Instance>         instance         = nullptr;
    unique_ptr<PhysicalDevice>   physicalDevice   = nullptr;
    unique_ptr<LogicalDevice>    logicalDevice    = nullptr;

};



};
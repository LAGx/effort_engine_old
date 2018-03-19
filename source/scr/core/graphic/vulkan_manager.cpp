#include "core/graphic/vulkan_manager.h"
#include <memory>


using namespace std;
using namespace eff;
using namespace eff_low;



VulkanManager::VulkanManager(const WindowCreateInfo& windowCreateInfo, const InstanceCreateInfo& instanceCreateInfo){

    window = move(unique_ptr<Window>(new Window(windowCreateInfo)));
    instance = move(unique_ptr<Instance>(new Instance(instanceCreateInfo)));
    window->setSurfaceInstance(instance->getInstance());
    
}


VulkanManager::~VulkanManager(){
    window.reset(nullptr);
    instance.reset(nullptr);
}
#pragma once
/**
 * @author Simon Brisebois-Therrien
 * @since 2021-09-20 
 * 
 * 
 */
#include <iostream>
#include <optional>
#include <vector>
#include <set>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


namespace VulkanGameEngine
{
    namespace Utils
    {
        const inline std::vector<const char*> device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME  
        };

        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphics_family;
            std::optional<uint32_t> present_family;

            bool is_complete();
        };

        QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR& surface);

        bool check_device_extension_support(VkPhysicalDevice device);

        bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR& surface);

    };
};
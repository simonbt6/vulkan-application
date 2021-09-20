#include "swapchain.hpp"

namespace VulkanGameEngine
{
    namespace Utils
    {
        SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device, VkSurfaceKHR& surface)
        {
            SwapChainSupportDetails details;

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

            uint32_t format_count;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

            if (format_count != 0)
            {
                details.formats.resize(format_count);
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
            }

            uint32_t present_mode_count;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);

            if (present_mode_count != 0)
            {
                details.present_modes.resize(present_mode_count);
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes.data());
            }

            return details;
        }

        VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats)
        {
            for (const VkSurfaceFormatKHR& available_format : available_formats)
                if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB 
                    && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    return available_format;

            return available_formats[0];
        }

        VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes)
        {
            for (const VkPresentModeKHR& available_present_mode : available_present_modes)
                if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
                    return available_present_mode;

            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
        {
            if (capabilities.currentExtent.width != UINT32_MAX)
                return capabilities.currentExtent;
            else
            {
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);

                VkExtent2D actual_extent = {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
                };

                actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

                return actual_extent;
            }
        }
    };
};
#include "queuefamily.hpp"
#include "swapchain.hpp"

namespace VulkanGameEngine
{
    namespace Utils
    {

        bool QueueFamilyIndices::is_complete()
        {
            return graphics_family.has_value() && present_family.has_value();
        }

        QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR& surface)
        {
            QueueFamilyIndices indices;

            uint32_t queue_family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

            std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

            int i = 0;
            for (const auto& queue_family : queue_families)
            {
                VkBool32 present_support = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);

                if (present_support)
                    indices.present_family = i;

                if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    indices.graphics_family = i;

                if (indices.is_complete())
                    break;

                i++;
            }

            return indices;
        }
        
        bool check_device_extension_support(VkPhysicalDevice device)
        {
            uint32_t extension_count;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

            std::vector<VkExtensionProperties> available_extensions(extension_count);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

            std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

            for (const auto& extension : available_extensions)
            {
                required_extensions.erase(extension.extensionName);
            }

            return required_extensions.empty();
        }

        bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR& surface)
        {
            QueueFamilyIndices indices = find_queue_families(device, surface);
            
            bool extensions_supported = check_device_extension_support(device);
            bool swap_chain_adequate = false;

            if (extensions_supported)
            {
                SwapChainSupportDetails swap_chain_support = query_swap_chain_support(device, surface);
                swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
            }

            return indices.is_complete() && extensions_supported && swap_chain_adequate;
        }

    };
}
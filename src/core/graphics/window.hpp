#pragma once
/**
 * @author Simon Brisebois-Therrien
 * @since 2021-09-20 
 * 
 * 
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "../utils/queuefamily.hpp"
#include "../utils/swapchain.hpp"



namespace VulkanGameEngine 
{
    namespace Graphics
    {
        class Window
        {
            private:
                /**
                 * Class members.
                 */
                GLFWwindow* window;

                VkInstance instance;

                VkDebugUtilsMessengerEXT debug_messenger;

                VkDevice device;

                VkPhysicalDevice physical_device = VK_NULL_HANDLE;

                VkQueue graphics_queue;
                VkQueue present_queue;

                VkSurfaceKHR surface;

                VkSwapchainKHR swapchain;
                std::vector<VkImage> swapchain_images;
                VkFormat swapchain_image_format;
                VkExtent2D swapchain_extent;
                std::vector<VkImageView> swapchain_image_views;

                /**
                 * GLFW window properties.
                 */
                std::string window_title;
                uint32_t w_Height;
                uint32_t w_Width;

                const std::vector<const char*> validation_layers = {
                    "VK_LAYER_KHRONOS_validation"
                };

                #ifdef NDEBUG
                    const bool enable_validation_layers = false;
                #else
                    const bool enable_validation_layers = true;
                #endif

                


            public:
                /**
                 * Public methods.
                 */
                Window(std::string window_title = "Default window name.", uint32_t width = 800, uint32_t height = 600);

            private:
                /**
                 * Private methods.
                 */
                void init_Window();

                void init_vulkan();

                void main_loop();

                void cleanup();

                void create_instance();

                void create_surface();

                void create_swap_chain();

                void create_image_views();

                void pick_physical_device();

                bool is_device_suitable(VkPhysicalDevice device);

                void create_logical_device();

                bool check_validation_layer_support();

                std::vector<const char*> get_required_extensions();

                void setup_debug_messenger();

                void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);

                void destroy_debug_messenger(
                    VkInstance instance,
                    VkDebugUtilsMessengerEXT debug_messenger,
                    const VkAllocationCallbacks* p_allocator);

                VkResult create_debug_utils_messenger_ext(
                    VkInstance instance, 
                    const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
                    const VkAllocationCallbacks* p_allocator,
                    VkDebugUtilsMessengerEXT* p_debug_messenger);

                static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
                    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                    VkDebugUtilsMessageTypeFlagsEXT message_type,
                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                    void* pUserData);

        };
    }
}
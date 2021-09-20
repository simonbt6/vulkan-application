#include "window.hpp"
/**
 * @author Simon Brisebois-Therrien 
 * @since 2021-09-20 
 * 
 * 
 */

namespace VulkanGameEngine
{
    namespace Graphics
    {
        Window::Window(std::string window_title, uint32_t width, uint32_t height)
        {
            this->window_title = window_title;
            this->w_Width = width;
            this->w_Height = height;

            this->init_Window();
            this->init_vulkan();
            this->main_loop();
            this->cleanup();
            
        }

        void Window::init_Window()
        {
            if(!glfwInit())
                throw std::runtime_error("\nFailed to init glfw.");
            
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            window = glfwCreateWindow(w_Width, w_Height, window_title.c_str(), nullptr, nullptr);

            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

            printf("Supported extensions: %i", extensionCount);
        }

        void Window::init_vulkan()
        {
            this->create_instance();
            this->setup_debug_messenger();
            this->create_surface();
            this->pick_physical_device();
            this->create_logical_device();
            this->create_swap_chain();
            this->create_image_views();
        }

        void Window::main_loop()
        {
            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();
            }
        }

        void Window::cleanup()
        {
            for (auto imageView : swapchain_image_views)
                vkDestroyImageView(device, imageView, nullptr);

            vkDestroySwapchainKHR(device, swapchain, nullptr);

            vkDestroyDevice(device, nullptr);

            if (enable_validation_layers)
                destroy_debug_messenger(instance, debug_messenger, nullptr);

            vkDestroySurfaceKHR(instance, surface, nullptr);
            vkDestroyInstance(instance, nullptr);

            glfwDestroyWindow(window);

            glfwTerminate();
        }

        void Window::create_instance()
        {
            if (enable_validation_layers && !check_validation_layer_support())
                throw std::runtime_error("\nValidation layers requested, but not available!");

            VkApplicationInfo app_info{};
            app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            app_info.pApplicationName = "Default Application name.";
            app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            app_info.pEngineName = "VulkanGameEngine";
            app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            app_info.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            create_info.pApplicationInfo = &app_info;


            uint32_t glfwExtensionCount = 0;
            
            auto extensions = get_required_extensions();
            create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            create_info.ppEnabledExtensionNames = extensions.data();

            VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
            if (enable_validation_layers)
            {
                create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
                create_info.ppEnabledLayerNames = validation_layers.data();

                populate_debug_messenger_create_info(debug_create_info);
                create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
            }
            else
            {
                create_info.enabledLayerCount = 0;
                create_info.pNext = nullptr;
            }

            if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS)
                throw std::runtime_error("\nFailed to create instance.");
        }

        void Window::create_surface()
        {
            if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
                throw std::runtime_error("\nFailed to create window surface.");
            
        }

        void Window::create_swap_chain()
        {
            Utils::SwapChainSupportDetails swap_chain_support = Utils::query_swap_chain_support(physical_device, surface);

            VkSurfaceFormatKHR surface_format = Utils::choose_swap_surface_format(swap_chain_support.formats);
            VkPresentModeKHR present_mode = Utils::choose_swap_present_mode(swap_chain_support.present_modes);
            VkExtent2D extent = Utils::choose_swap_extent(swap_chain_support.capabilities, window);

            int extra_image = 1;
            uint32_t image_count = swap_chain_support.capabilities.minImageCount + extra_image;

            if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount)
                image_count = swap_chain_support.capabilities.maxImageCount;

            VkSwapchainCreateInfoKHR create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            create_info.surface = surface;

            create_info.minImageCount = image_count;
            create_info.imageFormat = surface_format.format;
            create_info.imageColorSpace = surface_format.colorSpace;
            create_info.imageExtent = extent;
            create_info.imageArrayLayers = 1;
            create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            Utils::QueueFamilyIndices indices = Utils::find_queue_families(physical_device, surface);
            uint32_t queue_family_indices[] = {indices.graphics_family.value(), indices.present_family.value()};

            if (indices.graphics_family != indices.present_family)
            {
                create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                create_info.queueFamilyIndexCount = 2;
                create_info.pQueueFamilyIndices = queue_family_indices;
            }
            else
            {
                create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                create_info.queueFamilyIndexCount = 0;
                create_info.pQueueFamilyIndices = nullptr;
            }

            create_info.preTransform = swap_chain_support.capabilities.currentTransform;
            create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            create_info.presentMode = present_mode;
            create_info.clipped = VK_TRUE;
            create_info.oldSwapchain = VK_NULL_HANDLE;

            if (vkCreateSwapchainKHR(device, &create_info, nullptr, &swapchain))
                throw std::runtime_error("Failed to create swap chain.");

            vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);
            swapchain_images.resize(image_count);
            vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchain_images.data());

            swapchain_image_format = surface_format.format;
            swapchain_extent = extent;
        }

        void Window::create_image_views()
        {
            swapchain_image_views.resize(swapchain_images.size());

            for (size_t i = 0; i < swapchain_images.size(); i++)
            {
                VkImageViewCreateInfo create_info{};
                create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                create_info.image = swapchain_images[i];
                create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
                create_info.format = swapchain_image_format;

                create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

                create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                create_info.subresourceRange.baseMipLevel = 0;
                create_info.subresourceRange.levelCount = 1;
                create_info.subresourceRange.baseArrayLayer = 0;
                create_info.subresourceRange.layerCount = 1;

                if (vkCreateImageView(device, &create_info, nullptr, &swapchain_image_views[i]) != VK_SUCCESS)
                    throw std::runtime_error("\nFailed to create image views.");
            }
        }

        void Window::pick_physical_device()
        {

            uint32_t device_count = 0;
            vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

            if (device_count == 0)
                throw std::runtime_error("\nFailed to find GPUs with Vulkan support!");

            std::vector<VkPhysicalDevice> devices(device_count);
            vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

            for (const auto& device : devices) 
            {
                if (is_device_suitable(device))
                {
                    physical_device = device;
                    break;
                }
            }

            if (physical_device == VK_NULL_HANDLE)
                throw std::runtime_error("\nFailed to find a suitable GPU!");
        }

        bool Window::is_device_suitable(VkPhysicalDevice device)
        {
            VkPhysicalDeviceProperties device_properties;
            VkPhysicalDeviceFeatures device_features;

            vkGetPhysicalDeviceProperties(device, &device_properties);
            vkGetPhysicalDeviceFeatures(device, &device_features);

            return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                device_features.geometryShader;
        }

        void Window::create_logical_device()
        {
            Utils::QueueFamilyIndices indices = Utils::find_queue_families(physical_device, surface);

            std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
            std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};

            float queue_priority = 1.0f;
            for (uint32_t queue_family : unique_queue_families)
            {
                VkDeviceQueueCreateInfo queue_create_info{};
                queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue_create_info.queueFamilyIndex = queue_family;
                queue_create_info.queueCount = 1;
                queue_create_info.pQueuePriorities = &queue_priority;
                queue_create_infos.push_back(queue_create_info);
            }

            VkPhysicalDeviceFeatures device_features{};
            
            VkDeviceCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

            create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
            create_info.pQueueCreateInfos = queue_create_infos.data();
            
            create_info.pEnabledFeatures = &device_features;
            
            create_info.enabledExtensionCount = static_cast<uint32_t>(Utils::device_extensions.size());
            create_info.ppEnabledExtensionNames = Utils::device_extensions.data();

            if (enable_validation_layers)
            {
                create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
                create_info.ppEnabledLayerNames = validation_layers.data();
            }
            else
            {
                create_info.enabledLayerCount = 0;
            }

            if (vkCreateDevice(physical_device, &create_info, nullptr, &device))
                throw std::runtime_error("\nFailed to create logical device.");

            vkGetDeviceQueue(device, indices.graphics_family.value(), 0, &graphics_queue);
            vkGetDeviceQueue(device, indices.present_family.value(), 0, &present_queue);
        }

        bool Window::check_validation_layer_support()
        {
            uint32_t layer_count;
            vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

            std::vector<VkLayerProperties> available_layers;
            vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

            for (const char* layer_name : validation_layers)
            {
                bool layer_found;
                for (const auto& layer_properties : available_layers)
                    if (strcmp(layer_name, layer_properties.layerName) == 0)
                    {
                        layer_found = true;
                        break;
                    }
                if (!layer_found)
                    return false;
            }
            return true;
        }

        std::vector<const char*> Window::get_required_extensions()
        {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;

            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

            if (enable_validation_layers)
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            return extensions;
        }

        void Window::setup_debug_messenger()
        {
            if (!enable_validation_layers) return;

            VkDebugUtilsMessengerCreateInfoEXT create_info{};
            populate_debug_messenger_create_info(create_info);

            if (create_debug_utils_messenger_ext(instance, &create_info, nullptr, &debug_messenger) != VK_SUCCESS)
                throw std::runtime_error("\nFailed to setup debug messenger.");
        }

        void Window::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info)
        {
            create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            create_info.pfnUserCallback = debug_callback;
        }

        void Window::destroy_debug_messenger(
                    VkInstance instance,
                    VkDebugUtilsMessengerEXT debug_messenger,
                    const VkAllocationCallbacks* p_allocator)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugMessengerEXT");
            if (func != nullptr)
                func (instance, debug_messenger, p_allocator);
        }

        VkResult Window::create_debug_utils_messenger_ext(
                    VkInstance instance, 
                    const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
                    const VkAllocationCallbacks* p_allocator,
                    VkDebugUtilsMessengerEXT* p_debug_messenger)
        {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr)
                return func(instance, p_create_info, p_allocator, p_debug_messenger);
            else
                return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL Window::debug_callback(
                    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                    VkDebugUtilsMessageTypeFlagsEXT message_type,
                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                    void* pUserData)
        {
            std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

            return VK_FALSE;
        }  
    };
}
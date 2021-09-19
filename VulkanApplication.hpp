#pragma once

/**
 * @author Simon Brisebois-Therrien
 * @since 2021-09-19
 * @brief
 * Template Vulcan openGL application. 
 */

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <map>
#include <optional>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

using namespace std;


struct QueueFamilyIndices
{
    uint32_t graphicsFamily;
};


class Application 
{
    public:
        Application(string window_name = "Default template Vulkan application")
        {
            this->window_name = window_name;
        }
        void run() 
        {
            initWindow();
            initVulkan();
            mainLoop();
            cleanup();
        }

    private:
        /** 
         * Members
         **/
        string window_name;
        string application_name = "Default template Vulkan application";

        GLFWwindow* window;

        const uint32_t w_width = 800;
        const uint32_t w_height = 600;

        double last_draw = 0;

        VkInstance instance;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;


        /**
         * Methods
         **/

        void initVulkan()
        {
            createInstance();
            pickPhysicalDevice();
        }

        bool isDeviceSutable(VkPhysicalDevice device) 
        {
            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;

            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                   deviceFeatures.geometryShader;
        }

        void pickPhysicalDevice()
        {

            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

            if (deviceCount == 0)
            {
                throw std::runtime_error("failed to find GPUs with Vulkan support!");
            }

            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

            for (const auto& device : devices)
            {
                if (isDeviceSutable(device))
                {
                    physicalDevice = device;
                    break;
                }
            }

            if (physicalDevice == VK_NULL_HANDLE)
                throw std::runtime_error("Failed to find a suitable GPU!");
        }

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
        {
            QueueFamilyIndices indices;

            return indices;
        }

        void createInstance()
        {
            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = this->application_name.c_str();
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "VulkanGameEngine";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            createInfo.pApplicationInfo = &appInfo;

            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;

            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            createInfo.enabledExtensionCount = glfwExtensionCount;
            createInfo.ppEnabledExtensionNames = glfwExtensions;

            createInfo.enabledLayerCount = 0;

            VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

            if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create instance.");
            }

            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> extensions(extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

            printf("\nAvailable extensions: \n");

            for (const auto& extension : extensions)
            {
                printf("\t%s\n", extension.extensionName);
            }
        }

        void mainLoop()
        {
            while (!glfwWindowShouldClose(window))
            {
                glfwPollEvents();
                delta_time();
            }
        }

        void cleanup()
        {
            vkDestroyInstance(instance, nullptr);
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        void initWindow()
        {
            glfwInit();

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            window = glfwCreateWindow(w_width, w_height, this->window_name.c_str(), nullptr, nullptr);

        }

        void delta_time()
        {
            double time = glfwGetTime();
            double delta = time - last_draw;
            last_draw = time;

            printf("\nFPS: %f", 1/delta);
        }
};
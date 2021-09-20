#include <iostream>
#include "src/core/graphics/window.hpp"

// #include "VulkanApplication.hpp"



int main(int, char**) {
    try
    {
        VulkanGameEngine::Graphics::Window window;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    

}

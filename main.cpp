#include <iostream>


#include "VulkanApplication.hpp"

class TestApplication : public Application
{
        
};

int main(int, char**) {
    TestApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

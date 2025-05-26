#include "VulkanApplication.h"
#include <iostream>

int main() {
    try {
        VulkanApplication::Config config;
        config.appName = "c06_renderloop_new";
        config.width = 1024;
        config.height = 768;
        config.enableValidation = true;

        VulkanApplication app(config);
        app.mainLoop();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

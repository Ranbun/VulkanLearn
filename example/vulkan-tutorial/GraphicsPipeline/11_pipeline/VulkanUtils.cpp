#include "VulkanUtils.h"

#include <fstream>
#include <limits>
#include <GLFW/glfw3.h>
#include "Application.h"

namespace VulkanUtils
{
    VkSurfaceFormatKHR chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
    {
        for (auto & format: formats)
        {
            if (format.format == VK_FORMAT_B8G8R8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return format;
            }
        }

        return formats.front();
    }

    VkPresentModeKHR chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& presentModes)
    {
        for (auto const& presentMode: presentModes)
        {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return presentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }


    VkExtent2D chooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        /// 交换链图像的分辨率
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            auto & app = Application::Instance();
            auto window = static_cast<GLFWwindow *>(app.RenderWindow()->getNativeWindow());

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent
            {
                .width = static_cast<uint32_t>(width),
                .height = static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    std::vector<char> readFile(const std::string &filename) {

        // use 'ate' read file form tail
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        /// get file size
        auto fileSize = static_cast<long long>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
}

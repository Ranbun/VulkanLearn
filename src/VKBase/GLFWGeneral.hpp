#ifndef GLFWGENERAL_H_
#define GLFWGENERAL_H_

#include "VKBase.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <sstream>

#include "Log.hpp"

GLFWwindow * pWindow = nullptr;
GLFWmonitor * pMonitor = nullptr;
const char * windowTitle = "VLC";  /// Vulkan learn Case

/**
 *
 * @param size 窗口大小
 * @param fullScreen 是否全屏
 * @param isResizable 是否可以resize
 * @param limitFrameRate 是否限制帧率
 * @return bool 初始化结果
 */
bool InitializeWindow(VkExtent2D size, bool fullScreen = false, bool isResizable = true, bool limitFrameRate = true)
{
    /// TODO: 初始化窗口资源 ....
    if(!glfwInit())
    {
        LOG_ERROR("GLFW init failed.");
        return false;
    }

    /// 启用GLFW的Vulkan设置,不开启多余的OpenGL设置
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, isResizable);

    pMonitor = glfwGetPrimaryMonitor();

    /// 获取显示器的显示模式
    const GLFWvidmode * pMode = glfwGetVideoMode(pMonitor);
    /// note: 全屏时的屏幕区域应当和屏幕分辨率一样, 所以需要获的当前屏幕的分辨率大小
    /// 创建窗口
    pWindow = fullScreen ? glfwCreateWindow(pMode->width, pMode->height, windowTitle, pMonitor, nullptr) :
                         glfwCreateWindow(size.width, size.height, windowTitle, nullptr, nullptr);

    if(!pWindow)
    {
        LOG_DEBUG("Initialize Create Window Failed.")
        glfwTerminate();

        return false;
    }
    return true;
}

void TerminateWindow()
{
    glfwTerminate();
}

void TitleFps()
{
    static double time_start = glfwGetTime();
    static double time_end;

    static double dt;
    static int dframe = -1;
    static std::stringstream info;
    time_end = glfwGetTime();

    dframe++;
    if((dt = time_end - time_start) >= 1)
    {
        info.precision(1);
        info << windowTitle << "   " << std::fixed << dframe / dt << " Fps";
        glfwSetWindowTitle(pWindow, info.str().c_str());
        info.str("");
        time_start = time_end;
        dframe = 0;
    }
}

void MakeWindowFullScreen()
{
    const GLFWvidmode * pMode = glfwGetVideoMode(pMonitor);
    glfwSetWindowMonitor(pWindow, pMonitor, 0, 0, pMode->width, pMode->height, pMode->refreshRate);
}

void MakeWindowWindowed(VkOffset2D position, VkExtent2D size)
{
    const GLFWvidmode * pMode = glfwGetVideoMode(pMonitor);
    glfwSetWindowMonitor(pWindow, nullptr, position.x, position.y, size.width, size.height, pMode->refreshRate);
}

#endif //! GLFWGENERAL_H_

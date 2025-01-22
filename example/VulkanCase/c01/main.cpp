#include <iostream>

#include <GLFWGeneral.hpp>

static void keyCallBack(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    LOG_INFO("Key: ", key, " ", action ? "press" : "release");

    if(key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        MakeWindowFullScreen();
        // auto pMode = glfwGetVideoMode(pMonitor);
        // glViewport(0,0,pMode->width,pMode->height);

        if(mods & GLFW_MOD_CONTROL)
        {
            MakeWindowWindowed({400,400}, {1000,800});
            // glViewport(0,0,1000,800);
        }
    }
}

int main()
{
    std::cout<<__FUNCTION__ <<std::endl;

    if(!InitializeWindow({1000,800}))
    {
        return -1;
    }

    glfwSetKeyCallback(pWindow, keyCallBack);

    while(!glfwWindowShouldClose(pWindow))
    {
        glfwPollEvents();
        TitleFps();
    }

    TerminateWindow();
    return 0;
}



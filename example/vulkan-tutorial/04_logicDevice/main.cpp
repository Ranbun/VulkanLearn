
#include <iostream>
#include "Application.h"

#include "window_pro.h"

int main(int argc, char *argv[])
{
    WP.title = "physical Device and queue families";

    Application app;
    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}

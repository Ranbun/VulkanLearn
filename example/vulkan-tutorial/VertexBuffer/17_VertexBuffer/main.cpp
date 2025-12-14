
#include <iostream>

#include "Application.h"

int main(int argc, char *argv[])
{
    try
    {
        const Application app(800,600,"VertexBuffer");
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}

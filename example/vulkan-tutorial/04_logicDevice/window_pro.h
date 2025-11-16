#ifndef WINDOW_PRO_H_
#define WINDOW_PRO_H_

#include <string>

struct WindowPro
{
    int width;
    int height;
    std::string title;
};

static WindowPro WP = {
        800,
        600,
        "",
};

#endif

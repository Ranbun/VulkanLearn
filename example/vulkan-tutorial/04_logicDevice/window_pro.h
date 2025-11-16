#ifndef B1CB949F_58D9_4CA0_A9E6_E7CBE02669EC
#define B1CB949F_58D9_4CA0_A9E6_E7CBE02669EC
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


#endif /* B1CB949F_58D9_4CA0_A9E6_E7CBE02669EC */

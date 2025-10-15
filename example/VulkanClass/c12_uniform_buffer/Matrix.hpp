#ifndef VULKANLEARN_MATRIX_H
#define VULKANLEARN_MATRIX_H

#include <cmath>

namespace VKL
{
    static constexpr float PI = 3.14159265357;

    class Matrix
    {
    public:
        float m[16];
        void makeIdentify()
        {
            for (int i= 0 ;i < 16; i++)
            {
                m[i] = 0.0f;
            }
            // for (auto i = 0; i < 4;i++)
            // {
            //     m[4 * i  + i] = 1.0;
            // }
            m[0 + 0*4] = 1.0f;
            m[1 + 1*4] = 1.0f;
            m[2 + 2*4] = 1.0f;
            m[3 + 3*4] = 1.0f;

        };

        void makeRotate(float angle, float x, float y, float z)
        {
            auto mag = std::sqrt(x * x + y*y + z*z);
            auto sinAngle = sin(angle * PI / 180.0f);
            auto cosAngle = cos(angle * PI / 180.0f);
            float xx,yy,zz,xy,yz,zx,xs,ys,zs,oneMinusCos;

            if (mag <= 0.0f) return;
            makeIdentify();

            x /= mag; y /= mag; z /= mag;
            xx = x * x; yy = y *y; zz = z * z;
            xy = x * y; yz = y * z; zx = z * x;
            xs = x * sinAngle; ys = y * sinAngle; zs = z * sinAngle;
            oneMinusCos = 1.0f - cosAngle;

            m[0 * 4 + 0] = (oneMinusCos * xx) + cosAngle;
            m[0 * 4 + 1] = (oneMinusCos * xy) - zs;
            m[0 * 4 + 2] = (oneMinusCos * zx) + ys;
            m[0 * 4 + 3] = 0.0f;

            m[1 * 4 + 0] = (oneMinusCos * xy) + zs;
            m[1 * 4 + 1] = (oneMinusCos * yy) + cosAngle;
            m[1 * 4 + 2] = (oneMinusCos * yz) - xs;
            m[1 * 4 + 3] = 0.0f;

            m[2 * 4 + 0] = (oneMinusCos * zx) - ys;
            m[2 * 4 + 1] = (oneMinusCos * yz) + xs;
            m[2 * 4 + 2] = (oneMinusCos * zz) + cosAngle;
            m[2 * 4 + 3] = 0.0f;
        }

        void makeOrtho(float l, float r, float b, float t, float zn, float zf)
        {
            makeIdentify();
            m[0 * 4 + 0] = 2.0f / (r - l);
            m[1 * 4 + 1] = 2.0f / (t  - b);
            m[2 * 4 + 2] = 1.0f / (zn - zf);
            m[3 * 4 + 0] = -(r + l) / (r - l);
            m[3 * 4 + 1] = -(b + t) / (b - t);
            m[3 * 4 + 2] = zn / (zn - zf);
        }
    };

}// namespace VKL

#endif//VULKANLEARN_MATRIX_H

#ifndef UNIFORM_BUFFER_OBJECT_H_
#define UNIFORM_BUFFER_OBJECT_H_

#include <glm/matrix.hpp>

struct UniformBufferObject
{
    // glm::vec2 foo;          ///< alignment test
    // 使用 alignas 控制对齐
    alignas(16) glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

#endif //! UNIFORM_BUFFER_OBJECT_H_

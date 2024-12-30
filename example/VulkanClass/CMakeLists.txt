# cmake_minimum_required(VERSION 3.22)
# PROJECT(VulkanClass)

FIND_PACKAGE(Vulkan)
FIND_PACKAGE(glfw3)

ADD_SUBDIRECTORY(c00_framework)
ADD_SUBDIRECTORY(c01_instance)


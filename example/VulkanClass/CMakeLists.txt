# cmake_minimum_required(VERSION 3.22)
# PROJECT(VulkanClass)

FIND_PACKAGE(Vulkan REQUIRED)
FIND_PACKAGE(glfw3 REQUIRED)

ADD_SUBDIRECTORY(c00_framework)
ADD_SUBDIRECTORY(c01_instance)
ADD_SUBDIRECTORY(c02_devices)


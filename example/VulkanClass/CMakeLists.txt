cmake_minimum_required(VERSION 3.22)
PROJECT(VulkanClass)

FIND_PACKAGE(Vulkan REQUIRED)

ADD_SUBDIRECTORY(c00_framework)
ADD_SUBDIRECTORY(c01_instance)
ADD_SUBDIRECTORY(c02_devices)
ADD_SUBDIRECTORY(c03_surface)
ADD_SUBDIRECTORY(c04_swapchain)


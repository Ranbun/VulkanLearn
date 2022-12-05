#include "VkCallback.h"

#include <iostream>

/**
 * @brief ���յ�����Ϣ�Ļص����� - ��VulKan����
 * @param messageServerity ��Ϣ�ļ���
 * @param messageType ��Ϣ������
 * @param pCallbackData һ��VkDebugUtilsMessengerCallbackDataEXT��ָ��(����������Ϣ......)
 * @param pUserData ָ�����ûص�����ʱ�����õ����ݵ�ָ��
 * @return ��ʾ��������㴦���VK_API�Ƿ��ж� ͨ��ֻ���ڲ��Լ���㱾���ʱ��Ż᷵��true
 */

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageServerity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

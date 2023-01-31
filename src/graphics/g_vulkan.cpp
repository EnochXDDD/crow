#include "g_graphics.hpp"

namespace crow::graphics
{

struct Environment::impl
{
    std::shared_ptr<VkInstance> pInstance = nullptr;
    std::shared_ptr<VkDebugUtilsMessengerEXT> pDebugMessenger = nullptr;
    std::shared_ptr<VkPhysicalDevice> pPhysicalDevice = nullptr;
};

struct Surface::impl
{};

static VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugMessengerCallback
(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
)
{
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            DLOG(INFO) << pCallbackData->pMessage << std::endl;
            break;
        
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            DLOG(WARNING) << pCallbackData->pMessage << std::endl;
            break;
        
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            DLOG(ERROR) << pCallbackData->pMessage << std::endl;
            break;

        default:
            break;
    }

    return VK_FALSE;
}

void checkSupportedInstanceExtension(std::vector<const char*> vpchReqiredExtension)
{
    uint32_t nInstanceExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &nInstanceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> vInstanceExtension(nInstanceExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &nInstanceExtensionCount, vInstanceExtension.data());
    
    DLOG(INFO) << "available extensions:" << std::endl;
    for (const auto& extension: vInstanceExtension) {
        DLOG(INFO) << '\t' << extension.extensionName << std::endl;
    }

    if (
        std::count_if(
            std::execution::par_unseq,
            vpchReqiredExtension.begin(),
            vpchReqiredExtension.end(),
            [&](auto required){
                return std::find_if(
                    std::execution::par_unseq,
                    vInstanceExtension.begin(),
                    vInstanceExtension.end(),
                    [&](auto extension){ return std::string(extension.extensionName) == std::string(required); }
                ) != vInstanceExtension.end();
            }
        ) != static_cast<int>(vpchReqiredExtension.size())
    ) {
        DLOG(FATAL) << "instance extension not found" << std::endl;
        exit(1);
    }
}

void checkSupportedInstanceLayer(std::vector<const char*> vpchReqiredLayer)
{
    uint32_t nInstanceLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&nInstanceLayerCount, nullptr);
    std::vector<VkLayerProperties> vInstanceLayer(nInstanceLayerCount);
    vkEnumerateInstanceLayerProperties(&nInstanceLayerCount, vInstanceLayer.data());

    DLOG(INFO) << "available layers:" << std::endl;
    for (const auto& layer: vInstanceLayer) {
        DLOG(INFO) << '\t' << layer.layerName << std::endl;
    }

    if (
        std::count_if(
            std::execution::par_unseq,
            vpchReqiredLayer.begin(),
            vpchReqiredLayer.end(),
            [&](auto required){
                return std::find_if(
                    std::execution::par_unseq,
                    vInstanceLayer.begin(),
                    vInstanceLayer.end(),
                    [&](auto layer){
                        return std::string(layer.layerName) == std::string(required);
                    }) != vInstanceLayer.end();
            }
        ) != static_cast<int>(vpchReqiredLayer.size())
    ) {
        DLOG(FATAL) << "instance layer not found" << std::endl;
        exit(1);
    }
}

std::shared_ptr<VkInstance> createInstance(std::vector<const char*> vpchReqiredExtension, std::vector<const char*> vpchReqiredLayer)
{
    std::unique_ptr<VkApplicationInfo> pAppInfo = nullptr;
    pAppInfo = std::make_unique<VkApplicationInfo>();
    pAppInfo->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    pAppInfo->pApplicationName = "crow engine";
    pAppInfo->applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    pAppInfo->pEngineName = "crow";
    pAppInfo->engineVersion = VK_MAKE_VERSION(0, 0, 1);
    pAppInfo->apiVersion = VK_API_VERSION_1_3;

    auto pVkInstanceCreateInfo = std::make_unique<VkInstanceCreateInfo>();
    pVkInstanceCreateInfo->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    pVkInstanceCreateInfo->pApplicationInfo = pAppInfo.get();
    pVkInstanceCreateInfo->enabledExtensionCount = static_cast<uint32_t>(vpchReqiredExtension.size());
    pVkInstanceCreateInfo->ppEnabledExtensionNames = vpchReqiredExtension.data();
    pVkInstanceCreateInfo->enabledLayerCount = static_cast<uint32_t>(vpchReqiredLayer.size());
    pVkInstanceCreateInfo->ppEnabledLayerNames = vpchReqiredLayer.data();

    auto pInstance = std::shared_ptr<VkInstance>(new VkInstance, [](VkInstance* pInstance){ vkDestroyInstance(*pInstance, nullptr); });
    if (vkCreateInstance(pVkInstanceCreateInfo.get(), nullptr, pInstance.get()) != VK_SUCCESS ) {
        DLOG(FATAL) << "Create vulkan instance failed" << std::endl;
        exit(1);
    }

    checkSupportedInstanceExtension(vpchReqiredExtension);
    checkSupportedInstanceLayer(vpchReqiredLayer);

    return pInstance;
}

std::shared_ptr<VkDebugUtilsMessengerEXT> createDebugMessenger(VkInstance instance)
{
    auto pVkDebugMessengerCreateInfo = std::make_unique<VkDebugUtilsMessengerCreateInfoEXT>();
    pVkDebugMessengerCreateInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    pVkDebugMessengerCreateInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    pVkDebugMessengerCreateInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    pVkDebugMessengerCreateInfo->pfnUserCallback = vkDebugMessengerCallback;
    pVkDebugMessengerCreateInfo->pUserData = nullptr;

    auto pDebugMessenger = std::shared_ptr<VkDebugUtilsMessengerEXT>(
        new VkDebugUtilsMessengerEXT,
        [=](VkDebugUtilsMessengerEXT* pDebugMessenger){
            reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"))(
                instance,
                *pDebugMessenger,
                nullptr
            );
        }
    );
    
    if (
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"))(
            instance,
            pVkDebugMessengerCreateInfo.get(),
            nullptr,
            pDebugMessenger.get()
        ) != VK_SUCCESS
    ) {
        DLOG(FATAL) << "Create vulkan debug messenger failed" << std::endl;
    }

    return pDebugMessenger;
}

std::shared_ptr<VkPhysicalDevice> selectPhysicalDevice(VkInstance instance)
{
    uint32_t nPhysicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &nPhysicalDeviceCount, nullptr);
    
    if (nPhysicalDeviceCount == 0) {
        DLOG(FATAL) << "Find GPUs with vulkan support failed" << std::endl;
        exit(1);
    }

    std::vector<VkPhysicalDevice> vPhysicalDevice(nPhysicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &nPhysicalDeviceCount, vPhysicalDevice.data());
    
    DLOG(INFO) << "physical devices:" << std::endl;
    std::multimap<int, VkPhysicalDevice, std::greater<int>> mapPhysicalDevice;
    std::for_each(
        std::execution::par_unseq,
        vPhysicalDevice.begin(),
        vPhysicalDevice.end(),
        [&](VkPhysicalDevice device){
            int score = 0;
            
            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            switch (deviceProperties.deviceType) {
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    score += 3;
                    break;
                
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    score += 2;
                
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    score += 1;
                
                default:
                    break;
            }

            if (deviceFeatures.geometryShader == VK_TRUE) {
                score += 10;
            }

            if (deviceFeatures.tessellationShader == VK_TRUE) {
                score += 10;
            }

            DLOG(INFO) << '\t' << deviceProperties.deviceName << ": " << score << std::endl;
            mapPhysicalDevice.insert(std::make_pair(score, device));
        }
    );

    std::shared_ptr<VkPhysicalDevice> pDevice = nullptr;
    
    // if (mapPhysicalDevice.begin()->first > 0) {
    //     pDevice.reset(&mapPhysicalDevice.rbegin()->second);
    // } else {
    //     DLOG(FATAL) << "Find a suitable GPU failed" << std::endl;
    //     exit(1);
    // }

    return pDevice;
}

Environment::Environment(): pImpl(new impl)
{
    std::vector<const char*> vpchVkInstanceExtensionNames = {
        "VK_KHR_portability_enumeration",
        "VK_EXT_debug_utils",
        "VK_KHR_surface",
        "VK_KHR_xcb_surface",
        "VK_KHR_get_surface_capabilities2",
        "VK_KHR_get_physical_device_properties2",
        "VK_KHR_external_semaphore_capabilities",
        "VK_KHR_external_memory_capabilities",
        "VK_KHR_external_fence_capabilities",
        "VK_KHR_device_group_creation"
    };

    std::vector<const char*> vpchVkInstanceLayerNames = {
        "VK_LAYER_KHRONOS_validation"
    };

    pImpl->pInstance = createInstance(vpchVkInstanceExtensionNames, vpchVkInstanceLayerNames);
    pImpl->pDebugMessenger = createDebugMessenger(*pImpl->pInstance);
    pImpl->pPhysicalDevice = selectPhysicalDevice(*pImpl->pInstance);
}

Environment::~Environment()
{
    pImpl->pPhysicalDevice.reset();
    pImpl->pDebugMessenger.reset();
    pImpl->pInstance.reset();
}

Surface::Surface(): pImpl(new impl)
{}

Surface::~Surface()
{}

}

#include <GraphicsEngine/Devices/surface.h>

#include <GraphicsEngine/Devices/instance.h>
#include <GraphicsEngine/Devices/physical_device.h>
#include <GraphicsEngine/Devices/window.h>

namespace poke {
namespace graphics {
Surface::Surface(
    const Instance& instance,
    const PhysicalDevice& physicalDevice,
    Window& window)
    : kInstance_(instance)
    , kPhysicalDevice_(physicalDevice)
    , surface_(VK_NULL_HANDLE)
    , capabilities_({})
    , format_({}) {
    window.CreateSurface(kInstance_, nullptr, &surface_);

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		kPhysicalDevice_,
        surface_,
        &capabilities_);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(kPhysicalDevice_,
                                         surface_,
                                         &formatCount,
                                         nullptr);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(kPhysicalDevice_,
                                         surface_,
                                         &formatCount,
                                         surfaceFormats.data());

    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
        format_.format = VK_FORMAT_B8G8R8A8_UNORM;
        format_.colorSpace = surfaceFormats[0].colorSpace;
    } else {
        auto foundB8G8R8AUnorm = false;

        for (auto&& surfaceFormat : surfaceFormats) {
            if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM) {
                format_.format = surfaceFormat.format;
                format_.colorSpace = surfaceFormat.colorSpace;
                foundB8G8R8AUnorm = true;
                break;
            }
        }

        if (!foundB8G8R8AUnorm) {
            format_.format = surfaceFormats[0].format;
            format_.colorSpace = surfaceFormats[0].colorSpace;
        }
    }
}

Surface::~Surface() { vkDestroySurfaceKHR(kInstance_, surface_, nullptr); }

const VkSurfaceCapabilitiesKHR& Surface::GetCapabilities() const {
    return capabilities_;
}
} //namespace graphics
} //namespace poke

#include <vulkan/vulkan.h>
#include <vector>

namespace moe {
class MoePhysicalDevice {
public:
    MoePhysicalDevice(const VkInstance& instance);
    ~MoePhysicalDevice();

private:
    void fetchAll       ();
    void fetchBest      (const std::vector<VkPhysicalDevice>& devices, std::vector<const char *> extensions);
    int  score          (const VkPhysicalDevice& device, std::vector<const char*> extensions);

    void printPhysicalDeviceStats(VkPhysicalDevice device);

    const VkInstance&   _instance;
    VkPhysicalDevice    _device;
};
}


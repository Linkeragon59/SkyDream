///----- Vulkan Allocator ----- ///

#if WINDOWS_BUILD
#pragma warning(push)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#pragma warning(disable:4189)
#pragma warning(disable:4324)
#elif LINUX_BUILD
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#if WINDOWS_BUILD
#pragma warning(pop)
#elif LINUX_BUILD
#pragma GCC diagnostic pop
#endif

///----- Tiny gltf ----- ///

#if WINDOWS_BUILD
#pragma warning(push)
#pragma warning(disable:5311)
#elif LINUX_BUILD
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define STB_IMAGE_IMPLEMENTATION
#include "tiny_gltf.h"

#if WINDOWS_BUILD
#pragma warning(pop)
#elif LINUX_BUILD
#pragma GCC diagnostic pop
#endif

#include <windows.h>

#include <algorithm>
#include <atomic>
#include <charconv>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <system_error>
#include <string>

#include <reshade.hpp>

namespace
{
HMODULE g_module = nullptr;
std::atomic_uint64_t g_present_count = 0;

struct BridgeConfig
{
    bool enabled = true;
    float scale = 1.5f;
    uint32_t target_width = 0;
    uint32_t target_height = 0;
};

void log_info(const char *message)
{
    reshade::log::message(reshade::log::level::info, message);
}

void logf(const char *format, uint32_t a, uint32_t b, uint32_t c = 0, uint32_t d = 0)
{
    char message[512] = {};
    std::snprintf(message, sizeof(message), format, a, b, c, d);
    log_info(message);
}

bool parse_bool(const char *value, bool fallback)
{
    if (value == nullptr || value[0] == '\0')
        return fallback;

    return value[0] == '1' || value[0] == 't' || value[0] == 'T' ||
           value[0] == 'y' || value[0] == 'Y';
}

float parse_float(const char *value, float fallback)
{
    if (value == nullptr || value[0] == '\0')
        return fallback;

    char *end = nullptr;
    const float parsed = std::strtof(value, &end);
    return end != value ? parsed : fallback;
}

uint32_t parse_u32(const char *value, uint32_t fallback)
{
    if (value == nullptr || value[0] == '\0')
        return fallback;

    uint32_t parsed = 0;
    const auto *begin = value;
    const auto *end = value + std::strlen(value);
    const auto result = std::from_chars(begin, end, parsed);
    return result.ec == std::errc() ? parsed : fallback;
}

bool get_config_string(reshade::api::effect_runtime *runtime, const char *key, char *value, size_t value_size)
{
    size_t size = value_size;
    return reshade::get_config_value(runtime, "FSR2Injector", key, value, &size);
}

BridgeConfig load_config(reshade::api::effect_runtime *runtime)
{
    BridgeConfig config;
    char value[64] = {};

    if (get_config_string(runtime, "Enabled", value, sizeof(value)))
        config.enabled = parse_bool(value, config.enabled);

    value[0] = '\0';
    if (get_config_string(runtime, "Scale", value, sizeof(value)))
        config.scale = std::clamp(parse_float(value, config.scale), 1.0f, 4.0f);

    value[0] = '\0';
    if (get_config_string(runtime, "TargetWidth", value, sizeof(value)))
        config.target_width = parse_u32(value, config.target_width);

    value[0] = '\0';
    if (get_config_string(runtime, "TargetHeight", value, sizeof(value)))
        config.target_height = parse_u32(value, config.target_height);

    return config;
}

BridgeConfig load_global_config()
{
    BridgeConfig config;
    char value[64] = {};
    size_t value_size = sizeof(value);

    if (reshade::get_config_value(nullptr, "FSR2Injector", "Enabled", value, &value_size))
        config.enabled = parse_bool(value, config.enabled);

    value[0] = '\0';
    value_size = sizeof(value);
    if (reshade::get_config_value(nullptr, "FSR2Injector", "Scale", value, &value_size))
        config.scale = std::clamp(parse_float(value, config.scale), 1.0f, 4.0f);

    value[0] = '\0';
    value_size = sizeof(value);
    if (reshade::get_config_value(nullptr, "FSR2Injector", "TargetWidth", value, &value_size))
        config.target_width = parse_u32(value, config.target_width);

    value[0] = '\0';
    value_size = sizeof(value);
    if (reshade::get_config_value(nullptr, "FSR2Injector", "TargetHeight", value, &value_size))
        config.target_height = parse_u32(value, config.target_height);

    return config;
}

bool on_create_swapchain(reshade::api::device_api api, reshade::api::swapchain_desc &desc, void *)
{
    BridgeConfig config = load_global_config();
    if (!config.enabled)
        return false;

    uint32_t width = desc.back_buffer.texture.width;
    uint32_t height = desc.back_buffer.texture.height;
    if (width == 0 || height == 0)
        return false;

    uint32_t target_width = config.target_width;
    uint32_t target_height = config.target_height;
    if (target_width == 0 || target_height == 0)
    {
        target_width = static_cast<uint32_t>(width * config.scale + 0.5f);
        target_height = static_cast<uint32_t>(height * config.scale + 0.5f);
    }

    if (target_width <= width || target_height <= height)
        return false;

    desc.back_buffer.texture.width = target_width;
    desc.back_buffer.texture.height = target_height;

    logf("FSR2 injector bridge: resizing swapchain from %ux%u to %ux%u", width, height, target_width, target_height);
    return true;
}

void on_reshade_present(reshade::api::effect_runtime *runtime)
{
    const uint64_t frame = ++g_present_count;
    if (frame != 1 && frame % 300 != 0)
        return;

    const BridgeConfig config = load_config(runtime);
    if (!config.enabled)
        return;

    char message[256] = {};
    std::snprintf(message, sizeof(message), "FSR2 injector bridge: ReShade frame %llu, spatial scale %.2f", static_cast<unsigned long long>(frame), config.scale);
    log_info(message);
}

void on_present(reshade::api::command_queue *, reshade::api::swapchain *, const reshade::api::rect *, const reshade::api::rect *, uint32_t, const reshade::api::rect *)
{
    const uint64_t frame = g_present_count.load();
    if (frame == 0)
        log_info("FSR2 injector bridge: first swapchain present observed");
}
}

extern "C" __declspec(dllexport) bool AddonInit(HMODULE module, HMODULE reshade_module)
{
    g_module = module;
    if (!reshade::register_addon(module, reshade_module))
        return false;

    reshade::register_event<reshade::addon_event::create_swapchain>(&on_create_swapchain);
    reshade::register_event<reshade::addon_event::present>(&on_present);
    reshade::register_event<reshade::addon_event::reshade_present>(&on_reshade_present);

    log_info("FSR2 injector bridge: add-on loaded");
    return true;
}

extern "C" __declspec(dllexport) void AddonUninit(HMODULE module, HMODULE reshade_module)
{
    reshade::unregister_event<reshade::addon_event::reshade_present>(&on_reshade_present);
    reshade::unregister_event<reshade::addon_event::present>(&on_present);
    reshade::unregister_event<reshade::addon_event::create_swapchain>(&on_create_swapchain);
    reshade::unregister_addon(module, reshade_module);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        g_module = module;
        DisableThreadLibraryCalls(module);
    }
    return TRUE;
}

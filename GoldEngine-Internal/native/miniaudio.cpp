#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <functional>
#include "miniaudio_hook.h"

#define SAMPLE_RATE 44100

void initialize_buffer_store(buffer_store_t* buffer_store)
{
    buffer_store = new buffer_store_t();
    buffer_store->buffer = std::vector<int16_t>();
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    buffer_store_t* buffer_store = (buffer_store_t*)pDevice->pUserData;

    const int16_t* input = (const int16_t*)pInput;
    std::copy(input, input + frameCount, buffer_store->buffer.begin());

    long sum = 0;
    for (ma_uint32 i = 0; i < frameCount; i++)
    {
        sum += std::abs(input[i]);
    }

    float volume = static_cast<float>(sum) / static_cast<float>(frameCount);

    if (buffer_store->on_volume_change)
        buffer_store->on_volume_change(volume);

    if (volume > noise_threshold)
    {
        if (buffer_store->on_noise_detected)
        {
            buffer_store->on_noise_detected();
        }
    }
    else
    {
        if (buffer_store->on_silent)
            buffer_store->on_silent();
    }

    bool peak_detected = false;
    for (ma_uint32 i = 0; i < frameCount; ++i) {
        if (std::abs(input[i]) > peak_threshold) {
            peak_detected = true;
            break;
        }
    }
    if (peak_detected && buffer_store->on_peak_detected) {
        buffer_store->on_peak_detected();
    }
}

ma_device_info* pPlaybackInfos;
ma_device_info* pCaptureInfos;
ma_uint32 playbackCount;
ma_uint32 captureCount;

ma_context* context = nullptr;
buffer_store_t* buffer = nullptr;
ma_result* result = nullptr;
ma_device_config* deviceConfig = nullptr;
ma_device* device = nullptr;

buffer_store_t* get_buffer()
{
    return buffer;
}

bool initialize(int microphone)
{
    if (context != nullptr)
        return device;

    if (ma_context_init(NULL, 0, NULL, context) != MA_SUCCESS)
    {
        return nullptr;
    }

    if (ma_context_get_devices(context, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount) != MA_SUCCESS)
    {
        return nullptr;
    }

    initialize_buffer_store(buffer);

    deviceConfig = &ma_device_config_init(ma_device_type_capture);
    deviceConfig->capture.format = ma_format_s16; 
    deviceConfig->capture.channels = 1;
    deviceConfig->capture.pDeviceID = &pCaptureInfos[microphone].id;
    deviceConfig->sampleRate = SAMPLE_RATE;
    deviceConfig->dataCallback = data_callback;
    deviceConfig->pUserData = &buffer;

    if (ma_device_init(NULL, deviceConfig, device) != MA_SUCCESS)
    {
        return nullptr;
    }
    ma_device_start(device);


    return device;
}

void uninitialize()
{
    if (device == nullptr) return;

    ma_device_uninit(device);
    ma_context_uninit(context);
}
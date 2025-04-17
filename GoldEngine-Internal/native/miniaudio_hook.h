#pragma once

#define BUFFER_SIZE 1024

float noise_threshold = 1000.0f;
int peak_threshold = 3000;

typedef struct
{
	std::function<void()> on_noise_detected;
	std::function<void()> on_silent;
	std::function<void(float volume)> on_volume_change;
	std::function<void()> on_peak_detected;
	std::vector<int16_t> buffer;
} buffer_store_t;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
bool initialize(int microphone = 0);
void uninitialize();

buffer_store_t* get_buffer();
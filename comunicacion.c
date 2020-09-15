

#include "raylib.h"

#include<windows.h>
#include<stdio.h>

#include <stdlib.h>
#include <math.h>
#include <string.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


typedef struct {
	float d;
	float x;
	float y;
	float z;
} Theremin_Data;
		

int get_first_int(char* start, char* end) {
	char* current = start;
	char* next = current+1;
	while (*current >= '0' && *current <= '9') {
		current = next;
		next = current + 1;
		if (current == end) break;
	}
	char buf[10] = {0};
	memcpy(buf, start, current - start);
	return atoi(buf);
}

float unlerp(float value, float min, float max) {
	return (value-min)/(max-min);
}
float lerp(float value, float min, float max) {
	return min + value * (max-min);
}

Theremin_Data parse_data(char* buffer, int buf_size) { 
	Theremin_Data data;
	for (int idx = 0; idx < buf_size; ++idx) {
		char* start = buffer + idx + 1;
		char* end = buffer + buf_size;			
		if (buffer[idx] == 'D') data.d = get_first_int(start, end);
		if (buffer[idx] == 'X') data.x = unlerp(get_first_int(start, end), 268, 406);
		if (buffer[idx] == 'Y') data.y = unlerp(get_first_int(start, end), 260, 406);
		if (buffer[idx] == 'Z') data.z = unlerp(get_first_int(start, end), 272, 408);
	}
	
	
	return data;
}

int main() {
	printf("starting");
    // init raylib
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "THEREMIN");
    SetTargetFPS(60);
    
    
    HANDLE hComm;

    hComm = CreateFile("\\\\.\\COM6",                //port name
                      GENERIC_READ | GENERIC_WRITE, //Read/Write
                      0,                            // No Sharing
                      NULL,                         // No Security
                      OPEN_EXISTING,// Open existing port only
                      0,            // Non Overlapped I/O
                      NULL);        // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE) {
        printf("Error in opening serial port\n");
    } else {
        printf("opening serial port successful\n");
    }
    
    COMMTIMEOUTS timeouts = {
        MAXDWORD,
        0,
        0,
        0,
        0
    };
    //GetCommTimeouts(hComm, &timeouts);
	printf("ReadIntervalTimeout: %d\n", timeouts.ReadIntervalTimeout);
	printf("ReadTotalTimeoutMultiplier: %d\n", timeouts.ReadTotalTimeoutMultiplier);
	printf("ReadTotalTimeoutConstant: %d\n", timeouts.ReadTotalTimeoutConstant);
	printf("WriteTotalTimeoutMultiplier: %d\n", timeouts.WriteTotalTimeoutMultiplier);
	printf("WriteTotalTimeoutConstant: %d\n", timeouts.WriteTotalTimeoutConstant);
    SetCommTimeouts(hComm, &timeouts);
    
    
    char last_complete_message[1024] = {};
    char last_partial_message[1024];
    int partial_message_length;
	#define BUFFER_SIZE 1024
    char buffer[BUFFER_SIZE];
	int buffer_cursor = 0;
    int lines = 0;


	InitAudioDevice();
	#define SAMPLE_RATE 48000
	AudioStream stream = InitAudioStream(SAMPLE_RATE, 16, 1);
	PlayAudioStream(stream);
	float frequency = 440;
	float intensity = 1.0f;
	float old_intensity = 0;
	float old_frequency = 0;
	#define MAX_SAMPLES 1024
	#define MAX_SAMPLES_PER_UPDATE 4096
	short* wave = malloc(sizeof(short) * MAX_SAMPLES);
	short* sound_buffer = malloc(sizeof(short) * MAX_SAMPLES_PER_UPDATE);
	int wave_length = 0;
	int old_wave_length = 0;
	int read_cursor = 0;

	Theremin_Data data;
    float current_phase = 0;
    while (!WindowShouldClose()) {

		if (IsAudioStreamProcessed(stream)) {

			float TAU = 2*PI;
			float inv_k = 1.0f / MAX_SAMPLES_PER_UPDATE;
			float delta_freq = frequency - old_frequency;
			float delta_intensity = intensity - old_intensity;
			for (int idx= 0; idx < MAX_SAMPLES_PER_UPDATE; ++idx) {
				float s = inv_k * idx;
				float i = old_intensity + delta_intensity * s;
				sound_buffer[idx] = 4000 * intensity * sinf(current_phase);//idx * phase_scale);

				float f = old_frequency + s * delta_freq;
				wave_length = (int) (SAMPLE_RATE / f);
				float phase_scale = 2*PI / wave_length;
				current_phase += phase_scale;
				while (current_phase > TAU) current_phase -= TAU;
			}
			old_frequency = frequency;
			old_intensity = intensity;

			UpdateAudioStream(stream, sound_buffer, MAX_SAMPLES_PER_UPDATE);
		}

		bool changed = false;
        while (true) {
            int bytes_read = 0;
			int bytes_to_read = BUFFER_SIZE - buffer_cursor;
				
            ReadFile(hComm, buffer + buffer_cursor, bytes_to_read, &bytes_read, NULL);
			if (bytes_read == 0) {
				break;
			}

			int new_end = buffer_cursor + bytes_read;
			for (int idx = buffer_cursor; idx < new_end; ++idx) {
				if (buffer[idx] == '\n') {

					buffer[idx] = 0;
					data = parse_data(buffer, new_end);
					new_end = 0;
					changed = true;
					break;
				}
			}
			buffer_cursor = new_end;
        }
		
        if (changed) {
			intensity = data.x;

			
			float d = data.d;
			if (d > 2500) intensity = 0.01;
			
			float f = d / 2500 * 40;
			float freq = powf(1.05946f, f);
			frequency = 220 * freq;
		}
        char* text = TextFormat("values: %f %f %f %f", data.d, data.x, data.y, data.z);
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText(text, 190, 200, 20, DARKGRAY);
            GuiSliderBar((Rectangle){ 190, 220, 120, 20 }, "D", NULL, data.d, 0, 2500);
            GuiSliderBar((Rectangle){ 190, 250, 120, 20 }, "X", NULL, data.x, 0, 1);
            GuiSliderBar((Rectangle){ 190, 280, 120, 20 }, "Y", NULL, data.y, 0, 1);
            GuiSliderBar((Rectangle){ 190, 310, 120, 20 }, "Z", NULL, data.z, 0, 1);
        EndDrawing();
        
    }
    
    CloseWindow();
    CloseHandle(hComm);//Closing the Serial Port

    return 0;
}

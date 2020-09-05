

#include "raylib.h"

#include<windows.h>
#include<stdio.h>


#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main() {
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
        printf("Error in opening serial port");
    } else {
        printf("opening serial port successful");
    }
    
    COMMTIMEOUTS timeouts = {
        1,
        1,
        1,
        1,
        1
    };
    
    //SetCommTimeouts(hComm, &timeouts);
    
    
    char last_complete_message[1024] = {};
    char last_partial_message[1024];
    int partial_message_length;
         
    char buffer[1024];
    int lines = 0;
    
    while (!WindowShouldClose()) {
        
        int ended = 0;
        int buf_size = 0;
        while (!ended) {
            int bytes_read = 0;   
            ReadFile(hComm, &buffer[buf_size], 1, &bytes_read, NULL);
            if (buffer[buf_size] == '\n') {
                ended = true;
            } else {
                buf_size += 1;
            }
        }
        buffer[buf_size] = 0;
        
        int d, x, y, z;
        
        for (int idx = 0; idx < buf_size; ++idx) {
            if (buffer[idx] == 'D') {
                int start = idx + 1;
                int end = buf_size;
                for (int jdx = start; jdx < buf_size; jdx++) {
                    if (buffer[jdx] < '0' || buffer[jdx] > '9') {
                        end = jdx;
                        break;
                    }
                }
                char buf[10] = {0};
                memcpy(buf, &buffer[start], end - start);
                d = atoi(buf);
            }
            if (buffer[idx] == 'X') {
                int start = idx + 1;
                int end = buf_size;
                for (int jdx = start; jdx < buf_size; jdx++) {
                    if (buffer[jdx] < '0' || buffer[jdx] > '9') {
                        end = jdx;
                        break;
                    }
                }
                char buf[10] = {0};
                memcpy(buf, &buffer[start], end - start);
                x = atoi(buf);
            }
            if (buffer[idx] == 'Y') {
                int start = idx + 1;
                int end = buf_size;
                for (int jdx = start; jdx < buf_size; jdx++) {
                    if (buffer[jdx] < '0' || buffer[jdx] > '9') {
                        end = jdx;
                        break;
                    }
                }
                char buf[10] = {0};
                memcpy(buf, &buffer[start], end - start);
                y = atoi(buf);
            }
            if (buffer[idx] == 'Z') {
                int start = idx + 1;
                int end = buf_size;
                for (int jdx = start; jdx < buf_size; jdx++) {
                    if (buffer[jdx] < '0' || buffer[jdx] > '9') {
                        end = jdx;
                        break;
                    }
                }
                char buf[10] = {0};
                memcpy(buf, &buffer[start], end - start);
                z = atoi(buf);
            }
        }

        char* text = TextFormat("values: %d %d %d %d", d, x, y, z);
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText(text, 190, 200, 20, DARKGRAY);
            GuiSliderBar((Rectangle){ 190, 220, 120, 20 }, "D", NULL, d, 0, 2500);
            GuiSliderBar((Rectangle){ 190, 250, 120, 20 }, "X", NULL, x, 200, 460);
            GuiSliderBar((Rectangle){ 190, 280, 120, 20 }, "Y", NULL, y, 200, 460);
            GuiSliderBar((Rectangle){ 190, 310, 120, 20 }, "Z", NULL, z, 200, 460);
        EndDrawing();
        
    }
    
    CloseWindow();
    CloseHandle(hComm);//Closing the Serial Port

    return 0;
}
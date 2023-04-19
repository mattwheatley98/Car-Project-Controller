#include <Arduino.h>
#include "../lib/src/transmitTask.h"
#include "../lib/src/toggleTask.h"

SemaphoreHandle_t toggleSemaphore;

void setup() {
    Serial.begin(9600);
    toggleSemaphore = xSemaphoreCreateBinary();

    //Task for transmitting data to the receiver with ESP-NOW
    xTaskCreate(
            transmitTask,
            "Transmit Task",
            2048,
            nullptr,
            1,
            nullptr
    );
    //Task for handling switch toggles
    xTaskCreate(
            toggleTask,
            "Toggle Task",
            1024,
            nullptr,
            1,
            nullptr
    );
}

void loop() {
    //Do not use
}
//
// Created by matt on 4/18/23.
//

#include <Arduino.h>
#include "toggleTask.h"
#define SWITCH_PIN 18
#define LED_PIN 19

void toggleTask(void *parameter) {
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    //Gives a semaphore that is received by the transmit task when the switch is toggled
    //Half a second delay between the giving of semaphores
    while (1) {
        int buttonState = digitalRead(SWITCH_PIN);
        if (buttonState == LOW) {
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
            xSemaphoreGive(toggleSemaphore);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}

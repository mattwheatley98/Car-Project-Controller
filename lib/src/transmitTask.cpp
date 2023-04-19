//
// Created by matt on 4/18/23.
//

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "transmitTask.h"
#define X_PIN 35
#define Y_PIN 34

//Callback function signature
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

//Boolean for the toggle that gets converted to a char for smoother ESP-NOW transmission
boolean toggleStatus = false;

void transmitTask(void *parameter) {
    //Receiving ESP32's MAC address
    uint8_t broadcastAddress[] = {0x0C, 0xB8, 0x15, 0x76, 0x13, 0x98};

    //Struct to format transmitted controller data
    typedef struct structMessage {
        char x[16];
        char y[16];
        char toggle[8];
    } structMessage;

    //Initialize an instance of structMessage as readData
    structMessage readData;

    //Initialize readData.toggle with "false"
    strcpy(readData.toggle, "0");

    //Stores the peer information of the receiving ESP32
    esp_now_peer_info_t peerInfo;

    WiFi.mode(WIFI_STA);

    //Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
    }

    //Register the transmitting device's callback function (this is used for debugging)
    esp_now_register_send_cb(onDataSent);

    //Register peer ESP32
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    //Add peer ESP32
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
    }
    while (1) {
        int xVal = analogRead(X_PIN);
        int yVal = analogRead(Y_PIN);
        char convertedX[16];
        char convertedY[16];
        //Convert read analog values from joystick to char values
        itoa(xVal, convertedX, 10);
        itoa(yVal, convertedY, 10);

        //Set values in the struct to transmit
        strcpy(readData.x, convertedX);
        strcpy(readData.y, convertedY);

        //When the semaphore is received, toggleStatus' boolean is flipped and transmitted as a char
        if (xSemaphoreTake(toggleSemaphore, 0) == pdTRUE) {
            toggleStatus = !toggleStatus;
            if (!toggleStatus) {
                strcpy(readData.toggle, "0");
            } else if (toggleStatus) {
                strcpy(readData.toggle, "1");
            }
        }

        //For debugging the joystick
        //Serial.print("Read X: ");
        //Serial.print(readData.x);
        //Serial.print(" Read Y: ");
        //Serial.print(readData.y);
        //Serial.print(" Read Toggle: ");
        //Serial.println(readData.toggle);

        //Transmit "readData" to the receiving device via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &readData, sizeof(readData));

        //For debugging the transmission of data
        if (result == ESP_OK) {
            //Serial.println("Sent with success");
        } else {
            //Serial.println("Error sending the data");
        }
        //Delay to smoothen out data transmission (things break if under 20 in my experience)
        delay(20);
    }
}

//Callback function for when data is transmitted TO THE RECEIVER successfully or not
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    //Serial.print("\r\nLast Packet Send Status:\t");
    //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
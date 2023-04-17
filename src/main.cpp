#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#define X_PIN 35
#define Y_PIN 34

//Receiving ESP32's MAC address
uint8_t broadcastAddress[] = {0x0C, 0xB8, 0x15, 0x76, 0x13, 0x98};

//Struct to format transmitted controller data
typedef struct structMessage {
    char x[32];
    char y[32];
} structMessage;

//Initialize an instance of structMessage as readData
structMessage readData;

//Stores the peer information of the receiving ESP32
esp_now_peer_info_t peerInfo;

//Callback function for when data is transmitted TO THE RECEIVER successfully or not
//This is the GOOD one
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
    Serial.begin(9600);

    WiFi.mode(WIFI_STA);

    //Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
    }

    // read the state of the switch
    delay(300);

    // Register the transmitting device's callback function (this is used for debugging)
    esp_now_register_send_cb(OnDataSent);

    // Register peer ESP32
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    //Add peer ESP32
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
    }
}

void loop() {
    int xVal = analogRead(X_PIN);
    int yVal = analogRead(Y_PIN);
    char convertedX[32];
    char convertedY[32];
    itoa(xVal, convertedX, 10);
    itoa(yVal, convertedY, 10);


    //Set values in the struct to transmit
    strcpy(readData.x, convertedX);
    strcpy(readData.y, convertedY);

    //For debugging the joystick
    /*
    Serial.print("Read X: ");
    Serial.print(readData.x);
    Serial.print(" Read Y: ");
    Serial.println(readData.y);
     */

    //Transmit "readData" to the receiving device via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &readData, sizeof(readData));

    //For debugging the transmission of data
    //This is the weird one...
    if (result == ESP_OK) {
        //Serial.println("Sent with success");
    }
    else {
        //Serial.println("Error sending the data");
    }

    delay(20);

}
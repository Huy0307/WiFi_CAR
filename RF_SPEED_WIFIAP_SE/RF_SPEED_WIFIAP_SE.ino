#include <WiFi.h>
#include <ezButton.h>
#include <esp_now.h>
typedef struct data_structure {
  int x1;
  int y1;
  int x2;
  int y2;
  bool isButtonPressed1;
  bool isButtonPressed2;
} data_structure;
uint8_t broadcastAddress1[] = { 0xC8, 0xF0, 0x9E, 0xF3, 0x04, 0x34 };
esp_now_peer_info_t peerInfo;
void delay_millis(unsigned long ms) {
  unsigned long current_time = millis();
  while (millis() - current_time < ms) {
  }
}
data_structure sending_data;
const int JOY_STICK_VRX1 = 34;
const int JOY_STICK_VRY1 = 35;
const int PUSH_BUTTON1 = 25;
ezButton button(PUSH_BUTTON1);
const int JOY_STICK_VRX2 = 36;
const int JOY_STICK_VRY2 = 39;
const int PUSH_BUTTON2 = 4;
float x_axis1, y_axis1, x_axis2, y_axis2;
#include "Arduino.h"
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
           mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success"
                                                : "Delivery Fail");
}

void setup() {
  pinMode(JOY_STICK_VRX1, INPUT);
  pinMode(JOY_STICK_VRY1, INPUT);
  pinMode(PUSH_BUTTON1, INPUT_PULLUP);
  pinMode(JOY_STICK_VRX2, INPUT);
  pinMode(JOY_STICK_VRY2, INPUT);
  pinMode(PUSH_BUTTON2, INPUT_PULLUP);
  button.setDebounceTime(50); // set debounce time to 50 milliseconds
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}
void loop() {
  button.loop(); 
  x_axis1 = analogRead(JOY_STICK_VRX1);
  y_axis1 = analogRead(JOY_STICK_VRY1);
  int sw1 = button.getState();
  x_axis2 = analogRead(JOY_STICK_VRX2);
  y_axis2 = analogRead(JOY_STICK_VRY2);
  int sw2 = digitalRead(PUSH_BUTTON2);
  Serial.print("X1_axis: ");
  Serial.print(x_axis1);
  Serial.print("\n");
  Serial.print("Y1-axis: ");
  Serial.print(y_axis1);
  Serial.print("\n");
  Serial.print("SW1: ");
  Serial.print(sw1);
  Serial.print("\n\n");

  Serial.print("X2_axis: ");
  Serial.print(x_axis2);
  Serial.print("\n");
  Serial.print("Y2-axis: ");
  Serial.print(y_axis2);
  Serial.print("\n");
  Serial.print("SW2: ");
  Serial.print(sw2);
  Serial.print("\n\n");
  if (sw1 == 0) {
    sending_data.isButtonPressed1 = sw1;
  } else {
    sending_data.isButtonPressed1 = 1;
  }
  sending_data.x1 = x_axis1;
  sending_data.y1 = y_axis1;
  sending_data.isButtonPressed2 = sw2;
  sending_data.x2 = x_axis2;
  sending_data.y2 = y_axis2;
  esp_err_t result =
    esp_now_send(0, (uint8_t *)&sending_data, sizeof(data_structure));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
}

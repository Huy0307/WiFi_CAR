#include <WiFi.h>
#include <ESP32Servo.h>
#include <esp_now.h>
Servo x_servo;
Servo y_servo;
Servo z_servo;
int y_pos = 0;
int x_pos = 0;
int y_servoPin = 32;
int x_servoPin = 33;
int z_servoPin = 25;
float zservoAngle;
void delay_millis(unsigned long ms) {
  unsigned long current_time = millis();
  while (millis() - current_time < ms) {
  }
}
typedef struct data_structure {
  int x1;
  int y1;
  int x2;
  int y2;
  bool isButtonPressed1;
  bool isButtonPressed2;
} data_structure;
data_structure receivedData;

void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("x1: ");
  Serial.println(receivedData.x1);
  Serial.print("y1: ");
  Serial.println(receivedData.y1);
  Serial.print("isButtonPressed1: ");
  Serial.println(receivedData.isButtonPressed1);
  Serial.print("\n");
  Serial.print("x2: ");
  Serial.println(receivedData.x2);
  Serial.print("y2: ");
  Serial.println(receivedData.y2);
  Serial.print("isButtonPressed2: ");
  Serial.println(receivedData.isButtonPressed2);
  Serial.println();
  int button1_val = receivedData.isButtonPressed1;
  if (button1_val == 0) {          // Nếu nút bấm được nhấn
    if (zservoAngle == 0) {        // Nếu góc quay hiện tại của servo là 0 độ
      zservoAngle = 150;           // Chuyển sang góc quay 90 độ
      z_servo.write(zservoAngle);  // Đưa servo motor vào trạng thái tương ứng
      delay_millis(10);
    } else {                       // Ngược lại
      zservoAngle = 0;             // Chuyển sang góc quay 0 độ
      z_servo.write(zservoAngle);  // Đưa servo motor vào trạng thái tương ứng
      delay_millis(10);
    }
  }
  int joystickValue_1 = receivedData.y1;
  float servoAngle;
  if (joystickValue_1 >= 1600 && joystickValue_1 <= 2000) {
    servoAngle = 105.5;
    y_servo.write(servoAngle);
  } else {
    servoAngle = map(joystickValue_1, 0, 4095, 180, 50);
    y_servo.write(servoAngle);
  }
  int lastjoystickValue_2;
  int joystickValue_2 = receivedData.x2;
  int escSpeed;
  if (joystickValue_2 != lastjoystickValue_2) {  // kiểm tra xem giá trị joystick có thay đổi hay không
    escSpeed = map(joystickValue_2, 0, 4095, 1000, 2000);
    x_servo.write(escSpeed);
    lastjoystickValue_2 = joystickValue_2;  // lưu giá trị joystick mới vào biến lastjoystickValue_2
  }
}


void setup() {
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  x_servo.setPeriodHertz(50);
  x_servo.attach(x_servoPin, 100, 2000);
  y_servo.setPeriodHertz(50);
  y_servo.attach(y_servoPin, 1000, 2000);
  z_servo.setPeriodHertz(50);
  z_servo.attach(z_servoPin, 1000, 2000);
  Serial.begin(115200);
  zservoAngle = 0;
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
}
void loop() {
  esp_now_register_recv_cb(OnDataRecv);
}

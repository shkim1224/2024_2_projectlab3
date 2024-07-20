// 본 프로그램은 arduino nano rp2040 connect에 부착된 가속도 센서값을 원격의 라파에 mqtt 통신을 사용하여 topic을 전송하는 프로그램
#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <Arduino_LSM6DSOX.h>

// WiFi 설정
const char* ssid = " ";
const char* password = " ";

// MQTT 설정
const char* broker = "192.168.0.47";
int port = 1883;
const char* topic = "sensor/data";

WiFiClient wifiClient;
MqttClient mqttclient(wifiClient);  // MqttClient class로부터 mqttClient() 객체를 생성

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // WiFi 연결
  Serial.print("Connecting to WiFi");
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // MQTT 연결
  mqttclient.connect(broker, port);
  // while (!mqttclient.connect("arduinoNanoRP2040")) {
  //   Serial.print(".");
  //   delay(1000);
  // }
  Serial.println("\nConnected to MQTT broker");

  // IMU 초기화
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.println("IMU initialized!");
}

void loop() {
  float x, y, z;

  // IMU에서 가속도 값 읽기
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    // JSON 형식으로 데이터 생성
    String json = "{";
    json += "\"x\":" + String(x, 6) + ",";
    json += "\"y\":" + String(y, 6) + ",";
    json += "\"z\":" + String(z, 6);
    json += "}";

    // MQTT 메시지 전송
    mqttclient.beginMessage(topic);
    mqttclient.print(json);
    mqttclient.endMessage();

    Serial.println("Sent: " + json);

    delay(1000); // 데이터 전송 주기 조절
  }
}


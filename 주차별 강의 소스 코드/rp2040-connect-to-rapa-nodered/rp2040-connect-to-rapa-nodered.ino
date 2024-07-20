// 본 프로그램은 arduino nano rp2040 connect에 부착된 가속도 센서값을 json 형식으로 원격의 rapa에 설치된
// node-red의 http-in 을 통해 전송하는 프로그램임
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <Arduino_LSM6DSOX.h>

// WiFi 설정
const char* ssid = " ";  // 자신의 wifi 정보
const char* password = " ";  // wifi 비번

// 서버 설정
const char* serverAddress = "192.168.0.47";  // 라즈베리파이 wifi ip 주소
int port = 1880; // Node-RED의 기본 포트

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);

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

    // HTTP POST 요청 보내기
    client.beginRequest();
    client.post("/imu"); // Node-RED의 HTTP-in 노드 엔드포인트
    client.sendHeader("Content-Type", "application/json");
    client.sendHeader("Content-Length", json.length());
    client.beginBody();
    client.print(json);
    client.endRequest();

    // 서버 응답 확인
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();
    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);

    delay(1000); // 데이터 전송 주기 조절
  }
}


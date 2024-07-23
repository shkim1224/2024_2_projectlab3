#include <WiFiNINA.h>
#include <Arduino_LSM6DSOX.h>
#include <ArduinoHttpClient.h>

// WiFi 설정
const char* ssid = "iptime_shkim_24";
const char* password = "ksh89377";

// 서버 설정
const char* serverAddress = "192.168.0.58";  // Node-RED가 실행 중인 서버의 IP 주소
int port = 1880;  // Node-RED의 HTTP-in 노드가 사용하는 포트

WiFiClient wifiClient;
//HttpClient client = HttpClient(wifiClient, serverAddress, port);
HttpClient client(wifiClient, serverAddress, port);


void setup() {
  Serial.begin(9600);

  // WiFi 연결 시도
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // IMU 초기화
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.println("IMU initialized");
}

void loop() {
  float x, y, z;

  // 가속도 값 읽기
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    // 현재 시간 (밀리초 단위)
    unsigned long currentTime = millis();
    // JSON 데이터 생성
    String json = "{\"x\":" + String(x) + ", \"time\":" + String(currentTime) + "}";
    // HTTP POST 요청 전송
    client.beginRequest();
    client.post("/temp");  // Node-RED의 HTTP-in 노드의 엔드포인트
    client.sendHeader("Content-Type", "application/json");
    client.sendHeader("Content-Length", json.length());
    client.beginBody();
    client.print(json);
    client.endRequest();
    // 응답 확인
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();
    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);
  }

  delay(1000);  // 1초마다 데이터 전송
}

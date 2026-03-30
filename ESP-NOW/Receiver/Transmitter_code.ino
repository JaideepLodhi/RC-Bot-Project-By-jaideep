#include <WiFi.h>
#include <esp_now.h>
// Replace With your ESP MAC address 88:57:21:B6:55:38
uint8_t receiverMAC[] = {0x88, 0x57, 0x21, 0xB6, 0x55, 0x38};

typedef struct struct_message {
  char command;
} struct_message;

struct_message dataToSend;
esp_now_peer_info_t peerInfo;

// Button Pins
#define BTN_F 14
#define BTN_B 27
#define BTN_L 26
#define BTN_R 25

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  esp_now_add_peer(&peerInfo);

  // Button setup
  pinMode(BTN_F, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_L, INPUT_PULLUP);
  pinMode(BTN_R, INPUT_PULLUP);
}

void loop() {

  bool f = digitalRead(BTN_F) == LOW;
  bool b = digitalRead(BTN_B) == LOW;
  bool l = digitalRead(BTN_L) == LOW;
  bool r = digitalRead(BTN_R) == LOW;

  char command = 'S'; // default stop

  // DIAGONALS FIRST (priority)
  if (f && l) command = 'G';
  else if (f && r) command = 'I';
  else if (b && l) command = 'H';
  else if (b && r) command = 'J';

  // SINGLE DIRECTIONS
  else if (f) command = 'F';
  else if (b) command = 'B';
  else if (l) command = 'L';
  else if (r) command = 'R';

  // Send command
  sendCommand(command);

  delay(80); // stable + smooth control
}

void sendCommand(char cmd) {
  dataToSend.command = cmd;

  esp_now_send(receiverMAC, (uint8_t *)&dataToSend, sizeof(dataToSend));

  Serial.print("Sent: ");
  Serial.println(cmd);
}

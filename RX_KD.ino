#include <SPI.h>
#include <RF24.h>
#define rl1 4
#define rl2 5
#define rl3 6
#define rl4 7
char received[30] = "";
int relayPins[] = {rl1, rl2, rl3, rl4}; // mảng các chân relay
int pin;
RF24 radio(9,8);
const byte address[6] = "12345";

void setup_pin(){
  pinMode(rl1,OUTPUT);
  pinMode(rl2,OUTPUT);
  pinMode(rl3,OUTPUT);
  pinMode(rl4,OUTPUT);
  digitalWrite(rl1,HIGH);
  digitalWrite(rl2,HIGH);
  digitalWrite(rl3,HIGH);
  digitalWrite(rl4,HIGH);
}
void setup_rf(){
    radio.begin();
    radio.openReadingPipe(1, address);
    radio.setPALevel(RF24_PA_MIN);
    radio.setChannel(80);
    radio.setDataRate(RF24_250KBPS);
    radio.startListening();
}
void setup() {
    Serial.begin(9600);
    setup_pin();
    setup_rf();
}

void loop() {
    if (radio.available()) {
    radio.read(&received, sizeof(received));
    if (received[0] >= '1' && received[0] <= '4') {
    int motorID = received[0] - '1';  // đổi '1'->0, '2'->1,...
    int motorPins[] = {rl1, rl2, rl3, rl4};

    if (strstr(received, "ON")) {
      digitalWrite(motorPins[motorID], LOW);  // LOW để bật (relay thường active-low)
    } else if (strstr(received, "OFF")) {
      digitalWrite(motorPins[motorID], HIGH); // HIGH để tắt
    }
  }
} 

}

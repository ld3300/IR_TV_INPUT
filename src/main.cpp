
#include <Arduino.h>
#include <Adafruit_CircuitPlayground.h>
#include "codes.h"

#if !defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS)
  #error "Infrared support is only for the Circuit Playground Express, it doesn't work with the Classic version"
#endif

// User Variables //
#define LEDONTIME 1000                    // Turn off LEDs after this time
#define IR_PROTOCOL NECX                  // Vizio TV Remote Protocol
#define TV_ID 0x20DF0000               // Vizio tv product rx ID
#define NUM_BITS 32

#define IR_RX_PROTOCOL SONY
#define IR_RX_VALUE 682823
#define IR_RX_BITS 20

// constant variables //
unsigned long circle_color = 0;           // Store circle current color for iterating
unsigned long lastTime = 0;               // For keeping track of pixel color timeout

// Functions //
void SetPixelColor(bool clearAll){        // Set each pixel to a random color (if clearAll false)
  circle_color += random(0x00, 0xFFFFFF);
  for(int i = 0; i < 10; i++){
    unsigned long color = clearAll ? 0x00 : circle_color;
    CircuitPlayground.setPixelColor(i, color & 0xFFFFFF);
    lastTime = millis();
  }
}

uint8_t receiveCommand(){                 // analyze data from esp module
  char temp[30];
  uint8_t code = 0;                         // store return code.  Return 0 if no code received
  Serial.readBytes(temp, 30);
  // Serial.println()
  if (strncmp(temp, "esp", 3) == 0){         // if message from esp to circuit playground
    SetPixelColor(false);
    memmove(temp, temp+3, sizeof temp - sizeof *temp);
    for (int i = 0; temp[i]; i++){
      if((temp[i] & 0xDF >= 'a' ) && (temp[i] & 0xDF <= 'z' )){
        temp[i] &= 0b11011111;
      }
    }
    if(temp == "ON"){
      code = POWERONOFF;
    }
    else if(temp == "OFF"){
      code = POWEROFF;
    }
    else if(temp == "HDMI 1"){
      code = HDMI1;
    }
    else if(temp == "HDMI 2"){
      code = HDMI2;
    }
    else if(temp == "HDMI 3"){
      code = HDMI3;
    }
    else if(temp == "HDMI 4"){
      code = HDMI4;
    }
    else{
      code = 0;
    }
  }
  return code;
}

void SendIR(uint8_t command){
  // Serial.println(command, HEX);
  char buffer[200];
  sprintf(buffer, "irCommand: %#X test: %#X\n", command, (TV_ID & 0xFFFF0000) + (command << 8)+ (~command & 0xFF));
  Serial.print(buffer);
  uint32_t irCode = (TV_ID & 0xFFFF0000) + (command << 8) + (~command & 0xFF);
  CircuitPlayground.irSend.send(IR_PROTOCOL, irCode, NUM_BITS);
}

void CheckInput(uint8_t IR_protocol, uint32_t IR_value, uint16_t IR_bits){    // Check if Input command received
  if(IR_protocol == IR_RX_PROTOCOL && IR_value == IR_RX_VALUE && IR_bits == IR_RX_BITS){       // Received signal from dish remote
    SendIR(HDMINEXT);
    SetPixelColor(false);
  }
}

void CheckIR(){                                       // Decode results from IR remote
  if(CircuitPlayground.irReceiver.getResults()) { 
    if(CircuitPlayground.irDecoder.decode()) {
      CircuitPlayground.irDecoder.dumpResults(false);
      uint8_t IR_protocol = CircuitPlayground.irDecoder.protocolNum;
      uint32_t IR_value = CircuitPlayground.irDecoder.value;
      uint16_t IR_bits = CircuitPlayground.irDecoder.bits;
      CheckInput(IR_protocol, IR_value, IR_bits);   // Check if received signal for switch TV inputs
    }
    CircuitPlayground.irReceiver.enableIRIn();      // Restart receiver
  }
}

void setup() {
  Serial.begin(115200);
  CircuitPlayground.begin();
  CircuitPlayground.irReceiver.enableIRIn(); // Start the receiver
  // IR_protocol=0; //  Indicates we've not received a code yet
}

void loop() {
  if(millis() > lastTime + LEDONTIME) SetPixelColor(true);    // clear LEDs after 1 second
  CheckIR();
  if(Serial.available()){ 
    uint8_t tCom = receiveCommand();
    SendIR(tCom);
  }

  // if (CircuitPlayground.leftButton() || CircuitPlayground.rightButton()) {  // For testing
  //   CircuitPlayground.irSend.send(MY_PROTOCOL,MY_INPUT,MY_BITS);
  //   SetPixelColor(false);
  //   while (CircuitPlayground.leftButton()) {}//wait until button released
  // }
}


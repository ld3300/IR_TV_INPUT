#include <Arduino.h>

#include <Adafruit_CircuitPlayground.h>

#if !defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS)
  #error "Infrared support is only for the Circuit Playground Express, it doesn't work with the Classic version"
#endif

#define LEDONTIME 1000                    // Turn off LEDs after this time

#define MY_PROTOCOL NECX                // Vizio TV Remote Protocol
#define MY_BITS 32
#define MY_INPUT 0x20DFF40B             // Vizio tv switch inputs

unsigned long circle_color = 0;           // Store circle current color for iterating
unsigned long lastTime = 0;               // For keeping track of pixel color timeout

// void receiveCommand(){                        // Command to see if data present for remote command
//   char temp[30];
//     Serial.readBytes(temp,5);
//     if(temp[0] == 'c'){                         // use

//     }
//     return temp;
//   }
// }

void SetPixelColor(bool clearAll){                    // Set each pixel to a random color (if clearAll false)
  for(int i = 0; i < 10; i++){
    circle_color += random(0x00, 0xFFFFFF);
    unsigned long color = clearAll ? 0x00 : circle_color;
    CircuitPlayground.setPixelColor(i, color & 0xFFFFFF);
    lastTime = millis();
  }
}

void CheckInput(uint8_t IR_protocol, uint32_t IR_value, uint16_t IR_bits){    // Check if Input command received
  if(IR_protocol == SONY && IR_value == 682823 && IR_bits == 20){       // Received signal from dish remote
    CircuitPlayground.irSend.send(MY_PROTOCOL,MY_INPUT,MY_BITS);
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
  CircuitPlayground.begin();
  CircuitPlayground.irReceiver.enableIRIn(); // Start the receiver
  // IR_protocol=0; //  Indicates we've not received a code yet
}


void loop() {
  if(millis() > lastTime + LEDONTIME){      // After 1 second turn LEDs off
    SetPixelColor(true);
  }
  CheckIR();
  if (CircuitPlayground.leftButton() || CircuitPlayground.rightButton()) {  // For testing
    CircuitPlayground.irSend.send(MY_PROTOCOL,MY_INPUT,MY_BITS);
    SetPixelColor(false);
    while (CircuitPlayground.leftButton()) {}//wait until button released
  }
  // if(Serial.available()) uint8_t tCom = receiveCommand();

}


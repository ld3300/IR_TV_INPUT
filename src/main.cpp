/*////////////////////////////////////////////////
Vizio TV Commands
Syntax is:
1st byte - device ID
2nd byte - device ID logical inverse
3rd byte - command
4th byte - command logical inverse

"Function not available" is the message the TV displayed for this code.  Mode specific command unknown.

Device ID is 0x20

0x01 ???????
0x0C Function not available
0x0F Input Media
0x10 Power on/off
0x19 Input RGB
0x21 Input HDMI4
0x2A Function not available
0x2C Function not available
0x3B Function not available
0x40 Volume +
0x41 Input HDMI2
0x4A Function not available
0x4C Function not available
0x52 Function not available
0x54 TV On
0x58 switch to last input
0x5A Input Component
0x64 Next HDMI Input (cycles HDMIs)
0x66 Function not available
0x6B Input DTV/TV
0x6C Function not available
0x74 Function not available
0x81 Input HDMI1
0x85 Input AV
0x8A Input AV
0x8E Input Component
0x90 Mute/Unmute
0xA4 Power Off
0xAA Function not available
0xAC Function not available
0xC0 Volume -
0xC1 INPUT HDMI1
0xC2 Menu On
0xCA Function not available
0xCC Function not available
0xD8 Input HDMI3
0xEC Function not available
0xF4 Switch inputs
*/

#include <Arduino.h>

#include <Adafruit_CircuitPlayground.h>

#if !defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS)
  #error "Infrared support is only for the Circuit Playground Express, it doesn't work with the Classic version"
#endif

#define LEDONTIME 1000                    // Turn off LEDs after this time

#define MY_PROTOCOL NECX                  // Vizio TV Remote Protocol
#define MY_BITS 32
#define MY_INPUT 0x20DFF40B               // Vizio tv switch inputs

uint32_t irCode = 0x20DF00FF;
uint8_t irCommand = 0xFF;

unsigned long circle_color = 0;           // Store circle current color for iterating
unsigned long lastTime = 0;               // For keeping track of pixel color timeout


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
  Serial.begin(115200);
  CircuitPlayground.begin();
  CircuitPlayground.irReceiver.enableIRIn(); // Start the receiver
  // IR_protocol=0; //  Indicates we've not received a code yet
}

// This will increment through each of 256 commands on each button press
void loop() {
  if(millis() > lastTime + LEDONTIME){      // After 1 second turn LEDs off
    SetPixelColor(true);
  }
  // CheckIR();
  if (CircuitPlayground.rightButton()) {  // For testing
    // SetPixelColor(false);
    while (CircuitPlayground.rightButton()) {}
    irCommand ++;
    Serial.println(irCode, HEX);
    char buffer[200];
    sprintf(buffer, "irCommand: %#X test: %#X\n", irCommand, (irCode & 0xFFFF0000) + (irCommand <<8)+ (~irCommand & 0xFF));
    Serial.print(buffer);
    irCode = (irCode & 0xFFFF0000) + (irCommand << 8) + (~irCommand & 0xFF);
    CircuitPlayground.irSend.send(MY_PROTOCOL,irCode,MY_BITS);
    delay(100); // to eat debounce
  }
   if (CircuitPlayground.leftButton()) {  // For testing
    // SetPixelColor(false);
    while (CircuitPlayground.leftButton()) {
      if(CircuitPlayground.rightButton()){
        while(CircuitPlayground.leftButton()){}
        irCommand --;
      }
    }//wait until button released
    Serial.println(irCode, HEX);
    char buffer[200];
    sprintf(buffer, "irCommand: %#X test: %#X\n", irCommand, (irCode & 0xFFFF0000) + (irCommand <<8)+ (~irCommand & 0xFF));
    Serial.print(buffer);
    irCode = (irCode & 0xFFFF0000) + (irCommand << 8) + (~irCommand & 0xFF);
    CircuitPlayground.irSend.send(MY_PROTOCOL,irCode,MY_BITS);
    delay(100); // to eat debounce
  }
}


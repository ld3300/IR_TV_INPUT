/*////////////////////////////////////////////////
Vizio TV Commands
Syntax is:
1st byte - device ID
2nd byte - device ID logical inverse
3rd byte - command
4th byte - command logical inverse

"Function not available" is the message the TV displayed for this code.  Mode specific command unknown.

Device ID is 0x20

*/

// 0x01 ???????
// 0x0C Function not available
#define MEDIAINPUT      0x0F  // Input Media
#define POWERONOFF      0x10  // Power on/off
#define RGBINPUT        0x19  // Input RGB
#define HDMI4           0x21  // Input HDMI4
// 0x2A Function not available
// 0x2C Function not available
// 0x3B Function not available
#define VOLUMEUP        0x40  // Volume +
#define HDMI2           0x41  // Input HDMI2
// 0x4A Function not available
// 0x4C Function not available
// 0x52 Function not available
// 0x58 switch to last input
#define COMPONENTINPUT  0x5A  // Input Component
#define HDMINEXT        0x64  // Next HDMI Input (cycles HDMIs)
// 0x66 Function not available
#define TVINPUT         0x6B  // Input DTV/TV
// 0x6C Function not available
// 0x74 Function not available
#define HDMI1           0x81  // Input HDMI1
#define AVINPUT         0x85  // Input AV
// 0x8A Input AV
// 0x8E Input Component
#define MUTE            0x90  // Mute/Unmute
#define POWEROFF        0xA4  // Power Off
// 0xAA Function not available
// 0xAC Function not available
#define VOLUMEDOWN      0xC0  // Volume -
// 0xC1 INPUT HDMI1
// 0xC2 Menu On
// 0xCA Function not available
// 0xCC Function not available
#define HDMI3           0xD8  // Input HDMI3
// 0xEC Function not available
#define INPUTNEXT       0xF4  // Switch inputs

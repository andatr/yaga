#ifndef YAGA_ENGINE_INPUT_KEYS
#define YAGA_ENGINE_INPUT_KEYS

// the values are copied from glfw3

namespace yaga {
namespace keys {

constexpr int space        = 32;
constexpr int apostrophe   = 39;
constexpr int comma        = 44;
constexpr int minus        = 45;
constexpr int period       = 46;
constexpr int slash        = 47;
constexpr int n0           = 48;
constexpr int n1           = 49;
constexpr int n2           = 50;
constexpr int n3           = 51;
constexpr int n4           = 52;
constexpr int n5           = 53;
constexpr int n6           = 54;
constexpr int n7           = 55;
constexpr int n8           = 56;
constexpr int n9           = 57;
constexpr int semicolon    = 59;
constexpr int equal        = 61;
constexpr int a            = 65;
constexpr int b            = 66;
constexpr int c            = 67;
constexpr int d            = 68;
constexpr int e            = 69;
constexpr int f            = 70;
constexpr int g            = 71;
constexpr int h            = 72;
constexpr int i            = 73;
constexpr int j            = 74;
constexpr int k            = 75;
constexpr int l            = 76;
constexpr int m            = 77;
constexpr int n            = 78;
constexpr int o            = 79;
constexpr int p            = 80;
constexpr int q            = 81;
constexpr int r            = 82;
constexpr int s            = 83;
constexpr int t            = 84;
constexpr int u            = 85;
constexpr int v            = 86;
constexpr int w            = 87;
constexpr int x            = 88;
constexpr int y            = 89;
constexpr int z            = 90;
constexpr int leftBracket  = 91;
constexpr int backslash    = 92;
constexpr int rightBracket = 93;
constexpr int graveAccent  = 96;
constexpr int world1       = 161;
constexpr int world2       = 162;
constexpr int escape       = 256;
constexpr int enter        = 257;
constexpr int tab          = 258;
constexpr int backspace    = 259;
constexpr int insert       = 260;
constexpr int del          = 261;
constexpr int right        = 262;
constexpr int left         = 263;
constexpr int down         = 264;
constexpr int up           = 265;
constexpr int pageUp       = 266;
constexpr int pageDown     = 267;
constexpr int home         = 268;
constexpr int end          = 269;
constexpr int capsLock     = 280;
constexpr int scrollLock   = 281;
constexpr int numLock      = 282;
constexpr int printScreen  = 283;
constexpr int pause        = 284;
constexpr int f1           = 290;
constexpr int f2           = 291;
constexpr int f3           = 292;
constexpr int f4           = 293;
constexpr int f5           = 294;
constexpr int f6           = 295;
constexpr int f7           = 296;
constexpr int f8           = 297;
constexpr int f9           = 298;
constexpr int f10          = 299;
constexpr int f11          = 300;
constexpr int f12          = 301;
constexpr int f13          = 302;
constexpr int f14          = 303;
constexpr int f15          = 304;
constexpr int f16          = 305;
constexpr int f17          = 306;
constexpr int f18          = 307;
constexpr int f19          = 308;
constexpr int f20          = 309;
constexpr int f21          = 310;
constexpr int f22          = 311;
constexpr int f23          = 312;
constexpr int f24          = 313;
constexpr int f25          = 314;
constexpr int kp0          = 320;
constexpr int kp1          = 321;
constexpr int kp2          = 322;
constexpr int kp3          = 323;
constexpr int kp4          = 324;
constexpr int kp5          = 325;
constexpr int kp6          = 326;
constexpr int kp7          = 327;
constexpr int kp8          = 328;
constexpr int kp9          = 329;
constexpr int kpDecimal    = 330;
constexpr int kpDivide     = 331;
constexpr int kpMultiply   = 332;
constexpr int kpSubtract   = 333;
constexpr int kpAdd        = 334;
constexpr int kpEnter      = 335;
constexpr int kpEqual      = 336;
constexpr int leftShift    = 340;
constexpr int leftControl  = 341;
constexpr int leftAlt      = 342;
constexpr int leftSuper    = 343;
constexpr int rightShift   = 344;
constexpr int rightControl = 345;
constexpr int rightAlt     = 346;
constexpr int rightSuper   = 347;
constexpr int menu         = 348;

} // !namespace keys
namespace mouse_buttons {

constexpr int b1     = 0;
constexpr int b2     = 1;
constexpr int b3     = 2;
constexpr int b4     = 3;
constexpr int b5     = 4;
constexpr int b6     = 5;
constexpr int b7     = 6;
constexpr int b8     = 7;
constexpr int left   = b1;
constexpr int right  = b2;
constexpr int middle = b3;

} // !namespace mouse_buttons

constexpr int KEYS_NUMBER = keys::menu + 1;
constexpr int MOUSE_BUTTONS_NUMBER = mouse_buttons::b8 + 1;

} // !namespace yaga

#endif // !YAGA_ENGINE_INPUT_KEYS

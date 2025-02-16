/**
   @file arduino_build.ino
   @brief Main code for the SR71 project.
   (See project description above for details.)
*/

// -------------------------------------------------------------
// Includes
// -------------------------------------------------------------
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include "kmboxNet.h"   // Assumed to include definitions like cmd_head_t etc.
#include <Watchdog.h>
#include <USBHost_t36.h>
#include <Mouse.h>
#include <SPI.h>
#include <Wire.h>
#include <ILI9341_t3.h>
#include <XPT2046_Touchscreen.h>
#include "usb_desc.h"

void doxygenBanner(int theory);

// -------------------------------------------------------------
// TFT & Touchscreen Setup
// -------------------------------------------------------------
#define TFT_MISO 12
#define TFT_SCK 13
#define TFT_MOSI 11
#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 255
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCK, TFT_MISO);

#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000
#define TS_CS 8
#define TS_IRQ 41
XPT2046_Touchscreen ts(TS_CS, TS_IRQ);

// -------------------------------------------------------------
// Button Layout
// -------------------------------------------------------------
#define BTN_OFF_X 10
#define BTN_OFF_W 100
#define BTN_OFF_H 30
#define BTN_OFF_Y 150

#define BTN_RESET_X (BTN_OFF_X + BTN_OFF_W + 10)
#define BTN_RESET_Y BTN_OFF_Y
#define BTN_RESET_W 100
#define BTN_RESET_H 30

#define STATUS_X 10
#define FRAME_X 210
#define FRAME_Y 180
#define FRAME_W 100
#define FRAME_H 50
#define STATUS_Y_START 10
#define STATUS_LINE_HEIGHT 20
#define STATUS_MAX_LINES 4
#define STATUS_WIDTH 200
#define MIN_PRESSURE 10
#define MAX_PRESSURE 1000

#define HEARTBEAT_LED_PIN 6

// -------------------------------------------------------------
// USB Host and Mouse
// -------------------------------------------------------------
USBHost myusb;
USBHIDParser hid1(myusb);
MouseController mouse1(myusb);

// -------------------------------------------------------------
// Watchdog Setup
// -------------------------------------------------------------
Watchdog watchdog;

// -------------------------------------------------------------
// Debug & Feature Flags
// -------------------------------------------------------------
#define DEBUG_MODE 1
#define NETWORK_DEBUG_MODE 1
#define MOUSE_DEBUG_MODE 0
#define SMOOTH_MOUSE_MOVEMENT_OVERRIDE_EXISTING 1
#define EXTERNAL_SERIAL 1
#define ENABLE_WATCHDOG 0
#define MAX_MOVE_SPEED 3
#define MOUSE_MOVE_INTERVAL 1
#define WAIT_FOR_SERIAL 0

// -------------------------------------------------------------
// Mouse Packet Ring Buffer
// -------------------------------------------------------------
typedef struct __attribute__((packed))
{
  uint32_t millis_stamp;
  int16_t dx;
  int16_t dy;
  int16_t wheel;
  int16_t hwheel;
  uint8_t buttons;
} MousePacket;

#define N_MOUSE_PACKETS 16
const uint32_t minIntervalMs = MAX_MOVE_SPEED;

DMAMEM __attribute__((aligned(32))) static MousePacket mouse_ring[N_MOUSE_PACKETS];
static volatile uint8_t mouse_ring_head = 0;
static volatile uint8_t mouse_ring_tail = 0;

// Store a mouse packet into the ring buffer
static inline void storeMousePacket(int dx, int dy, int wheel, int hwheel, uint8_t btns) {
  MousePacket pkt;
  pkt.millis_stamp = millis();
  pkt.dx = dx;
  pkt.dy = dy;
  pkt.wheel = wheel;
  pkt.hwheel = hwheel;
  pkt.buttons = btns;
  mouse_ring[mouse_ring_head] = pkt;
  mouse_ring_head = (mouse_ring_head + 1) % N_MOUSE_PACKETS;
  // If the buffer is full, advance the tail to discard the oldest packet
  if (mouse_ring_head == mouse_ring_tail) {
    mouse_ring_tail = (mouse_ring_tail + 1) % N_MOUSE_PACKETS;
  }
}

// Retrieve the next mouse packet from the ring buffer
static inline bool getNextMousePacket(MousePacket &pkt) {
  if (mouse_ring_tail == mouse_ring_head)
    return false; // Buffer empty
  pkt = mouse_ring[mouse_ring_tail];
  mouse_ring_tail = (mouse_ring_tail + 1) % N_MOUSE_PACKETS;
  return true;
}

// -------------------------------------------------------------
// Ethernet / UDP Setup
// -------------------------------------------------------------
byte mac[] = {0x10, 0xA0, 0x69, 0x42, 0x00, 0x00};
IPAddress ip(192, 168, 1, 177);
unsigned int localPort = 8888;
EthernetUDP Udp;

// -------------------------------------------------------------
// Network Ring Buffer
// -------------------------------------------------------------
#define N_NET_PACKETS 60
#define UDP_BUFFER_SIZE 1500
struct Packet {
  uint8_t data[UDP_BUFFER_SIZE];
  int size;
  IPAddress remoteIP;
  uint16_t remotePort;
};
DMAMEM Packet ethernetBuffer[N_NET_PACKETS];
volatile uint8_t head = 0, tail = 0;

// -------------------------------------------------------------
// Global State Variables
// -------------------------------------------------------------
const unsigned long TIMEOUT_DURATION = 60000;
const unsigned long RESET_DURATION = 3000;
unsigned long lastActivityTime = 0;
unsigned long reset_time = 0;
bool ethernetInitialized = false;
bool ethernetConnected = false;
bool mouseConnected = false;
bool networkEnabled = true;
bool networkButtonPressed = false;
bool resetButtonPressed   = false;

static uint8_t mouse_buttons_prev = 0;
static bool monitorEnabled = false;
static IPAddress monitorIP;
static unsigned int monitorPort = 0;
static bool maskLeftButton = false;
static bool maskRightButton = false;
static bool maskMiddleButton = false;
static bool maskMovement = false;
static uint32_t transactionIndex = 0;
static bool overlayEnabled = true; // Added so processPacket() compiles
static volatile int lastDx = 0;
static volatile int lastDy = 0;
static volatile int lastWheel = 0;
static volatile int lastHWheel = 0;
static volatile uint8_t lastButtons = 0;

// -------------------------------------------------------------
// Endian Conversion Helpers
// -------------------------------------------------------------
inline int16_t ntohs(int16_t val) {
  return (val << 8) | ((val >> 8) & 0xFF);
}

inline int32_t ntohl(int32_t val) {
  return ((val >> 24) & 0x000000FF) |
         ((val >> 8)  & 0x0000FF00) |
         ((val << 8)  & 0x00FF0000) |
         ((val << 24) & 0xFF000000);
}

// -------------------------------------------------------------
// Forward Declarations for Functions
// -------------------------------------------------------------
void writeDisplay(const char *header, const char *message, int section = 0);
void sendMonitorEvent(const char *eventString);
void startAutoMouseMove(int x, int y, int durationMs);
void updateAutoMouseMove();
void handleMouse();
void processMouseMove(float x, float y, int16_t wheel, int16_t hwheel);
void updateMouseMovementState();
void simulateBezierMouseMove(int targetX, int targetY, int durationMs, int cx1, int cy1, int cx2, int cy2);
void handleLCDImageCommand(const uint8_t *data, int size);
void processPacket(const uint8_t *data, int size, IPAddress remoteIP, uint16_t remotePort);
void handleConnect(const cmd_head_t *header, IPAddress remoteIP, uint16_t remotePort);
void sendAckResponse(const cmd_head_t *header, IPAddress remoteIP, uint16_t remotePort);
void handleMonitorCommand(const uint8_t *data, int size, IPAddress remoteIP, uint16_t remotePort);
void handleMaskCommand(const uint8_t *data, int size);
void handleEthernetInitialization();
void handleHeartbeatLED();

// -------------------------------------------------------------
// Serial Utilities
// -------------------------------------------------------------
void _writeSerial(const char *message) {
#if EXTERNAL_SERIAL
  Serial8.println(message);
#else
  Serial.println(message);
#endif
}

void _beginSerial() {
#if EXTERNAL_SERIAL
  Serial8.begin(115200);
#else
  Serial.begin(115200);
#endif
}

void _endSerial() {
#if EXTERNAL_SERIAL
  Serial8.end();
#else
  Serial.end();
#endif
}

// -------------------------------------------------------------
// Soft Restart Utility
// -------------------------------------------------------------
void _softRestart() {
  _endSerial();
  SCB_AIRCR = 0x05FA0004; // Trigger system restart
}

// -------------------------------------------------------------
// Ethernet Initialization
// -------------------------------------------------------------
bool initializeEthernet() {
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    _writeSerial("Ethernet: No HW");
    writeDisplay("Ethernet:", "No HW", 1);
    return false;
  }

  if (Ethernet.begin(mac, 10000) == 0) {
    _writeSerial("Ethernet: No DHCP");
    writeDisplay("Ethernet:", "No DHCP", 1);
    Ethernet.begin(mac, ip);
  }

  if (Ethernet.linkStatus() == LinkOFF) {
    _writeSerial("Ethernet: Cable Off");
    writeDisplay("Ethernet:", "Cable Off", 1);
    return false;
  }

  char ipStr[32];
  snprintf(ipStr, sizeof(ipStr), "%d.%d.%d.%d:%d",
           Ethernet.localIP()[0], Ethernet.localIP()[1],
           Ethernet.localIP()[2], Ethernet.localIP()[3],
           localPort);
  writeDisplay("Ethernet:", ipStr, 1);
  _writeSerial("Ethernet connected");
  return true;
}

// -------------------------------------------------------------
// Hardware UUID Generation
// -------------------------------------------------------------
char deviceUUID[9] = {0};
void generateHardwareUUID() {
  uint32_t uid = HW_OCOTP_CFG0; // Read unique ID from hardware register
  snprintf(deviceUUID, sizeof(deviceUUID), "%08X", (unsigned int)uid);
}

// -------------------------------------------------------------
// TFT Display Functions
// -------------------------------------------------------------
void writeDisplay(const char *header, const char *message, int section) {
  if (section < 0 || section >= STATUS_MAX_LINES)
    return;

  tft.setTextSize(1);
  int yPos = STATUS_Y_START + (section * STATUS_LINE_HEIGHT);
  tft.fillRect(STATUS_X, yPos, STATUS_WIDTH, STATUS_LINE_HEIGHT, ILI9341_WHITE);
  tft.setCursor(STATUS_X, yPos);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);

  if (header && strlen(header) > 0) {
    tft.print(header);
    tft.print(": ");
  }
  if (message) {
    tft.print(message);
  }

#if DEBUG_MODE
  char debugStr[128];
  snprintf(debugStr, sizeof(debugStr), "[Display%d] %s%s", section, header ? header : "", message ? message : "");
  _writeSerial(debugStr);
#endif
}

void drawNetworkButton() {
  tft.fillRect(BTN_OFF_X, BTN_OFF_Y, BTN_OFF_W, BTN_OFF_H, ILI9341_WHITE);
  tft.drawRect(BTN_OFF_X, BTN_OFF_Y, BTN_OFF_W, BTN_OFF_H, ILI9341_BLACK);
  tft.setCursor(BTN_OFF_X + 5, BTN_OFF_Y + BTN_OFF_H / 2 - 4);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.print(networkEnabled ? "Disable Net" : "Enable Net");
}

void drawResetButton() {
  tft.fillRect(BTN_RESET_X, BTN_RESET_Y, BTN_RESET_W, BTN_RESET_H, ILI9341_WHITE);
  tft.drawRect(BTN_RESET_X, BTN_RESET_Y, BTN_RESET_W, BTN_RESET_H, ILI9341_BLACK);
  tft.setCursor(BTN_RESET_X + 5, BTN_RESET_Y + BTN_RESET_H / 2 - 4);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.print("Reset");
}

// -------------------------------------------------------------
// Auto Mouse Movement (Smooth Movement)
// -------------------------------------------------------------
struct AutoMoveState {
  bool active = false;
  elapsedMillis timer;
  int totalSteps = 0;
  int stepX = 0;
  int stepY = 0;
  int remainingX = 0;
  int remainingY = 0;
};
static AutoMoveState autoMove;

void startAutoMouseMove(int x, int y, int durationMs) {
  if (durationMs <= 0)
    durationMs = 10;
  autoMove.totalSteps = durationMs / 10;
  if (autoMove.totalSteps < 1)
    autoMove.totalSteps = 1;
  autoMove.stepX = x / autoMove.totalSteps;
  autoMove.stepY = y / autoMove.totalSteps;
  autoMove.remainingX = x;
  autoMove.remainingY = y;
  autoMove.timer = 0;
  autoMove.active = true;
}

void updateAutoMouseMove() {
  if (!autoMove.active)
    return;
  if (autoMove.timer >= 10) {
    autoMove.timer = 0;
    int moveX = (abs(autoMove.stepX) > abs(autoMove.remainingX)) ? autoMove.remainingX : autoMove.stepX;
    int moveY = (abs(autoMove.stepY) > abs(autoMove.remainingY)) ? autoMove.remainingY : autoMove.stepY;
    processMouseMove(moveX, moveY, 0, 0);
    autoMove.remainingX -= moveX;
    autoMove.remainingY -= moveY;
    if (autoMove.remainingX == 0 && autoMove.remainingY == 0) {
      autoMove.active = false;
    }
  }
}

// -------------------------------------------------------------
// Monitor Event Sender
// -------------------------------------------------------------
void sendMonitorEvent(const char *eventString) {
  if (!monitorEnabled)
    return;
  Udp.beginPacket(monitorIP, monitorPort);
  Udp.write(eventString);
  Udp.endPacket();
}

// -------------------------------------------------------------
// Mouse Handling (USB Host -> Device)
// -------------------------------------------------------------
enum MouseState { INIT, CONNECTED, PROCESS, CLEAR };
static MouseState state = INIT;

void handleMouse() {
  static int dx, dy, wheel, hwheel;
  static uint8_t btns, oldButtons = 0;

  switch (state) {
    case INIT:
      if (!mouseConnected) {
        mouseConnected = true;
        writeDisplay("Mouse:", "Connected", 0);
      }
      state = CONNECTED;
      break;

    case CONNECTED:
      dx = mouse1.getMouseX();
      dy = mouse1.getMouseY();
      wheel = mouse1.getWheel();
      hwheel = mouse1.getWheelH();
      btns = mouse1.getButtons();

      // Mask buttons if needed
      if (maskLeftButton)
        btns &= ~0x01;
      if (maskRightButton)
        btns &= ~0x02;
      if (maskMiddleButton)
        btns &= ~0x04;

      storeMousePacket(dx, dy, wheel, hwheel, btns);
      state = PROCESS;
      break;

    case PROCESS: {
        uint8_t changed = btns ^ oldButtons;
        if (changed & 0x01)
          sendMonitorEvent((btns & 0x01) ? "MOUSE_LEFT_PRESS" : "MOUSE_LEFT_RELEASE");
        if (changed & 0x02)
          sendMonitorEvent((btns & 0x02) ? "MOUSE_MIDDLE_PRESS" : "MOUSE_MIDDLE_RELEASE");
        if (changed & 0x04)
          sendMonitorEvent((btns & 0x04) ? "MOUSE_RIGHT_PRESS" : "MOUSE_RIGHT_RELEASE");
        oldButtons = btns;

        if (!maskMovement) {
          static int prevDx = 0, prevDy = 0, prevWheel = 0, prevHwheel = 0;
          if (dx != prevDx || dy != prevDy || wheel != prevWheel || hwheel != prevHwheel) {
            Mouse.move(dx, dy, wheel, hwheel);
            prevDx = dx; prevDy = dy; prevWheel = wheel; prevHwheel = hwheel;
          }
        }
        if (btns != mouse_buttons_prev) {
          Mouse.set_buttons((btns & 1), (btns & 2), (btns & 4), (btns & 8), (btns & 16));
          mouse_buttons_prev = btns;
        }
        state = CLEAR;
        break;
      }
    case CLEAR:
      mouse1.mouseDataClear();
      state = INIT;
      break;
  }
}

// -------------------------------------------------------------
// Non-blocking Mouse Movement State
// -------------------------------------------------------------
struct MouseMoveState {
  float remainingX = 0;
  float remainingY = 0;
  int16_t remainingWheel = 0;
  int16_t remainingHWheel = 0;
  bool active = false;
  uint32_t lastMoveTime = 0;
};
static MouseMoveState mouseMoveState;

void processMouseMove(float x, float y, int16_t wheel, int16_t hwheel) {
#if SMOOTH_MOUSE_MOVEMENT_OVERRIDE_EXISTING
  mouseMoveState.remainingX = x;
  mouseMoveState.remainingY = y;
  mouseMoveState.remainingWheel = wheel;
  mouseMoveState.remainingHWheel = hwheel;
#else
  mouseMoveState.remainingX += x;
  mouseMoveState.remainingY += y;
  mouseMoveState.remainingWheel += wheel;
  mouseMoveState.remainingHWheel += hwheel;
#endif
  mouseMoveState.active = true;
  mouseMoveState.lastMoveTime = millis();
}

void updateMouseMovementState() {
  if (!mouseMoveState.active)
    return;
  uint32_t now = millis();
  if (now - mouseMoveState.lastMoveTime >= MOUSE_MOVE_INTERVAL) {
    float dx = mouseMoveState.remainingX;
    float dy = mouseMoveState.remainingY;
    int16_t moveX = (abs(dx) > 127) ? (dx > 0 ? 127 : -127) : (int16_t)dx;
    int16_t moveY = (abs(dy) > 127) ? (dy > 0 ? 127 : -127) : (int16_t)dy;
    Mouse.move(moveX, moveY, 0, 0);
    mouseMoveState.remainingX -= moveX;
    mouseMoveState.remainingY -= moveY;
    mouseMoveState.lastMoveTime = now;
    if (mouseMoveState.remainingWheel != 0 || mouseMoveState.remainingHWheel != 0) {
      Mouse.move(0, 0, mouseMoveState.remainingWheel, mouseMoveState.remainingHWheel);
      mouseMoveState.remainingWheel = 0;
      mouseMoveState.remainingHWheel = 0;
    }
    if (mouseMoveState.remainingX == 0 && mouseMoveState.remainingY == 0)
      mouseMoveState.active = false;
  }
}

// -------------------------------------------------------------
// Bézier Curve Mouse Movement
// -------------------------------------------------------------
struct BezierState {
  elapsedMillis timer;
  float t = 0.0;
  bool active = false;
};
static BezierState bezierState;

void simulateBezierMouseMove(int targetX, int targetY, int durationMs, int cx1, int cy1, int cx2, int cy2) {
  if (!bezierState.active) {
    bezierState = {0, 0.0, true};
  }
  if (bezierState.timer >= 10 && bezierState.t <= 1.0f) {
    bezierState.timer = 0;
    float u = 1.0f - bezierState.t;
    float x = 3 * u * u * bezierState.t * cx1 + 3 * u * bezierState.t * bezierState.t * cx2 + bezierState.t * bezierState.t * bezierState.t * targetX;
    float y = 3 * u * u * bezierState.t * cy1 + 3 * u * bezierState.t * bezierState.t * cy2 + bezierState.t * bezierState.t * bezierState.t * targetY;
    processMouseMove((int)x, (int)y, 0, 0);
    bezierState.t += (10.0f / durationMs);
  } else if (bezierState.t > 1.0f) {
    bezierState.active = false;
    bezierState.t = 1.0f;
  }
}

// -------------------------------------------------------------
// LCD Image Command Handler (Not Implemented)
// -------------------------------------------------------------
void handleLCDImageCommand(const uint8_t *data, int size) {
  if (size < (int)(sizeof(cmd_head_t) + 1024)) {
#if NETWORK_DEBUG_MODE
    _writeSerial("ERROR: Invalid LCD image command");
#endif
    return;
  }
  // Example: extract and process image data...
#if NETWORK_DEBUG_MODE
  char debugStr[64];
  snprintf(debugStr, sizeof(debugStr), "LCD Image Command received");
  _writeSerial(debugStr);
#endif
}

// -------------------------------------------------------------
// Network Packet Processing
// -------------------------------------------------------------

enum Command : uint32_t
{
  CMD_CONNECT = 0x28283CAF,
  CMD_MOUSE_MOVE = 0xAEDE7345,
  CMD_AUTO_MOVE = 0xAEDE7346,
  CMD_MOUSE_LEFT = 0x9823AE8D,
  CMD_MOUSE_MIDDLE = 0x97A3AE8D,
  CMD_MOUSE_RIGHT = 0x238D8212,
  CMD_MOUSE_WHEEL = 0xFFEEAD38,
  CMD_BEZIER_MOVE = 0x5A4538A2,
  CMD_MONITOR = 0x27388020,
  CMD_MASK_MOUSE = 0x23234343,
  CMD_UNMASK_ALL = 0x23344343,
  CMD_SHOW_PIC = 0x12334883,
  CMD_REBOOT = 0xAA8855AA
};

void processPacket(const uint8_t *data, int size, IPAddress remoteIP, uint16_t remotePort) {
  if (size < (int)(sizeof(cmd_head_t))) {
#if NETWORK_DEBUG_MODE
    _writeSerial("ERROR: Invalid packet size");
#endif
    return;
  }
  const cmd_head_t *header = reinterpret_cast<const cmd_head_t *>(data);
  uint32_t cmd = ntohl(header->cmd);
  uint32_t clientMac = ntohl(header->mac);
#if NETWORK_DEBUG_MODE
  char debugStr[64];
  snprintf(debugStr, sizeof(debugStr), "CMD: 0x%08X MAC: 0x%08X", cmd, clientMac);
  _writeSerial(debugStr);
#endif
  if (!overlayEnabled)
    return;

  switch (cmd) {
    case CMD_CONNECT:
      handleConnect(header, remoteIP, remotePort);
      break;
    case CMD_MOUSE_MOVE: {
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_move_t))) {
          const mouse_move_t *move = reinterpret_cast<const mouse_move_t *>(data + sizeof(cmd_head_t));
          int32_t x = ntohl(move->x);
          int32_t y = ntohl(move->y);
          processMouseMove(x, y, 0, 0);
        }
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }
    case CMD_AUTO_MOVE: {
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(auto_move_t))) {
          const auto_move_t *autoMoveCmd = reinterpret_cast<const auto_move_t *>(data + sizeof(cmd_head_t));
          int32_t x = ntohl(autoMoveCmd->x);
          int32_t y = ntohl(autoMoveCmd->y);
          uint32_t duration = ntohl(autoMoveCmd->duration);
          startAutoMouseMove(x, y, duration);
        }
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }
    case CMD_MOUSE_LEFT: {
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_button_t))) {
          const mouse_button_t *btn = reinterpret_cast<const mouse_button_t *>(data + sizeof(cmd_head_t));
          Mouse.set_buttons(btn->state, Mouse.isPressed(MOUSE_RIGHT), Mouse.isPressed(MOUSE_MIDDLE));
        }
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }
    case CMD_MOUSE_MIDDLE: {
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_button_t))) {
          const mouse_button_t *btn = reinterpret_cast<const mouse_button_t *>(data + sizeof(cmd_head_t));
          Mouse.set_buttons(Mouse.isPressed(MOUSE_LEFT), Mouse.isPressed(MOUSE_RIGHT), btn->state);
        }
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }
    case CMD_MOUSE_RIGHT: {
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_button_t))) {
          const mouse_button_t *btn = reinterpret_cast<const mouse_button_t *>(data + sizeof(cmd_head_t));
          Mouse.set_buttons(Mouse.isPressed(MOUSE_LEFT), btn->state, Mouse.isPressed(MOUSE_MIDDLE));
        }
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }
    case CMD_MOUSE_WHEEL: {
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_wheel_t))) {
          const mouse_wheel_t *wheelCmd = reinterpret_cast<const mouse_wheel_t *>(data + sizeof(cmd_head_t));
          int32_t w = ntohl(wheelCmd->value);
          Mouse.move(0, 0, w, 0);
        }
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }
    case CMD_BEZIER_MOVE: {
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(bezier_move_t))) {
          const bezier_move_t *bezier = reinterpret_cast<const bezier_move_t *>(data + sizeof(cmd_head_t));
          int32_t x = ntohl(bezier->targetX);
          int32_t y = ntohl(bezier->targetY);
          uint32_t duration = ntohl(bezier->duration);
          int32_t cx1 = ntohl(bezier->ctrlX1);
          int32_t cy1 = ntohl(bezier->ctrlY1);
          int32_t cx2 = ntohl(bezier->ctrlX2);
          int32_t cy2 = ntohl(bezier->ctrlY2);
          simulateBezierMouseMove(x, y, duration, cx1, cy1, cx2, cy2);
        }
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }
    case CMD_MONITOR: {
        handleMonitorCommand(data, size, remoteIP, remotePort);
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }
    case CMD_MASK_MOUSE: {
        handleMaskCommand(data, size);
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }
    case CMD_UNMASK_ALL:
      maskLeftButton = maskRightButton = maskMiddleButton = maskMovement = false;
      sendAckResponse(header, remoteIP, remotePort);
      break;
    case CMD_SHOW_PIC: {
        handleLCDImageCommand(data, size);
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }
    case CMD_REBOOT:
      sendAckResponse(header, remoteIP, remotePort);
      _softRestart();
      break;
    default:
#if NETWORK_DEBUG_MODE
      {
        char debugStr[64];
        snprintf(debugStr, sizeof(debugStr), "Unknown command: 0x%08X", cmd);
        _writeSerial(debugStr);
      }
#endif
      break;
  }
}

// -------------------------------------------------------------
// Connection and Acknowledgment Helpers
// -------------------------------------------------------------
void handleConnect(const cmd_head_t *header, IPAddress remoteIP, uint16_t remotePort) {
  char receivedUUID[9];
  snprintf(receivedUUID, sizeof(receivedUUID), "%08lX", ntohl(header->mac));
  if (strncmp(receivedUUID, deviceUUID, 8) == 0) {
    monitorIP = remoteIP;
    monitorPort = remotePort;
    sendAckResponse(header, remoteIP, remotePort);
#if NETWORK_DEBUG_MODE
    _writeSerial("Valid client connected");
#endif
  } else {
    sendAckResponse(header, remoteIP, remotePort);
#if NETWORK_DEBUG_MODE
    _writeSerial("Invalid client connected");
#endif
  }
}

void sendAckResponse(const cmd_head_t *header, IPAddress remoteIP, uint16_t remotePort) {
  cmd_head_t resp;
  resp.mac = header->mac;
  resp.rand = 0;
  resp.indexpts = transactionIndex++;
  resp.cmd = header->cmd;
  Udp.beginPacket(remoteIP, remotePort);
  Udp.write((uint8_t *)&resp, sizeof(resp));
  Udp.endPacket();
}

void handleMonitorCommand(const uint8_t *data, int size, IPAddress remoteIP, uint16_t remotePort) {
  if (size < (int)(sizeof(cmd_head_t) + sizeof(monitor_cmd_t))) {
#if NETWORK_DEBUG_MODE
    _writeSerial("Invalid monitor command: packet too small");
#endif
    return;
  }
  const monitor_cmd_t *monitor_cmd = reinterpret_cast<const monitor_cmd_t *>(data + sizeof(cmd_head_t));
  monitorPort = ntohs(monitor_cmd->monitor_port);
  monitorIP = remoteIP;
  monitorEnabled = true;
#if NETWORK_DEBUG_MODE
  {
    char debugStr[64];
    snprintf(debugStr, sizeof(debugStr), "Monitoring enabled: %d.%d.%d.%d:%d",
             monitorIP[0], monitorIP[1], monitorIP[2], monitorIP[3], monitorPort);
    _writeSerial(debugStr);
  }
#endif
}

void handleMaskCommand(const uint8_t *data, int size) {
  if (size < (int)(sizeof(cmd_head_t) + sizeof(mask_cmd_t))) {
#if NETWORK_DEBUG_MODE
    _writeSerial("Invalid mask command: packet too small");
#endif
    return;
  }
  const mask_cmd_t *mask_cmd = reinterpret_cast<const mask_cmd_t *>(data + sizeof(cmd_head_t));
  maskLeftButton   = (ntohl(mask_cmd->mask_left)   != 0);
  maskRightButton  = (ntohl(mask_cmd->mask_right)  != 0);
  maskMiddleButton = (ntohl(mask_cmd->mask_middle) != 0);
  maskMovement     = (ntohl(mask_cmd->mask_movement) != 0);
#if NETWORK_DEBUG_MODE
  {
    char debugStr[64];
    snprintf(debugStr, sizeof(debugStr), "Mask: L=%d R=%d M=%d Move=%d",
             maskLeftButton, maskRightButton, maskMiddleButton, maskMovement);
    _writeSerial(debugStr);
  }
#endif
}

// -------------------------------------------------------------
// Ethernet Initialization Helper
// -------------------------------------------------------------
void handleEthernetInitialization() {
  static uint32_t lastEthernetCheck = 0;
  uint32_t now = millis();
#if ENABLE_WATCHDOG
  static uint32_t lastWatchdogReset = 0;
  if (now - lastWatchdogReset >= RESET_DURATION) {
    lastWatchdogReset = now;
    if ((now - lastActivityTime) <= TIMEOUT_DURATION)
      watchdog.reset();
  }
#endif
  if (!ethernetInitialized) {
    ethernetInitialized = initializeEthernet();
    if (ethernetInitialized) {
      ethernetConnected = true;
      Udp.begin(localPort);
    }
  }
}

// -------------------------------------------------------------
// Heartbeat LED
// -------------------------------------------------------------
void handleHeartbeatLED() {
  static uint32_t lastToggleTime = 0;
  static bool ledState = false;
  const uint32_t interval = 500; // toggle interval in milliseconds

  if (millis() - lastToggleTime >= interval) {
    lastToggleTime = millis();
    ledState = !ledState;
    digitalWrite(HEARTBEAT_LED_PIN, ledState ? HIGH : LOW);
  }
}

// -------------------------------------------------------------
// setup()
// -------------------------------------------------------------
void setup() {
  // Initialize watchdog if enabled
#if ENABLE_WATCHDOG
  watchdog.enable(Watchdog::TIMEOUT_8S);
#endif
  pinMode(HEARTBEAT_LED_PIN, OUTPUT);
  _beginSerial();
  _writeSerial("Setting up display...");
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextWrap(false);
  tft.setRotation(1);
  _writeSerial("Setting up touchscreen...");
  ts.begin();
  ts.setRotation(1);
  drawNetworkButton();
  drawResetButton();
  myusb.begin();
  _writeSerial("USB Host initialized");
  writeDisplay("Mouse:", "Disconnected", 0);
  ethernetInitialized = false;
  ethernetConnected = false;
  generateHardwareUUID();
  writeDisplay("UUID", deviceUUID, 2);
  char uuidStr[32];
  snprintf(uuidStr, sizeof(uuidStr), "UUID: %s", deviceUUID);
  _writeSerial(uuidStr);
  Mouse.begin();
  _writeSerial("Mouse started");
  writeDisplay("Service", "Offline", 3);
  lastActivityTime = millis();
  reset_time = millis();
  _writeSerial("Setup complete");
}

// -------------------------------------------------------------
// loop()
// -------------------------------------------------------------
void loop() {
  uint32_t now = millis();
  myusb.Task();
  handleMouse();
  updateAutoMouseMove();
  updateMouseMovementState();
  static uint32_t lastMousePacketCheck = 0;
  if (now - lastMousePacketCheck >= 10) {
    lastMousePacketCheck += 10;
#if MOUSE_DEBUG_MODE
    MousePacket p;
    while (getNextMousePacket(p)) {
      uint32_t latency = now - p.millis_stamp;
      char debugStr[128];
      snprintf(debugStr, sizeof(debugStr),
               "Captured host mouse => dx=%d dy=%d wheel=%d hwheel=%d btn=0x%02X (Latency: %u ms)",
               p.dx, p.dy, p.wheel, p.hwheel, p.buttons, (unsigned int)latency);
      _writeSerial(debugStr);
    }
#else
    MousePacket p;
    while (getNextMousePacket(p)) {
      /* do nothing */
    }
#endif
  }

  // Ethernet link check
  if (ethernetInitialized) {
    if (Ethernet.linkStatus() == LinkOFF) {
      if (ethernetConnected) {
        writeDisplay("Ethernet:", "Cable Gone", 1);
#if NETWORK_DEBUG_MODE
        _writeSerial("Cable gone");
#endif
        ethernetConnected = false;
      }
    } else {
      if (!ethernetConnected) {
        writeDisplay("Ethernet:", "Cable On", 1);
#if NETWORK_DEBUG_MODE
        _writeSerial("Cable on");
#endif
        ethernetConnected = true;
      }
    }
  }

  if (ethernetInitialized && ethernetConnected) {
    int packetSize;
    while ((packetSize = Udp.parsePacket()) > 0) {
#if NETWORK_DEBUG_MODE
      char debugStr[64];
      snprintf(debugStr, sizeof(debugStr), "Received packet: %d bytes", packetSize);
      _writeSerial(debugStr);
#endif
      if ((head + 1) % N_NET_PACKETS != tail) {
        ethernetBuffer[head].remoteIP = Udp.remoteIP();
        ethernetBuffer[head].remotePort = Udp.remotePort();
        ethernetBuffer[head].size = Udp.read(ethernetBuffer[head].data, UDP_BUFFER_SIZE);
        head = (head + 1) % N_NET_PACKETS;
      }
    }
    static uint32_t lastPacketProcessTime = 0;
    if (now - lastPacketProcessTime >= 10) {
      lastPacketProcessTime = now;
      if (tail != head) {
        processPacket(ethernetBuffer[tail].data,
                      ethernetBuffer[tail].size,
                      ethernetBuffer[tail].remoteIP,
                      ethernetBuffer[tail].remotePort);
        tail = (tail + 1) % N_NET_PACKETS;
      }
    }
  } else {
    handleEthernetInitialization();
  }
  TS_Point p = ts.getPoint();
  if (p.z > MIN_PRESSURE && p.z < MAX_PRESSURE) {
#if DEBUG_MODE
    _writeSerial("got touched");
#endif
    // Process network toggle button touch
    if (p.x >= BTN_OFF_X && p.x <= (BTN_OFF_X + BTN_OFF_W) &&
        p.y >= BTN_OFF_Y && p.y <= (BTN_OFF_Y + BTN_OFF_H)) {
      if (!networkButtonPressed) {  // debounce the button
#if DEBUG_MODE
        _writeSerial("restart button");
#endif
        networkEnabled = !networkEnabled;
        if (networkEnabled) {
          ethernetInitialized = initializeEthernet();
          if (ethernetInitialized) {
            ethernetConnected = true;
            Udp.begin(localPort);
          }
          writeDisplay("Ethernet", "Enabled", 0);
        } else {
          writeDisplay("Ethernet", "Disabled", 0);
        }
        drawNetworkButton();  // redraw to reflect the new state
        networkButtonPressed = true;
      }
    } else {
      networkButtonPressed = false;
    }

    // Process reset button touch
    if (p.x >= BTN_RESET_X && p.x <= (BTN_RESET_X + BTN_RESET_W) &&
        p.y >= BTN_RESET_Y && p.y <= (BTN_RESET_Y + BTN_RESET_H)) {
      if (!resetButtonPressed) {  // debounce the reset button
#if DEBUG_MODE
        _writeSerial("reset button");
#endif
        writeDisplay("Service", "Resetting", 3);
        delay(100);  // Allow the message to be seen briefly
        _softRestart();  // Call your soft restart function
        resetButtonPressed = true;
      }
    }
  }
  handleHeartbeatLED();

#if ENABLE_WATCHDOG
  if ((now - lastActivityTime <= TIMEOUT_DURATION) && (now - reset_time >= RESET_DURATION)) {
    reset_time = now;
    watchdog.reset();
  }
#endif
}
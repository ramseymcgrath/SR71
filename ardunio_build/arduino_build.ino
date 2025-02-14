// -------------------------------------------------------------
// Whole bunch of includes
// -------------------------------------------------------------
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include "kmboxNet.h"
#include <Watchdog.h>
#include <USBHost_t36.h>
#include <Mouse.h>
#include <SPI.h>
#include <Wire.h>
#include <ILI9341_t3.h>
#include <XPT2046_Touchscreen.h>

// -------------------------------------------------------------
// Arguments for display
// -------------------------------------------------------------
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000
#define TOUCH_CS  8
XPT2046_Touchscreen ts(TOUCH_CS);
#define TIRQ_PIN  2

#define TFT_DC  9
#define TFT_CS 10
#define TFT_RST 7
#define TFT_SCK 13
#define TFT_MISO 12
#define TFT_MOSI 11
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCK, TFT_MISO);

#define FRAME_X 210
#define FRAME_Y 180
#define FRAME_W 100
#define FRAME_H 50

// Display layout constants
#define STATUS_X 10
#define STATUS_Y_START 10
#define STATUS_LINE_HEIGHT 20  // Adjust based on font height
#define STATUS_MAX_LINES 4
#define STATUS_WIDTH 200
// -------------------------------------------------------------
// USB Host Setup
// -------------------------------------------------------------
USBHost         myusb;        // The USB host object
USBHIDParser    hid1(myusb);  // HID parser
MouseController mouse1(myusb);

// Watchdog timer
Watchdog watchdog;

// -------------------------------------------------------------
// Debug Modes
// DEBUG_MODE is general messages, NETWORK_DEBUG is more general
// -------------------------------------------------------------
#define DEBUG_MODE  1
#define NETWORK_DEBUG_MODE  1

// -----------------------------
// Feature flags (TODO: add some to ui)
// -----------------------------
#define SMOOTH_MOUSE_MOVEMENT_OVERRIDE_EXISTING 1 // override existing movements when smoothing or add to end of buffer 
#define EXTERNAL_SERIAL 1 // allows debugging without a main pc com port
#define ENABLE_WATCHDOG 0 // gracefully handle crashes
#define MAX_MOVE_SPEED 3 // max packet output from device (make this something reasonable)
#define MOUSE_MOVE_INTERVAL // set as low as possible, this is number of times the mouse position updates in chip

// -------------------------------------------------------------
// Mouse Packet Ring Buffer (for storing host mouse events)
// -------------------------------------------------------------
typedef struct __attribute__((packed)) {
  uint32_t millis_stamp;
  int16_t  dx;
  int16_t  dy;
  int16_t  wheel;
  int16_t  hwheel;
  uint8_t  buttons;
} MousePacket;

#define N_MOUSE_PACKETS 16
// Use this to cap the number of updates that can be sent to avoid empty packet spam
const uint32_t minIntervalMs = MAX_MOVE_SPEED;

// Place the ring buffer in DMAMEM for faster access
DMAMEM __attribute__((aligned(32))) static MousePacket mouse_ring[N_MOUSE_PACKETS];
static volatile uint8_t mouse_ring_head = 0;
static volatile uint8_t mouse_ring_tail = 0;
void processPacket(const uint8_t* data, int size, IPAddress remoteIP, uint16_t remotePort);

// -------------------------------------------------------------
// Ethernet / UDP Setup
// -------------------------------------------------------------
byte mac[] = { 0x10, 0xA0, 0x69, 0x42, 0x00, 0x00 };
IPAddress ip(192, 168, 1, 177); // fallback if DHCP fails
unsigned int localPort = 8888;  // local UDP port

EthernetUDP Udp;

// -------------------------------------------------------------
// 7) Global flags and state variables
// -------------------------------------------------------------
const unsigned long TIMEOUT_DURATION = 60000;
const unsigned long RESET_DURATION = 3000;
unsigned long enabled_time;
unsigned long reset_time;
bool ethernetInitialized = false;
bool ethernetConnected   = false;
bool mouseConnected = false;
static uint8_t mouse_buttons_prev = 0;
static bool monitorEnabled   = false;
static IPAddress monitorIP;
static unsigned int monitorPort = 0;
static bool maskLeftButton   = false;
static bool maskRightButton  = false;
static bool maskMiddleButton = false;
static bool maskMovement     = false;
static uint32_t transactionIndex = 0;
#define UDP_BUFFER_SIZE 1500 // Standard Ethernet MTU
static const uint32_t MOVE_INTERVAL_MS = 1;
bool overlayEnabled = true;

inline int16_t ntohs(int16_t val) {
  return (val << 8) | ((val >> 8) & 0xFF);
}

inline int32_t ntohl(int32_t val) {
  return ((val >> 24) & 0x000000FF) |
         ((val >> 8)  & 0x0000FF00) |
         ((val << 8)  & 0x00FF0000) |
         ((val << 24) & 0xFF000000);
}

// ------------------
// Network ring buffer
// -----------------
#define N_NET_PACKETS 60 // raise this to support more network traffic if needed
struct Packet {
  uint8_t data[UDP_BUFFER_SIZE];
  int size;
  IPAddress remoteIP;
  uint16_t remotePort;
};
DMAMEM Packet ethernetBuffer[N_NET_PACKETS];
volatile uint8_t head = 0, tail = 0;

// -------------------------------------------------------------
// Forward Declarations
// -------------------------------------------------------------
void writeDisplay(const char *header, const char *message, int section = 0);
void sendMonitorEvent(const char* eventString);

// -------------------------------------------------------------
// Utility: Soft Restart
// -------------------------------------------------------------
void _softRestart() {
#if EXTERNAL_SERIAL
  Serial8.end();
#else
  Serial.end();
#endif
  SCB_AIRCR = 0x05FA0004; // Write value for restart
}

// -------------------------------------------------------------
// Ethernet Initialization
// -------------------------------------------------------------
bool initializeEthernet() {
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
#if EXTERNAL_SERIAL
    Serial8.println("Ethernet hardware not found.");
#else
    Serial.println("Ethernet hardware not found.");
#endif
    writeDisplay("Ethernet:", "No HW", 1);
    return false;
  }

  // Attempt DHCP with 10s timeout
  if (Ethernet.begin(mac, 10000) == 0) {
#if EXTERNAL_SERIAL
    Serial8.println("Failed DHCP. Using static IP.");
#else
    Serial.println("Failed DHCP. Using static IP.");
#endif
    writeDisplay("Ethernet:", "No DHCP", 1);
    Ethernet.begin(mac, ip);
  }

  if (Ethernet.linkStatus() == LinkOFF) {
#if EXTERNAL_SERIAL
    Serial8.println("Ethernet cable is not connected.");
#else
    Serial.println("Ethernet cable is not connected.");
#endif
    writeDisplay("Ethernet:", "Cable Off", 1);
    return false;
  }

  // Build a string for IP:Port
  char ipStr[32];
  snprintf(ipStr, sizeof(ipStr), "%d.%d.%d.%d:%d",
           Ethernet.localIP()[0], Ethernet.localIP()[1],
           Ethernet.localIP()[2], Ethernet.localIP()[3],
           localPort);
  writeDisplay("Ethernet:", ipStr, 1);
#if EXTERNAL_SERIAL
  Serial8.print("Ethernet IP:");
  Serial8.println(Ethernet.localIP());
#else
  Serial.print("Ethernet IP:");
  Serial.println(Ethernet.localIP());
#endif
  return true;
}

// -------------------------------------------------------------
// Hardware UUID Generation (10-character)
// -------------------------------------------------------------
char deviceUUID[9] = {0}; // 8 chars + null terminator

void generateHardwareUUID() {
  // Read Teensy's unique hardware ID (4 bytes)
  uint32_t uid = HW_OCOTP_CFG0; // Use first 4 bytes
  snprintf(deviceUUID, sizeof(deviceUUID),
           "%08X", // 8-character uppercase hex
           (unsigned int)uid);
}
// -------------------------------------------------------------
// Display Layout Constants
// -------------------------------------------------------------
#define STATUS_X 10
#define STATUS_Y_START 10
#define STATUS_LINE_HEIGHT 20
#define STATUS_MAX_LINES 4
#define STATUS_WIDTH 200

// Predefined sections (0-3)
// Section 0: Mouse Status (line 1)
// Section 1: Ethernet Status (line 2)
// Section 2: UUID (line 3)
// Section 3: Service Status (line 4)

// -------------------------------------------------------------
// TFT Displ
// -------------------------------------------------------------
void writeDisplay(const char *header, const char *message, int section) {
  if (section < 0 || section >= STATUS_MAX_LINES) return;

  tft.setTextSize(1);

  // Calculate position
  int yPos = STATUS_Y_START + (section * STATUS_LINE_HEIGHT);

  // Clear previous content
  tft.fillRect(STATUS_X, yPos, STATUS_WIDTH, STATUS_LINE_HEIGHT, ILI9341_BLUE);

  tft.setCursor(STATUS_X, yPos);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);

  // Fixed header handling with proper string checks
  if (header && strlen(header) > 0) {
    tft.print(header);
    tft.print(": ");
  }
  if (message) {
    tft.print(message);
  }

  // Debug output with null checks
#if EXTERNAL_SERIAL
  Serial8.printf("[Display%d] %s%s\n", section, header ? header : "", message ? message : "");
#else
  Serial.printf("[Display%d] %s%s\n", section, header ? header : "", message ? message : "");
#endif
}

void drawFrame() {
  tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, ILI9341_BLACK);
}

// -------------------------------------------------------------
// Storing and Retrieving Mouse Packets
// -------------------------------------------------------------
static inline void storeMousePacket(int dx, int dy, int wheel, int hwheel, uint8_t btns) {
  uint8_t next = (mouse_ring_head + 1) % N_MOUSE_PACKETS;
  if (next == mouse_ring_tail) {
    // buffer full, drop oldest
    mouse_ring_tail = (mouse_ring_tail + 1) % N_MOUSE_PACKETS;
  }
  mouse_ring[mouse_ring_head].millis_stamp = millis();
  mouse_ring[mouse_ring_head].dx = dx;
  mouse_ring[mouse_ring_head].dy = dy;
  mouse_ring[mouse_ring_head].wheel = wheel;
  mouse_ring[mouse_ring_head].hwheel = hwheel;
  mouse_ring[mouse_ring_head].buttons = btns;
  mouse_ring_head = next;
}

static inline bool getNextMousePacket(MousePacket &pkt) {
  if (mouse_ring_tail == mouse_ring_head) {
    return false; // empty
  }
  pkt = mouse_ring[mouse_ring_tail];
  mouse_ring_tail = (mouse_ring_tail + 1) % N_MOUSE_PACKETS;
  return true;
}

// -------------------------------------------------------------
// Auto Mouse Move Helper
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
  if (durationMs <= 0) durationMs = 10;
  autoMove.totalSteps = durationMs / 10;
  if (autoMove.totalSteps < 1) autoMove.totalSteps = 1;
  autoMove.stepX = x / autoMove.totalSteps;
  autoMove.stepY = y / autoMove.totalSteps;
  autoMove.remainingX = x;
  autoMove.remainingY = y;
  autoMove.timer = 0;
  autoMove.active = true;
}

void updateAutoMouseMove() {
  if (!autoMove.active) return;
  if (autoMove.timer >= 10) {
    autoMove.timer = 0;
    processMouseMove(autoMove.stepX, autoMove.stepY, 0, 0);
    autoMove.remainingX -= autoMove.stepX;
    autoMove.remainingY -= autoMove.stepY;
    if (autoMove.remainingX == 0 && autoMove.remainingY == 0) {
      autoMove.active = false;
    }
  }
}

// -------------------------------------------------------------
// Send Monitor Event (if monitorEnabled is true)
// -------------------------------------------------------------
void sendMonitorEvent(const char* eventString) {
  if (!monitorEnabled) return;
  Udp.beginPacket(monitorIP, monitorPort);
  Udp.write(eventString);
  Udp.endPacket();
}

// -------------------------------------------------------------
// handleMouse(): Reads host mouse (via USB Host) and
//     forwards to device-side "Mouse" if not masked.
// -------------------------------------------------------------
void handleMouse() {
  if (!mouseConnected) {
    writeDisplay("Mouse:", "Connected", 0);
    mouseConnected = true;
  }
  int dx     = mouse1.getMouseX();
  int dy     = mouse1.getMouseY();
  int wheel  = mouse1.getWheel();
  int hwheel = mouse1.getWheelH();
  uint8_t btns = mouse1.getButtons();

  // Mask buttons if needed
  if (maskLeftButton)   btns &= ~0x01;  // Left (bit 0)
  if (maskRightButton)  btns &= ~0x02;  // Right (bit 1)
  if (maskMiddleButton) btns &= ~0x04;  // Middle (bit 2)

  // Store the packet (for debugging or inspection)
  storeMousePacket(dx, dy, wheel, hwheel, btns);

  // Possibly send monitor events if button state changed
  static uint8_t oldButtons = 0;
  uint8_t changed = btns ^ oldButtons;
  if (changed & 0x01) {
    if (btns & 0x01) sendMonitorEvent("MOUSE_LEFT_PRESS");
    else             sendMonitorEvent("MOUSE_LEFT_RELEASE");
  }
  if (changed & 0x02) {
    if (btns & 0x02) sendMonitorEvent("MOUSE_MIDDLE_PRESS");
    else             sendMonitorEvent("MOUSE_MIDDLE_RELEASE");
  }
  if (changed & 0x04) {
    if (btns & 0x04) sendMonitorEvent("MOUSE_RIGHT_PRESS");
    else             sendMonitorEvent("MOUSE_RIGHT_RELEASE");
  }
  oldButtons = btns;

  // Move only if not masked
  if (!maskMovement) {
    // Only send new movement if it changed
    static int prevDx = 0, prevDy = 0, prevWheel = 0, prevHwheel = 0;
    if (dx != prevDx || dy != prevDy || wheel != prevWheel || hwheel != prevHwheel) {
      Mouse.move(dx, dy, wheel, hwheel);
      prevDx    = dx;
      prevDy    = dy;
      prevWheel = wheel;
      prevHwheel = hwheel;
    }
  }

  // Update device-side button state if changed
  if (btns != mouse_buttons_prev) {
    Mouse.set_buttons( (btns & 1),   // left
                       (btns & 4),   // right
                       (btns & 2),   // middle
                       (btns & 8),   // back
                       (btns & 16)); // forward
    mouse_buttons_prev = btns;
  }

  mouse1.mouseDataClear();
}

// State for non-blocking mouse movement
struct MouseMoveState {
  float remainingX;    // Track partial pixel movements
  float remainingY;
  int16_t remainingWheel;
  int16_t remainingHWheel;
  bool active;
  uint32_t lastMoveTime;
};

static MouseMoveState mouseMoveState = {0, 0, 0, 0, false, 0};

void processMouseMove(float x, float y, int16_t wheel, int16_t hwheel) {
  // Smooth mouse movement can be overridden to replace existing movement or
  // added to existing movement.
#if SMOOTH_MOUSE_MOVEMENT_OVERRIDE_EXISTING
  mouseMoveState.remainingX     = x;
  mouseMoveState.remainingY     = y;
  mouseMoveState.remainingWheel = wheel;
  mouseMoveState.remainingHWheel = hwheel;
#else
  mouseMoveState.remainingX     += x;
  mouseMoveState.remainingY     += y;
  mouseMoveState.remainingWheel += wheel;
  mouseMoveState.remainingHWheel += hwheel;
#endif
  mouseMoveState.active         = true;
  mouseMoveState.lastMoveTime   = millis();
}

void updateMouseMovementState() {
  if (!mouseMoveState.active) return;

  uint32_t now = millis();
  if (now - mouseMoveState.lastMoveTime >= MOVE_INTERVAL_MS) {
    // Calculate how much to move this frame
    float dx = mouseMoveState.remainingX;
    float dy = mouseMoveState.remainingY;

    // Clamp to ±127 (USB HID typical max per report)
    int16_t moveX = (abs(dx) > 127) ? (dx > 0 ? 127 : -127) : (int16_t)dx;
    int16_t moveY = (abs(dy) > 127) ? (dy > 0 ? 127 : -127) : (int16_t)dy;

    // Apply the movement
    Mouse.move(moveX, moveY, 0, 0);

    // Update remainders
    mouseMoveState.remainingX -= moveX;
    mouseMoveState.remainingY -= moveY;
    mouseMoveState.lastMoveTime = now;
  }
  else {
    if (mouseMoveState.remainingWheel != 0 || mouseMoveState.remainingHWheel != 0) {
      if (now - mouseMoveState.lastMoveTime >= MOVE_INTERVAL_MS) {
        // Move wheel/hwheel exactly once
        Mouse.move(0, 0, mouseMoveState.remainingWheel, mouseMoveState.remainingHWheel);
        // Clear them out
        mouseMoveState.remainingWheel  = 0;
        mouseMoveState.remainingHWheel = 0;
        mouseMoveState.lastMoveTime = now;
      }
    } else {
      // Everything done
      mouseMoveState.active = false;
    }
  }
}

// -------------------------------------------------------------
// Bézier Curve Mouse Motion (Non-Blocking)
// -------------------------------------------------------------
struct BezierState {
  elapsedMillis timer;
  float t = 0.0;
  int startX = 0, startY = 0;
  bool active = false;
};

BezierState bezierState;

// Simulate mouse movement along a Bézier curve
void simulateBezierMouseMove(int targetX, int targetY, int durationMs, int cx1, int cy1, int cx2, int cy2) {
  if (!bezierState.active) {
    bezierState = {0, 0.0, 0, 0, true};  // Reset state
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
    bezierState.t = 1.0f; // Clamp to 1.0
  }

}

// -------------------------------------------------------------
// Handle LCD Image Command (not implemented)
// -------------------------------------------------------------
void handleLCDImageCommand(const uint8_t* data, int size) {
  // Validate packet structure
  if (size < (int)sizeof(cmd_head_t) + 1024) { // 16-byte header + 1024 image data
#if NETWORK_DEBUG_MODE
#if EXTERNAL_SERIAL
    Serial8.println("Invalid LCD image packet");
#else
    Serial.println("Invalid LCD image packet");
#endif
#endif
    return;
  }

  // Extract image chunk
  const uint8_t* imageData = data + sizeof(cmd_head_t);
  int chunkIndex = ntohl(*(const uint32_t*)(imageData)); // First 4 bytes: chunk index

  // Write to display buffer //TODO
  //display.drawBitmap(0, 0, imageData + 4, 128, 64, SSD1306_WHITE);
  //display.display();

#if EXTERNAL_SERIAL
  Serial8.printf("LCD image chunk %d received\n", chunkIndex);
#else
  Serial.printf("LCD image chunk %d received\n", chunkIndex);
#endif
}

// ------ network handling
void processPacket(const uint8_t* data, int size, IPAddress remoteIP, uint16_t remotePort) {
  if (size < (int)(sizeof(cmd_head_t))) {
#if NETWORK_DEBUG_MODE
#if EXTERNAL_SERIAL
    Serial8.println("ERROR: Packet too small");
#else
    Serial.println("ERROR: Packet too small");
#endif
#endif
    return;
  }
  const cmd_head_t* header = reinterpret_cast<const cmd_head_t*>(data);
  uint32_t cmd = ntohl(header->cmd);
  uint32_t clientMac = ntohl(header->mac);
#if NETWORK_DEBUG_MODE
#if EXTERNAL_SERIAL
  Serial8.printf("CMD: 0x%08X MAC: 0x%08X\n", cmd, clientMac);
#else
  Serial.printf("CMD: 0x%08X MAC: 0x%08X\n", cmd, clientMac);
#endif
#endif
  if (!overlayEnabled) {
    return;
  }
  switch (cmd) {
    // -------------------------------
    // Connection Command
    // -------------------------------
    case 0x28283CAF: // cmd_connect
      handleConnect(header, remoteIP, remotePort);
      break;

    // -------------------------------
    // Mouse Movement Commands
    // -------------------------------
    case 0xAEDE7345: { // cmd_mouse_move
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_move_t))) {
          const mouse_move_t* move = reinterpret_cast<const mouse_move_t*>(data + sizeof(cmd_head_t));
          int32_t x = ntohl(move->x);
          int32_t y = ntohl(move->y);
          processMouseMove(x, y, 0, 0);
        }
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }

    case 0xAEDE7346: { // cmd_auto_move
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(auto_move_t))) {
          const auto_move_t* autoMove = reinterpret_cast<const auto_move_t*>(data + sizeof(cmd_head_t));
          int32_t x = ntohl(autoMove->x);
          int32_t y = ntohl(autoMove->y);
          uint32_t duration = ntohl(autoMove->duration);
          startAutoMouseMove(x, y, duration);
        }
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }

    // -------------------------------
    // Mouse Button Commands
    // -------------------------------
    case 0x9823AE8D: { // cmd_mouse_left
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_button_t))) {
          const mouse_button_t* btn = reinterpret_cast<const mouse_button_t*>(data + sizeof(cmd_head_t));
          Mouse.set_buttons(btn->state, Mouse.isPressed(MOUSE_RIGHT), Mouse.isPressed(MOUSE_MIDDLE));
        }
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }

    case 0x97A3AE8D: { // cmd_mouse_middle
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_button_t))) {
          const mouse_button_t* btn = reinterpret_cast<const mouse_button_t*>(data + sizeof(cmd_head_t));
          Mouse.set_buttons(Mouse.isPressed(MOUSE_LEFT), Mouse.isPressed(MOUSE_RIGHT), btn->state);
        }
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }

    case 0x238D8212: { // cmd_mouse_right
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_button_t))) {
          const mouse_button_t* btn = reinterpret_cast<const mouse_button_t*>(data + sizeof(cmd_head_t));
          Mouse.set_buttons(Mouse.isPressed(MOUSE_LEFT), btn->state, Mouse.isPressed(MOUSE_MIDDLE));
        }
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }

    // -------------------------------
    // Wheel Command
    // -------------------------------
    case 0xFFEEAD38: { // cmd_mouse_wheel
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_wheel_t))) {
          const mouse_wheel_t* wheel = reinterpret_cast<const mouse_wheel_t*>(data + sizeof(cmd_head_t));
          int32_t w = ntohl(wheel->value);
          Mouse.move(0, 0, w);
        }
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }

    // -------------------------------
    // Bezier Curve Command
    // -------------------------------
    case 0x5A4538A2: { // cmd_bazerMove
        if (size >= (int)(sizeof(cmd_head_t) + sizeof(bezier_move_t))) {
          const bezier_move_t* bezier = reinterpret_cast<const bezier_move_t*>(data + sizeof(cmd_head_t));
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

    // -------------------------------
    // Monitoring & Configuration
    // -------------------------------
    case 0x27388020: { // cmd_monitor
        handleMonitorCommand(data, size, remoteIP, remotePort);
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }

    case 0x23234343: { // cmd_mask_mouse
        handleMaskCommand(data, size);
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }

    case 0x23344343: // cmd_unmask_all
      maskLeftButton = maskRightButton = maskMiddleButton = maskMovement = false;
      sendAckResponse(header, remoteIP, remotePort);
      break;

    case 0x12334883: { // cmd_showpic (after ntohl())
        handleLCDImageCommand(data, size);
        sendAckResponse(header, remoteIP, remotePort);
        break;
      }

    // -------------------------------
    // System Commands
    // -------------------------------
    case 0xAA8855AA: // cmd_reboot
      sendAckResponse(header, remoteIP, remotePort);
      _softRestart();
      break;
    default:
#if NETWORK_DEBUG_MODE
#if EXTERNAL_SERIAL
      Serial8.printf("Unknown command: 0x%08X\n", cmd);
#else
      Serial.printf("Unknown command: 0x%08X\n", cmd);
#endif
#endif
      break;
  }
}

//------------------------------
// Handle connection requests
//------------------------------
void handleConnect(const cmd_head_t* header, IPAddress remoteIP, uint16_t remotePort) {
  char receivedUUID[9];
  snprintf(receivedUUID, sizeof(receivedUUID), "%08lX", ntohl(header->mac));

  if (strncmp(receivedUUID, deviceUUID, 8) == 0) {
    monitorIP = remoteIP;
    monitorPort = remotePort;
    sendAckResponse(header, remoteIP, remotePort);
#if NETWORK_DEBUG_MODE
#if EXTERNAL_SERIAL
    Serial8.println("Valid client connected");
#else
    Serial.println("Valid client connected");
#endif
#endif
  } else {
    sendAckResponse(header, remoteIP, remotePort);
#if NETWORK_DEBUG_MODE
#if EXTERNAL_SERIAL
    Serial8.println("Invalid client MAC");
#else
    Serial.println("Invalid client MAC");
#endif
#endif
  }
}

//------------------------------
// Echo original request
//------------------------------
void sendAckResponse(const cmd_head_t* header, IPAddress remoteIP, uint16_t remotePort) {
  cmd_head_t resp;
  resp.mac = header->mac;
  resp.rand = 0;
  resp.indexpts = transactionIndex++;
  resp.cmd = header->cmd; // Echo original command
  Udp.beginPacket(remoteIP, remotePort);
  Udp.write((uint8_t*)&resp, sizeof(resp));
  Udp.endPacket();
}

//------------------------------
// Handle monitor requests
//------------------------------
void handleMonitorCommand(const uint8_t* data, int size, IPAddress remoteIP, uint16_t remotePort) {
  if (size < (int)(sizeof(cmd_head_t) + sizeof(monitor_cmd_t))) {
#if NETWORK_DEBUG_MODE
#if EXTERNAL_SERIAL
    Serial8.println("Invalid monitor command: packet too small");
#else
    Serial.println("Invalid monitor command: packet too small");
#endif
#endif
    return;
  }
  const monitor_cmd_t* monitor_cmd = reinterpret_cast<const monitor_cmd_t*>(data + sizeof(cmd_head_t));
  // Network byte order conversion
  monitorPort = ntohs(monitor_cmd->monitor_port);
  monitorIP = remoteIP; // Use sender's IP address
  monitorEnabled = true;

#if NETWORK_DEBUG_MODE
#if EXTERNAL_SERIAL
  Serial8.printf("Monitoring enabled: %d.%d.%d.%d:%d\n",
                 monitorIP[0], monitorIP[1],
                 monitorIP[2], monitorIP[3],
                 monitorPort);
#else
  Serial.printf("Monitoring enabled: %d.%d.%d.%d:%d\n",
                monitorIP[0], monitorIP[1],
                monitorIP[2], monitorIP[3],
                monitorPort);
#endif
#endif
}

void handleMaskCommand(const uint8_t* data, int size) {
  if (size < (int)(sizeof(cmd_head_t) + sizeof(mask_cmd_t))) {
#if NETWORK_DEBUG_MODE
#if EXTERNAL_SERIAL
    Serial8.println("Invalid mask command: packet too small");
#else
    Serial.println("Invalid mask command: packet too small");
#endif
#endif
    return;
  }

  const mask_cmd_t* mask_cmd = reinterpret_cast<const mask_cmd_t*>(data + sizeof(cmd_head_t));

  // Convert network byte order to host and interpret as booleans
  maskLeftButton = ntohl(mask_cmd->mask_left) != 0;
  maskRightButton = ntohl(mask_cmd->mask_right) != 0;
  maskMiddleButton = ntohl(mask_cmd->mask_middle) != 0;
  maskMovement = ntohl(mask_cmd->mask_movement) != 0;

#if NETWORK_DEBUG_MODE
#if EXTERNAL_SERIAL
  Serial8.printf("Mouse masks: L=%d R=%d M=%d Move=%d\n",
                 maskLeftButton, maskRightButton,
                 maskMiddleButton, maskMovement);
#else
  Serial.printf("Mouse masks: L=%d R=%d M=%d Move=%d\n",
                maskLeftButton, maskRightButton,
                maskMiddleButton, maskMovement);
#endif
#endif
}

// -------------------------------------------------------------
// setup()
// -------------------------------------------------------------
void setup() {
#if ENABLE_WATCHDOG
  watchdog.enable(Watchdog::TIMEOUT_8S);
#endif
#if EXTERNAL_SERIAL
  Serial8.begin(115200);
#else
  Serial.begin(115200);
#endif

  tft.begin();
  tft.fillScreen(ILI9341_BLUE);
  tft.setTextWrap(false);
  tft.setRotation(1);

  ts.begin();

  if (!ts.begin()) {
#if EXTERNAL_SERIAL
  Serial8.println("Touchscreen failed");
#else
  Serial.println("Touchscreen failed");
#endif
  } else {
    #if EXTERNAL_SERIAL
  Serial8.println("Touchscreen started");
#else
  Serial.println("Touchscreen started");
#endif
  }
  
  myusb.begin();
#if EXTERNAL_SERIAL
  Serial8.println("USB Host started.");
#else
  Serial.println("USB Host started.");
#endif

  writeDisplay("Mouse:", "Disconnected", 0);
  // Initialize Ethernet
  ethernetInitialized = initializeEthernet();
  if (ethernetInitialized) {
    ethernetConnected = true;
    Udp.begin(localPort);

#if NETWORK_DEBUG_MODE
#if EXTERNAL_SERIAL
    Serial8.print("UDP listening on port ");
    Serial8.println(localPort);
#else
    Serial.print("UDP listening on port ");
    Serial.println(localPort);
#endif
#endif
  }

  generateHardwareUUID();
  writeDisplay("UUID", deviceUUID, 2);

#if EXTERNAL_SERIAL
  Serial8.printf("Device UUID: %s\n", deviceUUID);
#else
  Serial.printf("Device UUID: %s\n", deviceUUID);
#endif

  // Start usb device
  Mouse.begin();
#if EXTERNAL_SERIAL
  Serial8.println("Mouse initialized.");
#else
  Serial.println("Mouse initialized.");
#endif

  writeDisplay("Service", "Offline", 3);
#if EXTERNAL_SERIAL
  Serial8.println("Setup complete.");
#else
  Serial.println("Setup complete.");
#endif
  pinMode(LED_BUILTIN, OUTPUT);
}
// -------------------------------------------------------------
// 17) loop()
// -------------------------------------------------------------
void loop() {
  // Keep USB Host tasks updated
  myusb.Task();
  handleMouse();
  updateAutoMouseMove();
  updateMouseMovementState();

  // Drain stored mouse packets every 10 ms if DEBUG_MODE is set
  static uint32_t lastMousePacketCheck = 0;
  uint32_t now = millis();
  if (now - lastMousePacketCheck >= 10) {
    lastMousePacketCheck += 10;
#if DEBUG_MODE
    MousePacket p;
    while (getNextMousePacket(p)) {
      uint32_t latency = now - p.millis_stamp;
#if EXTERNAL_SERIAL
      Serial8.printf("Captured host mouse => dx=%d dy=%d wheel=%d hwheel=%d btn=0x%02X (Latency: %u ms)\n",
                     p.dx, p.dy, p.wheel, p.hwheel, p.buttons, latency);
#else
      Serial.printf("Captured host mouse => dx=%d dy=%d wheel=%d hwheel=%d btn=0x%02X (Latency: %u ms)\n",
                    p.dx, p.dy, p.wheel, p.hwheel, p.buttons, latency);
#endif
    }
#else
    // If not debugging, we still empty the buffer:
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
        ethernetConnected = false;
      }
    } else {
      if (!ethernetConnected) {
        writeDisplay("Ethernet:", "Cable On", 1);
        ethernetConnected = true;
      }
    }
  }

  if (ethernetInitialized && ethernetConnected) {
    // Read packets into ring buffer
    int packetSize;
    while ((packetSize = Udp.parsePacket()) > 0) {
#if NETWORK_DEBUG_MODE
#if EXTERNAL_SERIAL
      Serial8.print("Got a packet, size=");
      Serial8.println(packetSize);
#else
      Serial.print("Got a packet, size=");
      Serial.println(packetSize);
#endif
#endif
      if ((head + 1) % N_NET_PACKETS != tail) {
        ethernetBuffer[head].remoteIP = Udp.remoteIP();
        ethernetBuffer[head].remotePort = Udp.remotePort();
        ethernetBuffer[head].size = Udp.read(ethernetBuffer[head].data, 1500);
        head = (head + 1) % N_NET_PACKETS;
      }
    }
    // Process packets from ring buffer
    while (tail != head) {
      processPacket(
        ethernetBuffer[tail].data,
        ethernetBuffer[tail].size,
        ethernetBuffer[tail].remoteIP,
        ethernetBuffer[tail].remotePort
      );
      tail = (tail + 1) % N_NET_PACKETS;
    }
  } else {
    // If Ethernet is not initialized or cable is off, try to re-init every 5s
    static uint32_t lastEthernetCheck = 0;
    if (now - lastEthernetCheck > 5000) {
      lastEthernetCheck = now;
      if (!ethernetInitialized) {
        writeDisplay("Ethernet: ", "Trying..", 1);
        ethernetInitialized = initializeEthernet();
        if (ethernetInitialized) {
          ethernetConnected = true;
          Udp.begin(localPort);
        }
      }
    }
  }

  // Heartbeat LED every 1s
  static uint32_t lastHeartbeat = 0;
  if (now - lastHeartbeat > 500) {
    lastHeartbeat += 500;
    static bool ledState = false;
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
  }
  boolean istouched = ts.touched();
  if (istouched) {
    if (overlayEnabled) { // Disable udp command parsing
#if EXTERNAL_SERIAL
      Serial8.println("Overlay disabled");
#else
      Serial.println("Overlay disabled");
#endif
      overlayEnabled = false;
      writeDisplay("Service", "Disabled", 3);
    } else { // reenable udp command parsing
#if EXTERNAL_SERIAL
      Serial8.println("Overlay enabled");
#else
      Serial.println("Overlay enabled"); x
#endif
      overlayEnabled = true;
      writeDisplay("Service", "Enabled", 3);
    }
  }
#if ENABLE_WATCHDOG
  // Watchdog handling
  if (((time - enabled_time) <= TIMEOUT_DURATION) && ((now - reset_time) >= RESET_DURATION)) {
    reset_time = now;
    watchdog.reset();
  }
#endif
}
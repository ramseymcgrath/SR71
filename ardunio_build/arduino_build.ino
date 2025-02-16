// -------------------------------------------------------------
// Whole bunch of includes (teensy mainly)
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
void doxygenBanner( int theory );

// -------------------------------------------------------------
// Arguments for display
// -------------------------------------------------------------
#define TFT_MISO 12
#define TFT_SCK 13
#define TFT_MOSI 11
#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 255
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCK, TFT_MISO);

#define FRAME_X 210
#define FRAME_Y 180
#define FRAME_W 100
#define FRAME_H 50

// Display layout constants
#define STATUS_X 10
#define STATUS_Y_START 10
#define STATUS_LINE_HEIGHT 20 // Adjust based on font height
#define STATUS_MAX_LINES 4
#define STATUS_WIDTH 200

// -------------------------------------------------------------
// USB Host Setup
// -------------------------------------------------------------
USBHost myusb;            // The USB host object
USBHIDParser hid1(myusb); // HID parser
MouseController mouse1(myusb);

// -------------------------------------------------------------
// Watchdog Setup
// -------------------------------------------------------------
Watchdog watchdog;

// -------------------------------------------------------------
// Debug Modes
// DEBUG_MODE is general messages, NETWORK_DEBUG is more network'y
// -------------------------------------------------------------
#define DEBUG_MODE 0
#define NETWORK_DEBUG_MODE 0

// -----------------------------
// Feature flags (TODO: add some to tft display)
// -----------------------------
#define SMOOTH_MOUSE_MOVEMENT_OVERRIDE_EXISTING 1 // override existing movements when smoothing or add to end of buffer
#define EXTERNAL_SERIAL 1                         // allows debugging without a main pc com port
#define ENABLE_WATCHDOG 0                         // gracefully handle crashes
#define MAX_MOVE_SPEED 3                          // max packet output from device (make this something reasonable)
#define MOUSE_MOVE_INTERVAL 1                     // set as low as possible, this is number of times the mouse position updates in chip
#define WAIT_FOR_SERIAL 0                         // wait for serial before setting up

// -------------------------------------------------------------
// Mouse Packet Ring Buffer (for storing host mouse events)
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

// Place the ring buffer in DMAMEM for faster access
DMAMEM __attribute__((aligned(32))) static MousePacket mouse_ring[N_MOUSE_PACKETS];
static volatile uint8_t mouse_ring_head = 0;
static volatile uint8_t mouse_ring_tail = 0;
void processPacket(const uint8_t *data, int size, IPAddress remoteIP, uint16_t remotePort);

// -------------------------------------------------------------
// Ethernet / UDP Setup
// -------------------------------------------------------------
byte mac[] = {0x10, 0xA0, 0x69, 0x42, 0x00, 0x00}; // Can be whatever
IPAddress ip(192, 168, 1, 177);                    // fallback if DHCP fails
unsigned int localPort = 8888;                     // local UDP port

EthernetUDP Udp;

// -------------------------------------------------------------
// Global flags and state variables
// -------------------------------------------------------------
const unsigned long TIMEOUT_DURATION = 60000;
const unsigned long RESET_DURATION = 3000;
unsigned long lastActivityTime;
unsigned long reset_time;
bool ethernetInitialized = false;
bool ethernetConnected = false;
bool mouseConnected = false;
static uint8_t mouse_buttons_prev = 0;
static bool monitorEnabled = false;
static IPAddress monitorIP;
static unsigned int monitorPort = 0;
static bool maskLeftButton = false;
static bool maskRightButton = false;
static bool maskMiddleButton = false;
static bool maskMovement = false;
static uint32_t transactionIndex = 0;
#define UDP_BUFFER_SIZE 1500 // Standard Ethernet MTU
static const uint32_t MOVE_INTERVAL_MS = MOUSE_MOVE_INTERVAL;
bool overlayEnabled = true;

inline int16_t ntohs(int16_t val)
{
  return (val << 8) | ((val >> 8) & 0xFF);
}

inline int32_t ntohl(int32_t val)
{
  return ((val >> 24) & 0x000000FF) |
         ((val >> 8) & 0x0000FF00) |
         ((val << 8) & 0x00FF0000) |
         ((val << 24) & 0xFF000000);
}

// ------------------
// Network ring buffer
// -----------------
#define N_NET_PACKETS 60 // raise this to support more network traffic if needed
struct Packet
{
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
void sendMonitorEvent(const char *eventString);

// -------------------------------------------------------------
// Utility: Soft Restart
// -------------------------------------------------------------
void _softRestart()
{
  _endSerial();
  SCB_AIRCR = 0x05FA0004; // Write value for restart
}

// ------------------------------------------------------------
// Utility: Write to Serial (useful for debugging)
// ------------------------------------------------------------
void _writeSerial(const char *message)
{
#if EXTERNAL_SERIAL
  Serial8.println(message);
#else
  Serial.println(message);
#endif
}

void _beginSerial()
{
#if EXTERNAL_SERIAL
  Serial8.begin(115200);
#else
  Serial.begin();
#endif
#if WAIT_FOR_SERIAL
#if EXTERNAL_SERIAL
  while (!Serial8)
  {
#else
  while (!Serial)
  {
#endif
  }
#endif
}

void _endSerial()
{
#if EXTERNAL_SERIAL
  Serial8.end();
#else
  Serial.end();
#endif
}

// -------------------------------------------------------------
// Ethernet Initialization
// -------------------------------------------------------------
bool initializeEthernet()
{
  if (Ethernet.hardwareStatus() == EthernetNoHardware)
  {
    _writeSerial("Ethernet: No HW");
    writeDisplay("Ethernet:", "No HW", 1);
    return false;
  }

  // Attempt DHCP with 10s timeout
  if (Ethernet.begin(mac, 10000) == 0)
  {
    _writeSerial("Ethernet: No DHCP");
    writeDisplay("Ethernet:", "No DHCP", 1);
    Ethernet.begin(mac, ip);
  }

  if (Ethernet.linkStatus() == LinkOFF)
  {
    _writeSerial("Ethernet: Cable Off");
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
  _writeSerial("Ethernet connected");
  return true;
}

// -------------------------------------------------------------
// Hardware UUID Generation (10-character)
// -------------------------------------------------------------
char deviceUUID[9] = {0}; // 8 chars + null terminator

/**
 * @brief Generates a unique hardware UUID for the device.
 *
 * This function reads the unique hardware ID from the Teensy's hardware
 * configuration register and formats it as an 8-character uppercase hexadecimal
 * string. The generated UUID is stored in the global variable `deviceUUID`.
 *
 * @note The hardware ID is read from the first 4 bytes of the HW_OCOTP_CFG0 register.
 */
void generateHardwareUUID()
{
  // Read Teensy's unique hardware ID (4 bytes)
  uint32_t uid = HW_OCOTP_CFG0; // Use first 4 bytes
  snprintf(deviceUUID, sizeof(deviceUUID),
           "%08X", // 8-character uppercase hex
           (unsigned int)uid);
}

// -------------------------------------------------------------
// TFT Display
// -------------------------------------------------------------
/**
 * @brief Writes a message to a specific section of the display.
 *
 * This function clears a section of the display and writes a header and message to it.
 * The display is divided into sections, and the section to write to is specified by the
 * `section` parameter.
 *
 * @param header The header text to display. If NULL or empty, no header is displayed.
 * @param message The message text to display. If NULL, no message is displayed.
 * @param section The section of the display to write to. Must be between 0 and 3 inclusive.
 *
 * @note The function does nothing if the `section` parameter is out of range.
 * @note The text is displayed with a black color on a white background.
 * @note If DEBUG_MODE is enabled, debug information is printed to the serial output.
 */
void writeDisplay(const char *header, const char *message, int section)
{
  if (section < 0 || section >= 4)
    return;

  // Set text size as needed
  tft.setTextSize(1);

  // Calculate the y position for this section based on predefined constants
  int yPos = STATUS_Y_START + (section * STATUS_LINE_HEIGHT);

  // Clear the section with a white background
  tft.fillRect(STATUS_X, yPos, STATUS_WIDTH, STATUS_LINE_HEIGHT, ILI9341_WHITE);

  // Set cursor position and set text color to black (with white background)
  tft.setCursor(STATUS_X, yPos);
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);

  // Print header (if provided) followed by a colon and space
  if (header && strlen(header) > 0)
  {
    tft.print(header);
    tft.print(": ");
  }

  // Print the message (if provided)
  if (message)
  {
    tft.print(message);
  }

#if DEBUG_MODE
  // Optionally, print debug info to serial output
  char debugStr[128];
  snprintf(debugStr, sizeof(debugStr), "[Display%d] %s%s", section, header ? header : "", message ? message : "");
  _writeSerial(debugStr);
#endif
}

// -------------------------------------------------------------
// Storing and Retrieving Mouse Packets
// -------------------------------------------------------------
static inline void storeMousePacket(int dx, int dy, int wheel, int hwheel, uint8_t btns)
{
  uint8_t next = (mouse_ring_head + 1) % N_MOUSE_PACKETS;
  if (next == mouse_ring_tail)
  {
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

static inline bool getNextMousePacket(MousePacket &pkt)
{
  if (mouse_ring_tail == mouse_ring_head)
  {
    return false; // empty
  }
  pkt = mouse_ring[mouse_ring_tail];
  mouse_ring_tail = (mouse_ring_tail + 1) % N_MOUSE_PACKETS;
  return true;
}

// -------------------------------------------------------------
// Auto Mouse Move Helper
// -------------------------------------------------------------
struct AutoMoveState
{
  bool active = false;
  elapsedMillis timer;
  int totalSteps = 0;
  int stepX = 0;
  int stepY = 0;
  int remainingX = 0;
  int remainingY = 0;
};

static AutoMoveState autoMove;

/**
 * @brief Starts an automated mouse movement over a specified duration.
 *
 * This function initializes the parameters required to move the mouse cursor
 * from its current position to a target position (x, y) over a given duration
 * in milliseconds. The movement is broken down into steps to achieve a smooth
 * transition.
 *
 * @param x The target x-coordinate to move the mouse to.
 * @param y The target y-coordinate to move the mouse to.
 * @param durationMs The duration over which the mouse should move, in milliseconds.
 *                   If the duration is less than or equal to 0, it defaults to 10 milliseconds.
 */
void startAutoMouseMove(int x, int y, int durationMs)
{
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

/**
 * @brief Updates the automatic mouse movement.
 *
 * This function checks if the automatic mouse movement is active. If it is,
 * it processes the mouse movement every 10 timer ticks. The function updates
 * the mouse position by the specified step values and decreases the remaining
 * distance to move. When the remaining distance in both X and Y directions
 * reaches zero, the automatic mouse movement is deactivated.
 */
void updateAutoMouseMove()
{
  if (!autoMove.active)
    return;
  if (autoMove.timer >= 10)
  {
    autoMove.timer = 0;
    processMouseMove(autoMove.stepX, autoMove.stepY, 0, 0);
    autoMove.remainingX -= autoMove.stepX;
    autoMove.remainingY -= autoMove.stepY;
    if (autoMove.remainingX == 0 && autoMove.remainingY == 0)
    {
      autoMove.active = false;
    }
  }
}

// -------------------------------------------------------------
// Send Monitor Event (if monitorEnabled is true)
// -------------------------------------------------------------
/**
 * @brief Sends a monitor event to a specified IP and port if monitoring is enabled.
 *
 * This function sends a given event string to a monitoring server using UDP.
 * It first checks if monitoring is enabled, and if so, it sends the event string
 * to the configured monitor IP and port.
 *
 * @param eventString The event string to be sent to the monitoring server.
 */
void sendMonitorEvent(const char *eventString)
{
  if (!monitorEnabled)
    return;
  Udp.beginPacket(monitorIP, monitorPort);
  Udp.write(eventString);
  Udp.endPacket();
}

// -------------------------------------------------------------
// handleMouse(): Reads host mouse (via USB Host) and
//     forwards to device-side "Mouse" if not masked.
// -------------------------------------------------------------
/**
 * @brief Handles mouse input and updates the system state accordingly.
 *
 * This function performs the following tasks:
 * - Connects the mouse if it is not already connected.
 * - Retrieves mouse movement (dx, dy), wheel movement (wheel, hwheel), and button states.
 * - Masks button states if needed.
 * - Stores the mouse packet for debugging or inspection.
 * - Sends monitor events if the button state has changed.
 * - Moves the mouse cursor if movement is not masked and there is a change in movement.
 * - Updates the device-side button state if it has changed.
 * - Clears the mouse data buffer.
 *
 * @note This function assumes the existence of several global variables and functions:
 * - mouseConnected: A boolean indicating if the mouse is connected.
 * - writeDisplay: A function to write messages to a display.
 * - mouse1: An object representing the mouse, with methods getMouseX(), getMouseY(), getWheel(), getWheelH(), getButtons(), and mouseDataClear().
 * - maskLeftButton, maskRightButton, maskMiddleButton: Booleans indicating if the respective buttons should be masked.
 * - storeMousePacket: A function to store the mouse packet data.
 * - sendMonitorEvent: A function to send monitor events.
 * - maskMovement: A boolean indicating if mouse movement should be masked.
 * - Mouse: An object representing the system mouse, with methods move() and set_buttons().
 * - mouse_buttons_prev: A variable storing the previous button state.
 */
void handleMouse()
{
  if (!mouseConnected)
  {
    mouseConnected = true;
    writeDisplay("Mouse:", "Connected", 0);
  }
  int dx = mouse1.getMouseX();
  int dy = mouse1.getMouseY();
  int wheel = mouse1.getWheel();
  int hwheel = mouse1.getWheelH();
  uint8_t btns = mouse1.getButtons();

  // Mask buttons if needed
  if (maskLeftButton)
    btns &= ~0x01; // Left (bit 0)
  if (maskRightButton)
    btns &= ~0x02; // Right (bit 1)
  if (maskMiddleButton)
    btns &= ~0x04; // Middle (bit 2)

  // Store the packet (for debugging or inspection)
  storeMousePacket(dx, dy, wheel, hwheel, btns);

  // Possibly send monitor events if button state changed
  static uint8_t oldButtons = 0;
  uint8_t changed = btns ^ oldButtons;
  if (changed & 0x01)
  {
    if (btns & 0x01)
      sendMonitorEvent("MOUSE_LEFT_PRESS");
    else
      sendMonitorEvent("MOUSE_LEFT_RELEASE");
  }
  if (changed & 0x02)
  {
    if (btns & 0x02)
      sendMonitorEvent("MOUSE_MIDDLE_PRESS");
    else
      sendMonitorEvent("MOUSE_MIDDLE_RELEASE");
  }
  if (changed & 0x04)
  {
    if (btns & 0x04)
      sendMonitorEvent("MOUSE_RIGHT_PRESS");
    else
      sendMonitorEvent("MOUSE_RIGHT_RELEASE");
  }
  oldButtons = btns;

  // Move only if not masked
  if (!maskMovement)
  {
    // Only send new movement if it changed
    static int prevDx = 0, prevDy = 0, prevWheel = 0, prevHwheel = 0;
    if (dx != prevDx || dy != prevDy || wheel != prevWheel || hwheel != prevHwheel)
    {
      Mouse.move(dx, dy, wheel, hwheel);
      prevDx = dx;
      prevDy = dy;
      prevWheel = wheel;
      prevHwheel = hwheel;
    }
  }
  // Update device-side button state if changed
  if (btns != mouse_buttons_prev)
  {
    Mouse.set_buttons((btns & 1),   // left
                      (btns & 2),   // right
                      (btns & 4),   // middle
                      (btns & 8),   // back
                      (btns & 16)); // forward
    mouse_buttons_prev = btns;
  }
  mouse1.mouseDataClear();
}

// State for non-blocking mouse movement
struct MouseMoveState
{
  float remainingX; // Track partial pixel movements
  float remainingY;
  int16_t remainingWheel;
  int16_t remainingHWheel;
  bool active;
  uint32_t lastMoveTime;
};

static MouseMoveState mouseMoveState = {0, 0, 0, 0, false, 0};

void processMouseMove(float x, float y, int16_t wheel, int16_t hwheel)
{
  // Smooth mouse movement can be overridden to replace existing movement or
  // added to existing movement.
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

void updateMouseMovementState()
{
  if (!mouseMoveState.active)
    return;

  uint32_t now = millis();
  if (now - mouseMoveState.lastMoveTime >= MOVE_INTERVAL_MS)
  {
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
  else
  {
    if (mouseMoveState.remainingWheel != 0 || mouseMoveState.remainingHWheel != 0)
    {
      if (now - mouseMoveState.lastMoveTime >= MOVE_INTERVAL_MS)
      {
        // Move wheel/hwheel exactly once
        Mouse.move(0, 0, mouseMoveState.remainingWheel, mouseMoveState.remainingHWheel);
        // Clear them out
        mouseMoveState.remainingWheel = 0;
        mouseMoveState.remainingHWheel = 0;
        mouseMoveState.lastMoveTime = now;
      }
    }
    else
    {
      // Everything done
      mouseMoveState.active = false;
    }
  }
}

// -------------------------------------------------------------
// Bézier Curve Mouse Motion (Non-Blocking)
// -------------------------------------------------------------
struct BezierState
{
  elapsedMillis timer;
  float t = 0.0;
  int startX = 0, startY = 0;
  bool active = false;
};

BezierState bezierState;

// Simulate mouse movement along a Bézier curve
void simulateBezierMouseMove(int targetX, int targetY, int durationMs, int cx1, int cy1, int cx2, int cy2)
{
  if (!bezierState.active)
  {
    bezierState = {0, 0.0, 0, 0, true}; // Reset state
  }

  if (bezierState.timer >= 10 && bezierState.t <= 1.0f)
  {
    bezierState.timer = 0;
    float u = 1.0f - bezierState.t;
    float x = 3 * u * u * bezierState.t * cx1 + 3 * u * bezierState.t * bezierState.t * cx2 + bezierState.t * bezierState.t * bezierState.t * targetX;
    float y = 3 * u * u * bezierState.t * cy1 + 3 * u * bezierState.t * bezierState.t * cy2 + bezierState.t * bezierState.t * bezierState.t * targetY;
    processMouseMove((int)x, (int)y, 0, 0);
    bezierState.t += (10.0f / durationMs);
  }
  else if (bezierState.t > 1.0f)
  {
    bezierState.active = false;
    bezierState.t = 1.0f; // Clamp to 1.0
  }
}

// -------------------------------------------------------------
// Handle LCD Image Command (not implemented)
// -------------------------------------------------------------
void handleLCDImageCommand(const uint8_t *data, int size)
{
  // Validate packet structure
  if (size < (int)sizeof(cmd_head_t) + 1024)
  { // 16-byte header + 1024 image data
#if NETWORK_DEBUG_MODE
    _writeSerial("ERROR: Invalid LCD image command");
#endif
    return;
  }

  // Extract image chunk
  const uint8_t *imageData = data + sizeof(cmd_head_t);
  int chunkIndex = ntohl(*(const uint32_t *)(imageData)); // First 4 bytes: chunk index

  // Write to display buffer //TODO
  // display.drawBitmap(0, 0, imageData + 4, 128, 64, SSD1306_WHITE);
  // display.display();

#if NETWORK_DEBUG_MODE
  char debugStr[64];
  snprintf(debugStr, sizeof(debugStr), "LCD Image Chunk: %d", chunkIndex);
  _writeSerial(debugStr);
#endif
}

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

// ------ network handling
/**
 * @brief Processes an incoming network packet and executes the corresponding command.
 *
 * @param data Pointer to the packet data.
 * @param size Size of the packet data.
 * @param remoteIP IP address of the remote sender.
 * @param remotePort Port number of the remote sender.
 *
 * This function handles various commands received in the packet data. It first checks if the packet size is valid.
 * Then, it extracts the command and client MAC address from the packet header. Depending on the command, it performs
 * different actions such as handling mouse movements, button presses, wheel movements, bezier curve movements,
 * monitoring, masking/unmasking mouse actions, showing pictures, and system reboot.
 *
 * Supported commands:
 * - CMD_CONNECT: Establishes a connection.
 * - CMD_MOUSE_MOVE: Processes mouse movement.
 * - CMD_AUTO_MOVE: Starts automatic mouse movement.
 * - CMD_MOUSE_LEFT: Handles left mouse button press.
 * - CMD_MOUSE_MIDDLE: Handles middle mouse button press.
 * - CMD_MOUSE_RIGHT: Handles right mouse button press.
 * - CMD_MOUSE_WHEEL: Handles mouse wheel movement.
 * - CMD_BEZIER_MOVE: Simulates bezier curve mouse movement.
 * - CMD_MONITOR: Handles monitoring command.
 * - CMD_MASK_MOUSE: Masks mouse actions.
 * - CMD_UNMASK_ALL: Unmasks all mouse actions.
 * - CMD_SHOW_PIC: Displays an image on the LCD.
 * - CMD_REBOOT: Reboots the system.
 *
 * If the command is unknown, it logs an error message in debug mode.
 */
void processPacket(const uint8_t *data, int size, IPAddress remoteIP, uint16_t remotePort)
{
  if (size < (int)(sizeof(cmd_head_t)))
  {
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
  snprintf(debugStr, sizeof(debugStr), "CMD: 0x%08X MAC: 0x%08X", (unsigned int)cmd, (unsigned int)clientMac);
  _writeSerial(debugStr);
#endif
  if (!overlayEnabled)
  {
    return;
  }
  switch (cmd)
  {
  // -------------------------------
  // Connection Command
  // -------------------------------
  case CMD_CONNECT: // cmd_connect
    handleConnect(header, remoteIP, remotePort);
    break;

  // -------------------------------
  // Mouse Movement Commands
  // -------------------------------
  case CMD_MOUSE_MOVE:
  { // cmd_mouse_move
    if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_move_t)))
    {
      const mouse_move_t *move = reinterpret_cast<const mouse_move_t *>(data + sizeof(cmd_head_t));
      int32_t x = ntohl(move->x);
      int32_t y = ntohl(move->y);
      processMouseMove(x, y, 0, 0);
    }
    sendAckResponse(header, remoteIP, remotePort);
    break;
  }

  case CMD_AUTO_MOVE:
  { // cmd_auto_move
    if (size >= (int)(sizeof(cmd_head_t) + sizeof(auto_move_t)))
    {
      const auto_move_t *autoMove = reinterpret_cast<const auto_move_t *>(data + sizeof(cmd_head_t));
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
  case CMD_MOUSE_LEFT:
  { // cmd_mouse_left
    if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_button_t)))
    {
      const mouse_button_t *btn = reinterpret_cast<const mouse_button_t *>(data + sizeof(cmd_head_t));
      Mouse.set_buttons(btn->state, Mouse.isPressed(MOUSE_RIGHT), Mouse.isPressed(MOUSE_MIDDLE));
    }
    sendAckResponse(header, remoteIP, remotePort);
    break;
  }

  case CMD_MOUSE_MIDDLE:
  { // cmd_mouse_middle
    if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_button_t)))
    {
      const mouse_button_t *btn = reinterpret_cast<const mouse_button_t *>(data + sizeof(cmd_head_t));
      Mouse.set_buttons(Mouse.isPressed(MOUSE_LEFT), Mouse.isPressed(MOUSE_RIGHT), btn->state);
    }
    sendAckResponse(header, remoteIP, remotePort);
    break;
  }

  case CMD_MOUSE_RIGHT:
  { // cmd_mouse_right
    if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_button_t)))
    {
      const mouse_button_t *btn = reinterpret_cast<const mouse_button_t *>(data + sizeof(cmd_head_t));
      Mouse.set_buttons(Mouse.isPressed(MOUSE_LEFT), btn->state, Mouse.isPressed(MOUSE_MIDDLE));
    }
    sendAckResponse(header, remoteIP, remotePort);
    break;
  }

  // -------------------------------
  // Wheel Command
  // -------------------------------
  case CMD_MOUSE_WHEEL:
  { // cmd_mouse_wheel
    if (size >= (int)(sizeof(cmd_head_t) + sizeof(mouse_wheel_t)))
    {
      const mouse_wheel_t *wheel = reinterpret_cast<const mouse_wheel_t *>(data + sizeof(cmd_head_t));
      int32_t w = ntohl(wheel->value);
      Mouse.move(0, 0, w);
    }
    sendAckResponse(header, remoteIP, remotePort);
    break;
  }

  // -------------------------------
  // Bezier Curve Command
  // -------------------------------
  case CMD_BEZIER_MOVE:
  { // cmd_bazerMove
    if (size >= (int)(sizeof(cmd_head_t) + sizeof(bezier_move_t)))
    {
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

  // -------------------------------
  // Monitoring & Configuration
  // -------------------------------
  case CMD_MONITOR:
  { // cmd_monitor
    handleMonitorCommand(data, size, remoteIP, remotePort);
    sendAckResponse(header, remoteIP, remotePort);
    break;
  }

  case CMD_MASK_MOUSE:
  { // cmd_mask_mouse
    handleMaskCommand(data, size);
    sendAckResponse(header, remoteIP, remotePort);
    break;
  }

  case CMD_UNMASK_ALL: // cmd_unmask_all
    maskLeftButton = maskRightButton = maskMiddleButton = maskMovement = false;
    sendAckResponse(header, remoteIP, remotePort);
    break;

  case CMD_SHOW_PIC:
  { // cmd_showpic (after ntohl())
    handleLCDImageCommand(data, size);
    sendAckResponse(header, remoteIP, remotePort);
    break;
  }

  // -------------------------------
  // System Commands
  // -------------------------------
  case CMD_REBOOT: // cmd_reboot
    sendAckResponse(header, remoteIP, remotePort);
    _softRestart();
    break;

  default:
#if NETWORK_DEBUG_MODE
    char debugStr[64];
    snprintf(debugStr, sizeof(debugStr), "Unknown command: 0x%08X", (unsigned int)cmd);
    _writeSerial(debugStr);
#endif
    break;
  }
}

//------------------------------
// Handle connection requests
//------------------------------
/**
 * @brief Handles an incoming connection request.
 *
 * This function processes a connection request by comparing the received UUID
 * with the device's UUID. If they match, it sets the monitor IP and port and
 * sends an acknowledgment response. If they do not match, it still sends an
 * acknowledgment response but does not update the monitor IP and port.
 *
 * @param header Pointer to the command header containing the MAC address.
 * @param remoteIP The IP address of the remote client.
 * @param remotePort The port number of the remote client.
 */
void handleConnect(const cmd_head_t *header, IPAddress remoteIP, uint16_t remotePort)
{
  char receivedUUID[9];
  snprintf(receivedUUID, sizeof(receivedUUID), "%08lX", ntohl(header->mac));

  if (strncmp(receivedUUID, deviceUUID, 8) == 0)
  {
    monitorIP = remoteIP;
    monitorPort = remotePort;
    sendAckResponse(header, remoteIP, remotePort);
#if NETWORK_DEBUG_MODE
    _writeSerial("Valid client connected");
#endif
  }
  else
  {
    sendAckResponse(header, remoteIP, remotePort);
#if NETWORK_DEBUG_MODE
    _writeSerial("Invalid client connected");
#endif
  }
}

//------------------------------
// Echo original request
//------------------------------
/**
 * @brief Sends an acknowledgment response to a remote IP and port.
 *
 * This function constructs a response packet based on the provided command header,
 * sets the response fields, and sends the packet to the specified remote IP address
 * and port using UDP.
 *
 * @param header Pointer to the command header containing the original command details.
 * @param remoteIP The IP address of the remote device to send the response to.
 * @param remotePort The port number of the remote device to send the response to.
 */
void sendAckResponse(const cmd_head_t *header, IPAddress remoteIP, uint16_t remotePort)
{
  cmd_head_t resp;
  resp.mac = header->mac;
  resp.rand = 0;
  resp.indexpts = transactionIndex++;
  resp.cmd = header->cmd; // Echo original command
  Udp.beginPacket(remoteIP, remotePort);
  Udp.write((uint8_t *)&resp, sizeof(resp));
  Udp.endPacket();
}

//------------------------------
// Handle monitor requests
//------------------------------
/**
 * @brief Handles the monitor command received over the network.
 *
 * This function processes the incoming monitor command, validates the packet size,
 * extracts the monitor command data, and enables monitoring with the provided
 * IP address and port.
 *
 * @param data Pointer to the received data buffer.
 * @param size Size of the received data buffer.
 * @param remoteIP IP address of the sender.
 * @param remotePort Port number of the sender.
 */
void handleMonitorCommand(const uint8_t *data, int size, IPAddress remoteIP, uint16_t remotePort)
{
  if (size < (int)(sizeof(cmd_head_t) + sizeof(monitor_cmd_t)))
  {
#if NETWORK_DEBUG_MODE
    _writeSerial("Invalid monitor command: packet too small");
#endif
    return;
  }
  const monitor_cmd_t *monitor_cmd = reinterpret_cast<const monitor_cmd_t *>(data + sizeof(cmd_head_t));
  // Network byte order conversion
  monitorPort = ntohs(monitor_cmd->monitor_port);
  monitorIP = remoteIP; // Use sender's IP address
  monitorEnabled = true;

#if NETWORK_DEBUG_MODE
  char debugStr[64];
  snprintf(debugStr, sizeof(debugStr), "Monitoring enabled: %d.%d.%d.%d:%d",
           monitorIP[0], monitorIP[1], monitorIP[2], monitorIP[3], monitorPort);
  _writeSerial(debugStr);
#endif
}

/**
 * @brief Handles the mask command received from the network.
 *
 * This function processes a mask command by interpreting the data received
 * over the network. It checks if the packet size is sufficient, extracts the
 * mask command data, and updates the mask button states accordingly.
 *
 * @param data Pointer to the received data buffer.
 * @param size Size of the received data buffer.
 *
 * The function expects the data buffer to contain a command header followed
 * by the mask command data. The mask command data is interpreted in network
 * byte order and converted to host byte order before updating the mask button
 * states.
 *
 * If the packet size is too small, the function logs an error message (if
 * NETWORK_DEBUG_MODE is enabled) and returns without processing the data.
 *
 * The mask button states are updated as follows:
 * - maskLeftButton: State of the left mask button.
 * - maskRightButton: State of the right mask button.
 * - maskMiddleButton: State of the middle mask button.
 * - maskMovement: State of the mask movement.
 *
 * If NETWORK_DEBUG_MODE is enabled, the function logs the updated mask button
 * states to the serial output.
 */
void handleMaskCommand(const uint8_t *data, int size)
{
  if (size < (int)(sizeof(cmd_head_t) + sizeof(mask_cmd_t)))
  {
#if NETWORK_DEBUG_MODE
    _writeSerial("Invalid mask command: packet too small");
#endif
    return;
  }

  const mask_cmd_t *mask_cmd = reinterpret_cast<const mask_cmd_t *>(data + sizeof(cmd_head_t));

  // Convert network byte order to host and interpret as booleans
  maskLeftButton = ntohl(mask_cmd->mask_left) != 0;
  maskRightButton = ntohl(mask_cmd->mask_right) != 0;
  maskMiddleButton = ntohl(mask_cmd->mask_middle) != 0;
  maskMovement = ntohl(mask_cmd->mask_movement) != 0;

#if NETWORK_DEBUG_MODE
  char debugStr[64];
  snprintf(debugStr, sizeof(debugStr), "Mask: L=%d R=%d M=%d Move=%d",
           maskLeftButton, maskRightButton, maskMiddleButton, maskMovement);
  _writeSerial(debugStr);
#endif
}

// -------------------------------------------------------------
// setup()
// -------------------------------------------------------------
void setup()
{
  // Initialize watchdog
#if ENABLE_WATCHDOG
  watchdog.enable(Watchdog::TIMEOUT_8S);
#endif
  _beginSerial();
  // Initialize TFT Display
  _writeSerial("setting up display");
  tft.begin();
  _writeSerial("setting color");
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextWrap(false);
  tft.setRotation(1);
  // Initialize USB Host
  myusb.begin();
  _writeSerial("USB Host initialized");
  writeDisplay("Mouse:", "Disconnected", 0);
  // Initialize Ethernet
  ethernetInitialized = initializeEthernet();
  if (ethernetInitialized)
  {
    ethernetConnected = true;
    Udp.begin(localPort);
    _writeSerial("Ethernet initialized");
  }
  // generate UUID
  generateHardwareUUID();
  writeDisplay("UUID", deviceUUID, 2);
  char uuidStr[32];
  snprintf(uuidStr, sizeof(uuidStr), "UUID: %s", deviceUUID);
  _writeSerial(uuidStr);
  // Initialize usb device
  Mouse.begin();
  _writeSerial("Mouse started");
  writeDisplay("Service", "Offline", 3);
  _writeSerial("Setup complete");
  //  pinMode(LED_BUILTIN, OUTPUT);
}
// -------------------------------------------------------------
// 17 loop()
// -------------------------------------------------------------
void loop()
{
  // Keep USB Host tasks updated
  myusb.Task();
  handleMouse();
  updateAutoMouseMove();
  updateMouseMovementState();

  // Drain stored mouse packets every 10 ms if DEBUG_MODE is set
  static uint32_t lastMousePacketCheck = 0;
  uint32_t now = millis();
  if (now - lastMousePacketCheck >= 10)
  {
    lastMousePacketCheck += 10;
#if DEBUG_MODE
    MousePacket p;
    while (getNextMousePacket(p))
    {
      uint32_t latency = now - p.millis_stamp;
      char debugStr[128];
      snprintf(debugStr, sizeof(debugStr), "Captured host mouse => dx=%d dy=%d wheel=%d hwheel=%d btn=0x%02X (Latency: %u ms)",
               p.dx, p.dy, p.wheel, p.hwheel, p.buttons, (unsigned int)latency);
      _writeSerial(debugStr);
    }
#else
    MousePacket p;
    while (getNextMousePacket(p))
    {
      /* do nothing */
    }
#endif
  }

  // Ethernet link check
  if (ethernetInitialized)
  {
    if (Ethernet.linkStatus() == LinkOFF)
    {
      if (ethernetConnected)
      {
        writeDisplay("Ethernet:", "Cable Gone", 1);
#if NETWORK_DEBUG_MODE
        _writeSerial("cable gone");
#endif
        ethernetConnected = false;
      }
    }
    else
    {
      if (!ethernetConnected)
      {
        writeDisplay("Ethernet:", "Cable On", 1);
#if NETWORK_DEBUG_MODE
        _writeSerial("cable on");
#endif
        ethernetConnected = true;
      }
    }
  }

  if (ethernetInitialized && ethernetConnected)
  {
    // Read packets into ring buffer
    int packetSize;
    while ((packetSize = Udp.parsePacket()) > 0)
    {
#if NETWORK_DEBUG_MODE
      char debugStr[64];
      snprintf(debugStr, sizeof(debugStr), "Received packet: %d bytes", packetSize);
      _writeSerial(debugStr);
#endif
      if ((head + 1) % N_NET_PACKETS != tail)
      {
        ethernetBuffer[head].remoteIP = Udp.remoteIP();
        ethernetBuffer[head].remotePort = Udp.remotePort();
        ethernetBuffer[head].size = Udp.read(ethernetBuffer[head].data, 1500);
        head = (head + 1) % N_NET_PACKETS;
      }
    }
    // Process packets from ring buffer
    while (tail != head)
    {
      processPacket(
          ethernetBuffer[tail].data,
          ethernetBuffer[tail].size,
          ethernetBuffer[tail].remoteIP,
          ethernetBuffer[tail].remotePort);
      tail = (tail + 1) % N_NET_PACKETS;
    }
  }
  else
  {
    // If Ethernet is not initialized or cable is off, try to re-init every 5s
    static uint32_t lastEthernetCheck = 0;
    if (now - lastEthernetCheck > 5000)
    {
      lastEthernetCheck = now;
      if (!ethernetInitialized)
      {
        writeDisplay("Ethernet: ", "Trying..", 1);
        ethernetInitialized = initializeEthernet();
        if (ethernetInitialized)
        {
          ethernetConnected = true;
          Udp.begin(localPort);
        }
      }
    }
  }

  // Heartbeat LED every 1s
  static uint32_t lastHeartbeat = 0;
  if (now - lastHeartbeat > 500)
  {
    lastHeartbeat += 500;
    static bool ledState = false;
    ledState = !ledState;
    //    digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
  }

#if ENABLE_WATCHDOG
  // Watchdog handling
  if (((now - lastActivityTime) <= TIMEOUT_DURATION) && ((now - reset_time) >= RESET_DURATION))
  {
    reset_time = now;
    watchdog.reset();
  }
#endif
}

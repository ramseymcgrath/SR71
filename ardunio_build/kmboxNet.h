#include <cstdint>
#define cmd_connect 0xaf3c2828
#define cmd_mouse_move 0xaede7345
#define cmd_mouse_left 0x9823AE8D
#define cmd_mouse_middle 0x97a3AE8D
#define cmd_mouse_right 0x238d8212
#define cmd_mouse_wheel 0xffeead38
#define cmd_mouse_automove 0xaede7346
#define cmd_keyboard_all 0x123c2c2f
#define cmd_reboot 0xaa8855aa
#define cmd_bazerMove 0xa238455a
#define cmd_monitor 0x27388020
#define cmd_debug 0x27382021
#define cmd_mask_mouse 0x23234343
#define cmd_unmask_all 0x23344343
#define cmd_setconfig 0x1d3d3323
#define cmd_showpic 0x12334883

#pragma pack(push, 1) // Exact byte alignment
typedef struct
{
  uint32_t mac;
  uint32_t rand;
  uint32_t indexpts;
  uint32_t cmd;
} cmd_head_t;
#pragma pack(pop)

typedef struct
{
  unsigned char buff[1024];
} cmd_data_t;

typedef struct
{
  unsigned short buff[512];
} cmd_u16_t;

typedef struct __attribute__((packed))
{
  int32_t button;
  int32_t x;
  int32_t y;
  int32_t wheel;
  int32_t point[10];
} soft_mouse_t;

typedef struct
{
  char ctrl;
  char resvel;
  char button[10];
} soft_keyboard_t;

typedef struct
{
  cmd_head_t head;
  union
  {
    cmd_data_t u8buff;
    cmd_u16_t u16buff;
    soft_mouse_t cmd_mouse;
    soft_keyboard_t cmd_keyboard;
  };
} client_tx;

enum
{
  err_creat_socket = -9000,
  err_net_version,
  err_net_tx,
  err_net_rx_timeout,
  err_net_cmd,
  err_net_pts,
  success = 0,
  usb_dev_tx_timeout,
};

#pragma pack(push, 1)
typedef struct
{
  int32_t x;
  int32_t y;
} mouse_move_t;

typedef struct
{
  int32_t targetX;
  int32_t targetY;
  int32_t ctrlX1;
  int32_t ctrlY1;
  int32_t ctrlX2;
  int32_t ctrlY2;
  uint32_t duration;
} bezier_move_t;

typedef struct
{
  uint8_t state;
} mouse_button_t;

typedef struct
{
  int32_t value;
} mouse_wheel_t;

typedef struct
{
  int32_t x;
  int32_t y;
  uint32_t duration;
} auto_move_t;
#pragma pack(pop)

#pragma pack(push, 1) // Exact byte alignment

// For cmd_monitor (0x27388020)
typedef struct
{
  uint16_t monitor_port; // 16-bit port in network byte order
  uint16_t padding;      // Alignment padding (not used)
} monitor_cmd_t;

// For cmd_mask_mouse (0x23234343)
typedef struct
{
  int32_t mask_left;     // Non-zero to mask left button
  int32_t mask_right;    // Non-zero to mask right button
  int32_t mask_middle;   // Non-zero to mask middle button
  int32_t mask_movement; // Non-zero to mask movement
} mask_cmd_t;

#pragma pack(pop)

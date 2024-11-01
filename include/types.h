#ifndef TYPES
#define TYPES

#include <stddef.h>
#include <inttypes.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

struct BufferDispatcher
{
};

struct Buffer
{
  struct Request** requests;
  size_t size;
  size_t current_index;
};

struct Request
{
  u32 gen_number;
  u32 gen_time;
  u32 buf_time;
  u32 dev_time;
  u8 is_active;
};

struct RequestDispatcher
{
};

struct Device
{
  u32 number;
  u8 is_free;
};

struct Generator
{
  u32 number;
};

struct MassServiceSystem
{
  struct BufferDispatcher b_disp;
  struct RequestDispatcher r_disp;
  struct Buffer buffer;
  struct Device** devices;
  size_t devices_len;
};

struct Environment
{
  struct Generator* generators;
};

struct Event
{
  u8 type;
  u8 is_acitve;
  u32 time_in_sec;
};

struct EventCalendar
{
  struct Event* events;
};

struct Model
{
  struct MassServiceSystem mss;
  struct Environment env;
  struct EventCalendar calendar;
};

#endif

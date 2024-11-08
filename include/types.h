#ifndef TYPES
#define TYPES

#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>

typedef uint16_t u16;
typedef uint32_t u32;

struct Buffer
{
  struct Request* requests;
  size_t size;
  size_t current_index;
};

struct Request
{
  u32 gen_number;
  u32 gen_time;
  u32 buf_time;
  u32 dev_time;
  bool is_active;
};

struct Device
{
  u32 number;
  bool is_free;
};

struct Generator
{
  u32 number;
};

struct MassServiceSystem
{
  struct Buffer* buffer;
  struct Device* devices;
  size_t devices_len;
};

struct Environment
{
  struct Generator* generators;
  size_t generators_len;
};

enum EVENT_TYPE
{
  UNDEFINED,
  GET_REQUEST,
  DEVICE_FREE,
  STOP_MODELING,
};

union EventData
{
  struct Request request;
  struct Device device;
};

struct Event
{
  union EventData data;
  enum EVENT_TYPE type;
  u32 time_in_sec;
  bool is_active;
};

struct EventCalendar
{
  struct Event* events;
  size_t events_len;
};

#endif

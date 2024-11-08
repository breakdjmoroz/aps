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
  int gen_number;
  double gen_time;
  double buf_time;
  double dev_time;
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
  bool is_active;
  double time_in_sec;
};

struct EventCalendar
{
  struct Event* events;
  size_t events_len;
};

#endif

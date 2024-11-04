#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "interfaces.h"

int buffer_insert(struct Buffer* const buffer, const struct Request* const request)
{
  int err = 0;
  struct Request rejected_request;

  buffer_insert_with_rejected(buffer, request, &rejected_request, &err);

  free(&rejected_request);

  return err;
}

void buffer_insert_with_rejected(
    struct Buffer* const buffer,
    const struct Request* const in_request,
    struct Request* rejected_request,
    int* const err_num)
{
  int err = 0;
  size_t current_index = buffer->current_index;
  rejected_request = NULL;

  if (buffer == NULL || in_request == NULL || rejected_request == NULL)
  {
    err = -1;
  }
  else
  {
    //Cycle insert rule
    while (buffer->requests[current_index].is_active)
    {
      current_index = (current_index + 1) % buffer->size;
      //Reject under pointer
      if (current_index == buffer->current_index)
      {
        *rejected_request = buffer->requests[current_index];
        rejected_request->is_active = false;
      }
    }

    //TODO: Fill in_request->buf_time
    buffer->requests[current_index] = *in_request;

    current_index = (current_index + 1) % buffer->size;
  }
  if (err_num != NULL)
  {
    *err_num = err;
  }
}

void buffer_extract(struct Buffer* const buffer, struct Request* request, int* const err_num)
{
  u32 max_priority = 0;
  u32 current_priority = 0;
  size_t current_index = 0;
  size_t request_index = 0;
  int err = 0;
  size_t i;

  if (buffer == NULL || request == NULL)
  {
    err = -1;
  }

  for (i = 0; i < buffer->size; ++i)
  {
    current_index = (buffer->current_index + i) % buffer->size;
    current_priority =
      buffer->requests[current_index].gen_number;

    if (current_priority > max_priority)
    {
      max_priority = current_priority;
      request_index = current_index;
    }
    else if ((current_priority == max_priority) &&
        (buffer->requests[current_index].buf_time <
         buffer->requests[request_index].buf_time))
    {
      request_index = current_index;
    }
  }

  *request = buffer->requests[request_index];
  buffer->requests[request_index].is_active = false;

  if (err_num != NULL)
  {
    *err_num = err;
  }
}

int select_device(const struct MassServiceSystem* const mss)
{
  int i = 0;

  if (mss == NULL)
  {
    return -2;
  }

  while ((i < mss->devices_len) && !(mss->devices[i].is_free))
  {
    ++i;
  }

  if (!(i < mss->devices_len))
  {
    i = -1;
  }

  return i;
}

void  allocate_devices(struct Device* devices, size_t devices_len)
{
  size_t i = 0;
  if (devices != NULL)
  {
    for(i = 0; i < devices_len; ++i)
    {
      devices[i].number = (u32)i;
      devices[i].is_free = true;
    }
  }
}

bool allocate_buffer(struct Buffer* buffer, size_t buf_size)
{
  size_t i = 0;
  bool is_allocated = false;

  if (buffer != NULL)
  {
    buffer->requests = (struct Request*)malloc(buf_size * sizeof(struct Request));

    if (buffer->requests != NULL)
    {
      is_allocated = true;

      buffer->size = buf_size;
      buffer->current_index = 0;

      for(i = 0; i < buf_size; ++i)
      {
        buffer->requests[i].is_active = false;
      }
    }
  }

  return is_allocated;
}

struct MassServiceSystem* new_mss(size_t devices_len, size_t buf_size)
{
  size_t i = 0;
  bool is_allocated = true;
  struct MassServiceSystem* mss = (struct MassServiceSystem*)malloc(sizeof(struct MassServiceSystem));

  if (mss != NULL)
  {
    mss->devices_len=devices_len;
    mss->devices = (struct Device*)malloc(sizeof(struct Device*) * devices_len);
    if (mss->devices != NULL)
    {
      allocate_devices(mss->devices, devices_len);
    }
    else
    {
      is_allocated = false;
    }

    if (is_allocated)
    {
      mss->buffer = (struct Buffer*)malloc(sizeof(struct Buffer));
      if (mss->buffer != NULL)
      {
        is_allocated = allocate_buffer(mss->buffer, buf_size);

        if (!is_allocated)
        {
          free(mss->buffer);
        }
      }
    }

    if (!is_allocated)
    {
        free(mss);
        mss = NULL;
    }
  }

  return mss;
}

struct EventCalendar* new_calendar(size_t events_len)
{
  size_t i = 0;
  bool is_allocated = true;
  struct EventCalendar* calendar = (struct EventCalendar*)malloc(sizeof(struct EventCalendar));

  if (calendar != NULL)
  {
    calendar->events_len=events_len;
    calendar->events = (struct Event*)malloc(sizeof(struct Event) * events_len);
    if (calendar->events != NULL)
    {
      for(i = 0; i < events_len; ++i)
      {
        calendar->events[i].is_active = false;
      }
    }
    else
    {
      free(calendar);
      calendar = NULL;
    }
  }

  return calendar;
}

struct Environment* new_env(size_t gen_num)
{
  size_t i = 0;
  struct Environment* env = (struct Environment*)malloc(sizeof(struct Environment));

  if (env != NULL)
  {
    env->generators = (struct Generator*)malloc(gen_num * sizeof(struct Generator));
    if (env->generators != NULL)
    {
      for(i = 0; i < gen_num; ++i)
      {
        env->generators[i].number = i;
      }
    }
    else
    {
      free(env);
      env = NULL;
    }
  }

  return env;
}

void generate_requests(const struct Environment* const env, struct EventCalendar* calendar)
{
  size_t i = 0;

  for (i = 0; i < env->generators_len; ++i)
  {
    generate_request_for(env->generators[i].number, calendar);
  }
}

struct Event get_next_event(const struct EventCalendar* const calendar)
{
  size_t i = 0;
  u32 next_time = UINT32_MAX;
  size_t next_time_index = 0;

  for (i = 0; i < calendar->events_len; ++i)
  {
    if(calendar->events[i].is_active)
    {
      if (calendar->events[i].time_in_sec < next_time)
      {
        next_time = calendar->events[i].time_in_sec;
        next_time_index = i;
      }
    }
  }

  calendar->events[next_time_index].is_active = false;

  return calendar->events[next_time_index];
}

void generate_request_for(u32 generator_number, struct EventCalendar* calendar)
{
  struct Request request =
  {
    .gen_number = generator_number,
    .gen_time = 0, /*TODO: insert correct distribution law*/
    .is_active = true,
  };

  struct Event event =
  {
    .data.request = request,
    .type = GET_REQUEST,
    .time_in_sec = 0, /*TODO: insert correct distribution law*/
    .is_active = true,
  };

  insert_event(calendar, &event);
}

void serve_a_request(struct Request* request, struct Device* device, struct EventCalendar* calendar)
{
  request->dev_time = 0; /*TODO: use correct distribution law*/
  device->is_free = false;

  struct Event event =
  {
    .data.device = *device,
    .type = DEVICE_FREE,
    .time_in_sec = 0, /*TODO: insert correct distribution law*/
    .is_active = true,
  };

  insert_event(calendar, &event);
}

void insert_event(struct EventCalendar* calendar, struct Event* event)
{
  size_t i = 0;
  while (calendar->events[i].is_active)
  {
    ++i;
  }
  calendar->events[i] = *event;
}

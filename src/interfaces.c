#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
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
    while (buffer->requests[current_index]->is_active)
    {
      current_index = (current_index + 1) % buffer->size;
      //Reject under pointer
      if (current_index == buffer->current_index)
      {
        rejected_request = buffer->requests[current_index];
        rejected_request->is_active = 0;
      }
    }

    //TODO: Fill in_request->buf_time
    buffer->requests[current_index] = in_request;

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
      buffer->requests[current_index]->gen_number;

    if (current_priority > max_priority)
    {
      max_priority = current_priority;
      request_index = current_index;
    }
    else if ((current_priority == max_priority) &&
        (buffer->requests[current_index]->buf_time <
         buffer->requests[request_index]->buf_time))
    {
      request_index = current_index;
    }
  }

  request = buffer->requests[request_index];

  if (err_num != NULL)
  {
    *err_num = err;
  }
}

size_t select_device(const struct MassServiceSystem* const mss)
{
  size_t i = 0;

  if (mss == NULL)
  {
    return -1;
  }

  while ((i < mss->devices_len) && !(mss->devices[i]->is_free))
  {
    ++i;
  }

  return i;
}

bool allocate_devices(struct Device** devices, size_t devices_len)
{
  size_t i = 0;
  bool is_allocated = false;

  if (devices != NULL)
  {
    is_allocated = true;

    for(i = 0; is_allocated && (i < devices_len); ++i)
    {
      devices[i] = (struct Device*)malloc(sizeof(struct Device));
      if (devices[i] != NULL)
      {
        (devices[i])->number = (u32)i;
        (devices[i])->is_free = true;
      }
      else
      {
        is_allocated = false;
      }
    }

    if (!is_allocated)
    {
      for(i = i - 1; i >= 0; --i)
      {
        free(devices[i]);
      }
    }
  }

  return is_allocated;
}

bool allocate_buffer(struct Buffer* buffer, size_t buf_size)
{
  size_t i = 0;
  bool is_allocated = false;

  if (buffer != NULL)
  {
    buffer->requests = (struct Request**)malloc(buf_size * sizeof(struct Request*));
    if (buffer->requests != NULL)
    {
      is_allocated = true;
    }

    buffer->size = buf_size;
    for(i = 0; i < buf_size; ++i)
    {
      buffer->requests[i] = NULL;
    }
    buffer->current_index = 0;
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
    mss->devices = (struct Device**)malloc(sizeof(struct Device*) * devices_len);
    if (mss->devices != NULL)
    {
      is_allocated = allocate_devices(mss->devices, devices_len);
      if (!is_allocated)
      {
        free(mss->devices);
      }
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
    calendar->events = (struct Event**)malloc(sizeof(struct Event*) * events_len);
    if (calendar->events != NULL)
    {
      for(i = 0; is_allocated && (i < events_len); ++i)
      {
        calendar->events[i] = (struct Event*)malloc(sizeof(struct Event));
        if (calendar->events[i] != NULL)
        {
          (calendar->events[i])->data = NULL;
          (calendar->events[i])->type = UNDEFINED;
          (calendar->events[i])->time_in_sec = 0;
          (calendar->events[i])->is_active = false;
        }
        else
        {
          is_allocated = false;
        }
      }

      if (!is_allocated)
      {
        for(i = i - 1; i >= 0; --i)
        {
          free(calendar->events[i]);
        }
        free(calendar->events);
      }
    }
    else
    {
      is_allocated = false;
    }

    if (!is_allocated)
    {
        free(calendar);
        calendar = NULL;
    }
  }

  return calendar;
}

struct Event get_next_event()
{
}

void generate_requests()
{
}

void generate_request_for(u32 generator_number)
{
}

bool have_free_device()
{
}

void serve_a_request()
{
}

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <float.h>
#include <time.h>

/* Move this to distributions.* files */
#include <math.h>

#define RAND_EXP_LAMBDA (0.05)

double rand_exp(double lambda)
{
  double u = rand() / (RAND_MAX + 1.0);
  return -log(1.0 - u) / lambda;
}
/* End of block */

#include "interfaces.h"

const struct Request EMPTY_REQUEST =
{
  .gen_number = -1,
  .gen_time = -1.0,
  .buf_time = -1.0,
  .dev_time = -1.0,
  .is_active = false,
};

bool is_stop_modeling(struct Event event)
{
  return event.type == STOP_MODELING;
}

void create_break_event(struct EventCalendar* calendar, double break_time)
{
  struct Event break_event =
  {
    .type = STOP_MODELING,
    .time_in_sec = break_time,
    .is_active = true,
  };

  insert_event(calendar, &break_event);
}

bool is_empty_request(struct Request request)
{
  return !(request.gen_number == EMPTY_REQUEST.gen_number &&
      request.gen_time == EMPTY_REQUEST.gen_time &&
      request.buf_time == EMPTY_REQUEST.buf_time &&
      request.dev_time == EMPTY_REQUEST.dev_time &&
      request.is_active == EMPTY_REQUEST.is_active);
}

int buffer_insert(struct Buffer* const buffer, struct Request* request)
{
  int err = 0;
  struct Request rejected_request;

  buffer_insert_with_rejected(buffer, request, &rejected_request, &err);

  return err;
}

void buffer_insert_with_rejected(
    struct Buffer* const buffer,
    struct Request* in_request,
    struct Request* rejected_request,
    int* const err_num)
{
  int err = 0;
  size_t current_index = buffer->current_index;

  if (buffer == NULL || in_request == NULL || rejected_request == NULL)
  {
    err = -1;
  }
  else
  {
    *rejected_request = EMPTY_REQUEST;
    //Cycle insert rule
    while (buffer->requests[current_index].is_active)
    {
      current_index = (current_index + 1) % buffer->size;
      //Reject under pointer
      if (current_index == buffer->current_index)
      {
        *rejected_request = buffer->requests[current_index];
        rejected_request->is_active = false;
        rejected_request->buf_time = global_current_time;
        break;
      }
    }

    buffer->requests[current_index] = *in_request;

    current_index = (current_index + 1) % buffer->size;
    buffer->current_index = current_index;
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
  double min_gen_time = DBL_MAX;
  double current_gen_time = 0.0;
  size_t current_index = buffer->current_index;
  size_t request_index = buffer->current_index;
  int err = 0;
  bool is_buffer_full = false;
  size_t i;

  if (buffer == NULL || request == NULL)
  {
    err = -1;
  }

  for (i = 0; i < buffer->size && !err; ++i)
  {
    current_index = (current_index + i) % buffer->size;
    if (buffer->requests[current_index].is_active)
    {
      is_buffer_full = true;
      current_priority =
        buffer->requests[current_index].gen_number;
      current_gen_time =
        buffer->requests[current_index].gen_time;

      if (current_priority > max_priority)
      {
        max_priority = current_priority;
        min_gen_time = current_gen_time;
        request_index = current_index;
      }
      else if ((current_priority == max_priority) &&
              (current_gen_time < min_gen_time))
      {
        min_gen_time = current_gen_time;
        request_index = current_index;
      }
    }
  }

  if (is_buffer_full && !err)
  {
    buffer->requests[request_index].buf_time = global_current_time;
    *request = buffer->requests[request_index];
    buffer->current_index = request_index + 1;
    buffer->requests[request_index].is_active = false;
  }
  else
  {
    err = -2;
  }

  if (err_num != NULL)
  {
    *err_num = err;
  }
}

int select_device(const struct MassServiceSystem* const mss)
{
  size_t i = 0;
  int device_index = -1;

  if (mss == NULL)
  {
    return -2;
  }

  for (i = 0; i < (mss->devices_len); ++i)
  {
    if (mss->devices[i].is_free)
    {
      device_index = (int)i;
      break;
    }
  }

  return device_index;
}

static void allocate_devices(struct Device* devices, size_t devices_len)
{
  size_t i;

  if (devices != NULL)
  {
    for(i = 0; i < devices_len; ++i)
    {
      devices[i].number = (u32)i;
      devices[i].is_free = true;
      devices[i].use_time = 0.0;
    }
  }
}

static bool allocate_buffer(struct Buffer* buffer, size_t buf_size)
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
    mss->devices = (struct Device*)malloc(sizeof(struct Device) * devices_len);
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

struct EventCalendar* new_calendar(size_t generators_len, size_t devices_len)
{
  size_t events_len = generators_len + devices_len + 1; /* Plus one space for STOP_MODELING */
  struct EventCalendar* calendar = (struct EventCalendar*)malloc(sizeof(struct EventCalendar));
  struct Event* events = (struct Event*)malloc(sizeof(struct Event) * events_len);

  if (calendar && events)
  {
    calendar->generators_len = generators_len;
    calendar->devices_len = devices_len;
    calendar->events_len = events_len;
    calendar->events = events;

    for(size_t i = 0; i < events_len; ++i)
    {
      calendar->events[i].is_active = false;
    }
  }

  if (!events)
  {
    free(calendar);
    calendar = NULL;
  }

  return calendar;
}

struct Environment* new_env(size_t gen_num)
{
  size_t i = 0;
  struct Environment* env = (struct Environment*)malloc(sizeof(struct Environment));

  if (env != NULL)
  {
    env->generators_len = gen_num;
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

void delete_mss(struct MassServiceSystem* mss)
{
  free(mss->buffer);
  free(mss->devices);
  free(mss);
}

void delete_calendar(struct EventCalendar* calendar)
{
  free(calendar->events);
  free(calendar);
}

void delete_env(struct Environment* env)
{
  free(env->generators);
  free(env);
}

struct Event get_next_event(const struct EventCalendar* const calendar, bool* err)
{
  size_t i = 0;
  double next_time = DBL_MAX;
  size_t next_time_index = 0;
  bool is_empty = true;

  for (i = 0; i < calendar->events_len; ++i)
  {
    if(calendar->events[i].is_active)
    {
      is_empty = false;
      if (calendar->events[i].time_in_sec < next_time)
      {
        next_time = calendar->events[i].time_in_sec;
        next_time_index = i;
      }
    }
  }

  if (err != NULL)
  {
    *err = is_empty;
  }

  calendar->events[next_time_index].is_active = false;

  return calendar->events[next_time_index];
}

void generate_request_for(u32 generator_number, struct EventCalendar* calendar, struct Request* generated_request)
{
  struct Request request =
  {
    .gen_number = generator_number,
    .buf_time = 0.0,
    .dev_time = 0.0,
    .is_active = true,
  };

  struct Event event =
  {
    .type = GET_REQUEST,
    .is_active = true,
  };

  request.gen_time = global_current_time + (double)((rand() % 998) + 1) * 0.1;

  event.time_in_sec = request.gen_time;
  event.data.request = request,

  insert_event(calendar, &event);

  if (generated_request)
  {
    *generated_request = request;
  }
}

void serve_a_request(struct Request* request, struct Device* device, struct EventCalendar* calendar)
{
  request->dev_time = global_current_time + rand_exp(RAND_EXP_LAMBDA);

  device->is_free = false;
  device->use_time = global_current_time;

  struct Event event =
  {
    .data.device = *device,
    .type = DEVICE_FREE,
    .time_in_sec = request->dev_time,
    .is_active = true,
  };

  insert_event(calendar, &event);
}

void insert_event(struct EventCalendar* calendar, struct Event* event)
{
  size_t index = 0;

  switch(event->type)
  {
    case GET_REQUEST:
      index = event->data.request.gen_number;
      break;
    case DEVICE_FREE:
      index = event->data.device.number + calendar->generators_len;
      break;
    case STOP_MODELING:
      index = calendar->events_len - 1;
      break;
    default:
      return;
      break;
  }
  
  calendar->events[index] = *event;
}

void print_calendar(const struct EventCalendar* const calendar)
{
  printf("Current time (sec): %lf\n", global_current_time);
  for (size_t i = 0; i < calendar->events_len; ++i)
  {
    if (i < calendar->generators_len)
    {
      printf("|  Generator[%d] | Next time: %lf | Active flag: %d |\n",
          i,
          calendar->events[i].time_in_sec,
          calendar->events[i].is_active
          );
    }
    else if (i < calendar->devices_len + calendar->generators_len)
    {
      printf("|   Device[%d]   | Next time: %lf | Active flag: %d |\n",
          i - calendar->generators_len,
          calendar->events[i].time_in_sec,
          calendar->events[i].is_active
          );
    }
    else
    {
      printf("| Stop modeling | Next time: %lf | Active flag: %d |\n",
          calendar->events[i].time_in_sec,
          calendar->events[i].is_active
          );
    }
  }
}

void print_buffer(const struct Buffer* const buf)
{
  printf("Buffer: ");
  for (size_t i = 0; i < buf->size; ++i)
  {
    if (buf->requests[i].is_active)
    {
      printf("| %d ", buf->requests[i].gen_number);
    }
    else
    {
      printf("| --- ");
    }
  }
  printf("|\n");
}

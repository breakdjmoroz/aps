#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "types.h"
#include "interfaces.h"
#include "statistic.h"

#define N_DEVICES     (20)
#define N_GENERATORS  (2)
#define BUF_SIZE      (2)
#define STOP_TIME     (2.000)
#define ONESHOT_MODE  (true)

const struct Event BREAK_EVENT =
{
  .type = STOP_MODELING,
  .time_in_sec = STOP_TIME,
  .is_active = true,
};

const struct Request EMPTY_REQUEST =
{
  .gen_number = -1,
  .gen_time = -1.0,
  .buf_time = -1.0,
  .dev_time = -1.0,
  .is_active = false,
};

double global_current_time;

int main()
{
  size_t i;
  bool is_modeling = true;
  struct MassServiceSystem* mss = new_mss(N_DEVICES, BUF_SIZE);
  struct EventCalendar* calendar = new_calendar(N_GENERATORS, N_DEVICES);
  struct Environment* env = new_env(N_GENERATORS);

  struct StatisticTable* stat = new_stat(N_GENERATORS, N_DEVICES);
  start_statistic(stat);

  global_current_time = 0.0;
  insert_event(calendar, &BREAK_EVENT);

  for (i = 0; i < env->generators_len; ++i)
  {
    struct Request tmp_request;
    generate_request_for(env->generators[i].number, calendar, &tmp_request);
  }

  while(is_modeling)
  {
#if ONESHOT_MODE
    print_calendar(calendar);
#endif

    struct Event event = get_next_event(calendar, NULL);
    struct Request request;
    int device_index;

    global_current_time = event.time_in_sec;

    if (is_equal_events(event, BREAK_EVENT))
    {
      is_modeling = false;
      for(size_t i = 0; i < calendar->events_len; ++i)
      {
        calendar->events[i].is_active = false;
      }
    }

    switch(event.type)
    {
      case GET_REQUEST:
        request = event.data.request;
        generate_request_for(request.gen_number, calendar, NULL);

        device_index = select_device(mss);
        if (device_index >= 0)
        {
          serve_a_request(&request, &(mss->devices[device_index]), calendar);
          collect_statistic(stat, &request, SERVED_REQUEST);
        }
        else
        {
          int err;
          struct Request rejected_request;
          buffer_insert_with_rejected(mss->buffer, &request, &rejected_request, &err);
          if (!is_equal_requests(rejected_request, EMPTY_REQUEST))
          {
            collect_statistic(stat, &rejected_request, REJECTED_REQUEST);
          }
        }
        break;
      case DEVICE_FREE:
        int err;
        mss->devices[event.data.device.number].is_free = true;
        collect_statistic_device(stat, &(mss->devices[event.data.device.number]), event.data.device.number);
        buffer_extract(mss->buffer, &request, &err);
        if (err == 0)
        {
          device_index = select_device(mss);
          if (device_index >= 0)
          {
              serve_a_request(&request, &(mss->devices[device_index]), calendar);
              collect_statistic(stat, &request, SERVED_REQUEST);
          }
        }
        break;
    }
#if ONESHOT_MODE
    getchar();
#endif
  }

#if ONESHOT_MODE
  print_calendar(calendar);
#endif
  stop_statistic(stat);
  print_statistic(stat);

  return 0;
}

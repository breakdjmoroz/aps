#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "types.h"
#include "interfaces.h"
#include "statistic.h"

#define N_PARAMS      (5)

const char MESSAGE_USAGE[] = "Usage: model.out N_DEV N_GEN BUF_SIZE STOP_TIME MODE\nN_DEV - amount of devices\nN_GEN - amount of generators\nBUF_SIZE - buffer size\nSTOP_TIME - simulation end time\nMODE - 0 means automatic mode, 1 means oneshot mode\n";

double global_current_time;

int main(int argc, char* argv[])
{
  if (argc != (1 + N_PARAMS))
  {
    fprintf(stderr, "Your input:\n");
    for (size_t i = 0; i < argc; ++i)
    {
      fprintf(stderr, "%s\n", argv[i]);
    }
    fprintf(stderr, "Wrong amount of arguments!\n%s\n", MESSAGE_USAGE);
    return -1;
  }

  const size_t N_DEVICES    = (size_t)atoi(argv[1]);
  const size_t N_GENERATORS = (size_t)atoi(argv[2]);
  const size_t BUF_SIZE     = (size_t)atoi(argv[3]);
  const double STOP_TIME    = (double)atof(argv[4]);
  const bool ONESHOT_MODE   = (bool)atoi(argv[5]);

  size_t i;
  bool is_modeling = true;
  struct MassServiceSystem* mss = new_mss(N_DEVICES, BUF_SIZE);
  struct EventCalendar* calendar = new_calendar(N_GENERATORS, N_DEVICES);
  struct Environment* env = new_env(N_GENERATORS);

  struct StatisticTable* stat = new_stat(N_GENERATORS, N_DEVICES);
  start_statistic(stat);

  global_current_time = 0.0;

  create_break_event(calendar, STOP_TIME);

  for (i = 0; i < env->generators_len; ++i)
  {
    struct Request tmp_request;
    generate_request_for(env->generators[i].number, calendar, &tmp_request);
  }

  while(is_modeling)
  {
#if ONESHOT_MODE
    print_calendar(calendar);
    print_buffer(mss->buffer);
#endif

    struct Event event = get_next_event(calendar, NULL);
    struct Request request;
    int device_index;

    global_current_time = event.time_in_sec;

    if (is_stop_modeling(event))
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
  print_buffer(mss->buffer);
#endif
  stop_statistic(stat);
  print_statistic(stat);

  return 0;
}

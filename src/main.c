#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "types.h"
#include "interfaces.h"
#include "statistic.h"

#define N_DEVICES     (5)
#define N_EVENTS      (160)
#define N_GENERATORS  (6)
#define BUF_SIZE      (8)
#define STOP_TIME     (2.0)

const struct Event BREAK_EVENT =
{
  .type = STOP_MODELING,
  .time_in_sec = STOP_TIME,
  .is_active = true,
};

clock_t global_start_time;
clock_t global_current_time;
clock_t global_end_time;

int main()
{
  size_t i;
  bool is_modeling = true;
  struct MassServiceSystem* mss = new_mss(N_DEVICES, BUF_SIZE);
  struct EventCalendar* calendar = new_calendar(N_EVENTS);
  struct Environment* env = new_env(N_GENERATORS);

  struct StatisticTable* stat = new_stat(N_EVENTS, N_GENERATORS, N_DEVICES);
  start_statistic();

  insert_event(calendar, &BREAK_EVENT);

  for (i = 0; i < env->generators_len; ++i)
  {
    struct Request tmp_request;
    generate_request_for(env->generators[i].number, calendar, &tmp_request);
    collect_statistic(stat, &tmp_request, GENERATED_REQUEST);
  }

  while(is_modeling)
  {
    struct Event event = get_next_event(calendar);
    struct Request request;

    printf("<<< time (sec): %lf\n", event.time_in_sec);
    switch(event.type)
    {
      case GET_REQUEST:
        printf(">>> event: GET_REQUEST\n");
        request = event.data.request;
        generate_request_for(request.gen_number, calendar, NULL);
        int device_index = select_device(mss);
        if (device_index >= 0)
        {
          serve_a_request(&request, &env->generators[device_index], calendar);
          collect_statistic(stat, &request, SERVED_REQUEST);
        }
        else
        {
          int err;
          struct Request* rejected_request;
          buffer_insert_with_rejected(mss->buffer, &request, rejected_request, &err);
          if (rejected_request != NULL)
          {
            collect_statistic(stat, rejected_request, REJECTED_REQUEST);
          }
        }
        break;
      case DEVICE_FREE:
        printf(">>> event: DEVICE_FREE\n");
        int err;
        buffer_extract(mss->buffer, &request, &err);
        if (err > 0)
        {
          int device_index = select_device(mss);
          if (device_index >= 0)
          {
              serve_a_request(&request, &env->generators[device_index], calendar);
              collect_statistic(stat, &request, SERVED_REQUEST);
          }
        }
        break;
      case STOP_MODELING:
        printf(">>> event: STOP_MODELING\n");
        is_modeling = false;
        stop_statistic(stat);
        break;
    }
  }

  print_statistic(stat);

  return 0;
}

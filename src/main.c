#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "types.h"
#include "interfaces.h"
#include "statistic.h"

#define N_DEVICES     (1)
#define N_EVENTS      (3200000)
#define N_GENERATORS  (3)
#define BUF_SIZE      (4)
#define STOP_TIME     (2.000)

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

clock_t global_start_time;
double global_current_time;
clock_t global_end_time;

int main()
{
  size_t i;
  bool is_modeling = true;
  bool is_generating_request = true;
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
    int device_index;

    global_current_time = event.time_in_sec;

    if (!is_generating_request && is_equal_events(event, BREAK_EVENT))
    {
      is_modeling = false;
    }

    switch(event.type)
    {
      case GET_REQUEST:
        printf("==========================\n");
        printf(">>> event: GET_REQUEST\n");
        printf("time (sec): %lf\n", global_current_time);
        request = event.data.request;
        printf(">>>>>>>>> request: generator's num is %d\n", request.gen_number);
        if(is_generating_request)
        {
          generate_request_for(request.gen_number, calendar, NULL);
        }
        device_index = select_device(mss);
        if (device_index >= 0)
        {
          printf(">>>>>>>>> device [%d]: serve requset immideatly\n", device_index);
          serve_a_request(&request, &(mss->devices[device_index]), calendar);
          collect_statistic(stat, &request, SERVED_REQUEST);
        }
        else
        {
          printf(">>>>>>>>> device: send request to buffer\n", device_index);
          int err;
          struct Request rejected_request;
          buffer_insert_with_rejected(mss->buffer, &request, &rejected_request, &err);
          if (!is_equal_requests(rejected_request, EMPTY_REQUEST))
          {
            printf(">>>>>>>>> buffer: reject request under pointer [%d]\n", (mss->buffer->current_index - 1) % mss->buffer->size);
            collect_statistic(stat, &rejected_request, REJECTED_REQUEST);
          }
          else
          {
            printf(">>>>>>>>> buffer [%d]: insert a request\n", (mss->buffer->current_index - 1) % mss->buffer->size);
          }
        }
        break;
      case DEVICE_FREE:
        printf("==========================\n");
        printf(">>> event: DEVICE_FREE\n");
        printf("time (sec): %lf\n", global_current_time);
        int err;
        mss->devices[event.data.device.number].is_free = true;
        printf(">>>>>>>>> device [%d]: service finished\n", event.data.device.number);
        buffer_extract(mss->buffer, &request, &err);
        if (err == 0)
        {
          printf(">>>>>>>>> buffer [%d]: extract a request\n", (mss->buffer->current_index - 1) % mss->buffer->size);
          device_index = select_device(mss);
          if (device_index >= 0)
          {
              serve_a_request(&request, &(mss->devices[device_index]), calendar);
              collect_statistic(stat, &request, SERVED_REQUEST);
          }
        }
        else if (!is_generating_request)
        {
          printf(">>>>>>>>> buffer: all requests served\n");
          is_modeling = false;
        }
        else
        {
          printf(">>>>>>>>> device [%d]: halt\n", event.data.device.number);
        }
        break;
      case STOP_MODELING:
        printf("==========================\n");
        printf("||     STOP_MODELING    ||\n");
        printf("==========================\n");
        printf("time (sec): %lf\n", global_current_time);
        is_generating_request = false;
        break;
    }
  }

  printf(">>> event: END_MODELING\n");

  stop_statistic(stat);

  print_statistic(stat);

  return 0;
}

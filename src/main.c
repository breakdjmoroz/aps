#include <stdbool.h>

#include "types.h"
#include "interfaces.h"

#define N_DEVICES     (5)
#define N_EVENTS      (160)
#define N_GENERATORS  (6)
#define BUF_SIZE      (8)

int main()
{
  bool is_modeling = true;
  struct MassServiceSystem* mss = new_mss(N_DEVICES, BUF_SIZE);
  struct EventCalendar* calendar = new_calendar(N_EVENTS);
  struct Environment* env = new_env(N_GENERATORS);

  generate_requests(env, calendar);

  while(is_modeling)
  {
    struct Event event = get_next_event(calendar);
    struct Request request;

    switch(event.type)
    {
      case GET_REQUEST:
        request = event.data.request;
        generate_request_for(request.gen_number, calendar);
        int device_index = select_device(mss);
        if (device_index >= 0)
        {
            serve_a_request(&request, &env->generators[device_index]);
        }
        else
        {
          buffer_insert(mss->buffer, &request);
        }
        break;
      case DEVICE_FREE:
        int err;
        buffer_extract(mss->buffer, &request, &err);
        if (err = BUFFER_OK)
        {
          int device_index = select_device(mss);
          if (device_index >= 0)
          {
              serve_a_request(&request, &env->generators[device_index]);
          }
        }
        break;
      case STOP_MODELING:
        is_modeling = false;
        break;
    }
  }

  return 0;
}

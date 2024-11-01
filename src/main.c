#include <stdbool.h>

#include "types.h"
#include "interfaces.h"

int main()
{
  struct MassServiceSystem mss = new_mss();
  bool is_modeling = true;

  generate_requests();

  while(is_modeling)
  {
    struct Event event = get_next_event();
    struct Request request;

    switch(event.type)
    {
      case GET_EVENT:
        request = *(struct Request*)event.data;
        generate_request_for(request.gen_number);
        if (have_free_device())
        {
          select_device(&mss);
          serve_a_request();
        }
        else
        {
          buffer_insert(&mss.buffer, &request);
        }
        break;
      case DEVICE_FREE:
        int err;
        buffer_extract(&mss.buffer, &request, &err);
        if (err = BUFFER_OK)
        {
          serve_a_request();
        }
        break;
      case STOP_MODELING:
        is_modeling = false;
        break;
    }
  }

  return 0;
}

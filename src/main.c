#include "types.h"
#include "interfaces.h"

int main()
{
  generate_requests();

  for(;;)
  {
    struct Event next_event = get_next_event();

    switch(next_event.type)
    {
      case GET_EVENT:
        break;
      case DEVICE_FREE:
        break;
      case STOP_MODELING:
        break;
    }
  }

  return 0;
}

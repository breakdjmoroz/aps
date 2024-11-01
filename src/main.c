#include "types.h"
#include "interfaces.h"

int main()
{
  generate_requests();

  for(;;)
  {
    Event next_event = get_next_event();

    switch(next_event)
    {
      case get_request:
        break;
      case device_is_free:
        break;
      case stop_modeling:
        break;
      default:
        break;
    }
  }

  return 0;
}

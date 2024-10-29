#include <stddef.h>
#include "interfaces.h"

int buffer_insert(struct Buffer const* buffer, const struct Request const* request)
{
  int err = 0;
  struct Request* rejected_request;

  buffer_insert_with_rejected(buffer, request, rejected_request, &err);

  free(rejected_request);

  return err;
}

void buffer_insert_with_rejected(struct Buffer const* buffer, const struct Request const* in_request,
    struct Request const* rejected_request, int const* err_num)
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

    buffer->requests[current_index] = in_request;

    current_index = (current_index + 1) % buffer->size;
  }
  if (err_num != NULL)
  {
    *err_num = err;
  }
}

void buffer_extract(struct Buffer const* buffer, struct Request const* request, int* err_num)
{
  struct Request tmp_request = NULL;
  u32 max_priority = 0;
  u32 current_priority = 0;
  size_t request_index = 0;
  int err = 0;
  size_t i;

  if (buffer == NULL || request == NULL)
  {
    err = -1;
  }

  for (i = 0; i < buffer->size; ++i)
  {
    current_priority =
      buffer->requests[(buffer->current_index + i) % buffer->size].gen_number;

    if (current_priority > max_priority)
    {
      max_priority = current_priority;
      request_index = (buffer->current_index + i) % buffer->size;
    }
    else if (current_priority == max_priority)
    {
      //TODO: select the oldest request as the most priorited
    }
  }

  request = buffer->requests[request_index];

  if (err_num != NULL)
  {
    *err_num = err;
  }
}


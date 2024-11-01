#ifndef INTERFACE
#define INTERFACE

#include "types.h"

void buffer_insert_with_rejected(
    struct Buffer* const,const struct Request* const,
    struct Request*, int* const
    );
int buffer_insert(struct Buffer* const, const struct Request* const);
void buffer_extract(struct Buffer* const, struct Request*, int* const);

size_t select_device(const struct MassServiceSystem* const);

struct Event get_next_event();
void generate_requests();

#endif

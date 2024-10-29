#ifndef INTERFACE
#define INTERFACE

#include "types.h"

void buffer_insert_with_rejected(
    struct Buffer const*, const struct Request const*,
    struct Request const*, int const*
    );
int buffer_insert(struct Buffer const*, const struct Request const*);
void buffer_extract(struct Buffer const*, struct Request const*, int*);

size_t select_device(const struct MassServiceSystem const*);

#endif

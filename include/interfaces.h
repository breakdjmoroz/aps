#ifndef INTERFACE
#define INTERFACE

#include <stdbool.h>
#include "types.h"

//TODO: change this value if need
#define BUFFER_OK     (0)

void buffer_insert_with_rejected(
    struct Buffer* const,const struct Request* const,
    struct Request*, int* const
    );
int buffer_insert(struct Buffer* const, const struct Request* const);
void buffer_extract(struct Buffer* const, struct Request*, int* const);

size_t select_device(const struct MassServiceSystem* const);

struct MassServiceSystem* new_mss(size_t, size_t);
struct EventCalendar* new_calendar(size_t);
struct Environment* new_env(size_t);

void generate_requests(const struct Environment* const, struct EventCalendar*);

struct Event get_next_event();
void generate_request_for(u32);

bool have_free_device();

void serve_a_request();

void insert_event(struct EventCalendar*, struct Event*);

#endif

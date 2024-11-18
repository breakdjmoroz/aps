#ifndef INTERFACE
#define INTERFACE

#include <time.h>
#include <stdbool.h>
#include "types.h"

extern const struct Request EMPTY_REQUEST;

extern double global_current_time;

bool is_equal_requests(struct Request, struct Request);
bool is_equal_events(struct Event, struct Event);

void buffer_insert_with_rejected(
    struct Buffer* const, struct Request*,
    struct Request*, int* const
    );
int buffer_insert(struct Buffer* const, struct Request*);
void buffer_extract(struct Buffer* const, struct Request*, int* const);

int select_device(const struct MassServiceSystem* const);

struct MassServiceSystem* new_mss(size_t, size_t);
struct EventCalendar* new_calendar(size_t, size_t);
struct Environment* new_env(size_t);

struct Event get_next_event(const struct EventCalendar* const, bool*);
void generate_request_for(u32, struct EventCalendar*, struct Request*);

void serve_a_request(struct Request*, struct Device*, struct EventCalendar*);

void insert_event(struct EventCalendar*, struct Event*);

void print_calendar(struct EventCalendar*);

#endif

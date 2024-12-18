#ifndef INTERFACE
#define INTERFACE

#include <time.h>
#include <stdbool.h>
#include "types.h"

extern double global_current_time;

void create_break_event(struct EventCalendar*, double);
bool is_stop_modeling(struct Event);

bool is_empty_request(struct Request);

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

void delete_mss(struct MassServiceSystem*);
void delete_calendar(struct EventCalendar*);
void delete_env(struct Environment*);

struct Event get_next_event(const struct EventCalendar* const, bool*);
void generate_request_for(u32, struct EventCalendar*, struct Request*);

void serve_a_request(struct Request*, struct Device*, struct EventCalendar*);

void insert_event(struct EventCalendar*, struct Event*);

void print_calendar(const struct EventCalendar* const);
void print_buffer(const struct Buffer* const);

#endif

#ifndef STATISTIC
#define STATISTIC

#include "types.h"

#define REJECTED_REQUEST  (0)
#define SERVED_REQUEST    (1)

struct StatisticTable
{
  struct Request* statistics;
  size_t statistics_len;
  u32* average_waiting_time;
  u32* average_serving_time;
  u32* average_total_time;
  size_t generators_num;
  u32* average_total_time_device;
  size_t devices_num;
  u32 total_realization_time;
};

struct StatisticTable* new_stat(size_t, size_t, size_t);

void start_statistic(struct StatisticTable*);
void collect_statistic(struct StatisticTable*, struct Request*, int);
void stop_statistic(struct StatisticTable*);

void print_statistic(struct StatisticTable*);

#endif

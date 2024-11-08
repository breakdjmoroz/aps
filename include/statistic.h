#ifndef STATISTIC
#define STATISTIC

#include "types.h"

#define REJECTED_REQUEST  (0)
#define SERVED_REQUEST    (1)
#define GENERATED_REQUEST (2)

struct Statistic
{
};

struct StatisticTable
{
  struct Statistic* statistics;
  size_t statistics_len;
  double* average_waiting_time;
  double* average_serving_time;
  double* average_total_time;
  size_t generators_num;
  double* average_total_time_device;
  size_t devices_num;
  double total_realization_time;
};

struct StatisticTable* new_stat(size_t, size_t, size_t);

void start_statistic();
void collect_statistic(struct StatisticTable*, struct Request*, int);
void stop_statistic(struct StatisticTable*);

void print_statistic(struct StatisticTable*);

#endif

#ifndef STATISTIC
#define STATISTIC

#include "types.h"

#define REJECTED_REQUEST  (0)
#define SERVED_REQUEST    (1)
#define HALTED_DEVICE     (2)

struct GeneratorStatistic
{
  int total_amount;
  int rejected_amount;
  double average_waiting_time;
  double average_serving_time;
  double average_total_time;
};

struct DeviceStatistic
{
  double average_total_time;
};

struct StatisticTable
{
  struct GeneratorStatistic* generators;
  size_t generators_num;
  struct DeviceStatistic* devices;
  size_t devices_num;
  double total_realization_time;
};

struct StatisticTable* new_stat(size_t, size_t);

void start_statistic();
void collect_statistic(struct StatisticTable*, struct Request*, int);
void stop_statistic(struct StatisticTable*);

void print_statistic(struct StatisticTable*);

#endif

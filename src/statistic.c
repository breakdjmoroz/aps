#include <time.h>
#include <stdlib.h>

#include "statistic.h"

extern time_t global_start_time;
extern time_t global_current_time;
extern time_t global_end_time;

struct StatisticTable* new_stat(size_t statistics_len, size_t generators_num, size_t devices_num)
{
  struct StatisticTable* new_stat = (struct StatisticTable*)malloc(sizeof(struct StatisticTable));
  if (new_stat != NULL)
  {
    new_stat->statistics_len = statistics_len;
    new_stat->generators_num = generators_num;
    new_stat->devices_num = devices_num;

    new_stat->statistics = (struct Statistic*)malloc(sizeof(struct Request) * statistics_len);
    new_stat->average_waiting_time = (double*)malloc(sizeof(double) * generators_num);
    new_stat->average_serving_time = (double*)malloc(sizeof(double) * generators_num);
    new_stat->average_total_time = (double*)malloc(sizeof(double) * generators_num);
    new_stat->average_total_time_device = (double*)malloc(sizeof(double) * devices_num);
    
    if (!new_stat->statistics || !new_stat->average_waiting_time ||
        !new_stat->average_serving_time || !new_stat->average_total_time ||
        !new_stat->average_total_time_device)
    {
      free(new_stat->statistics);
      free(new_stat->average_waiting_time);
      free(new_stat->average_serving_time);
      free(new_stat->average_total_time);
      free(new_stat->average_total_time_device);
      free(new_stat);
      new_stat = NULL;
    }

    new_stat->total_realization_time = 0.0;

  }
  return new_stat;
}

void start_statistic()
{
  global_start_time = clock();
}

void collect_statistic(struct StatisticTable*, struct Request*, int)
{
}

void stop_statistic(struct StatisticTable* stat)
{
  global_end_time = clock();
  stat->total_realization_time = (double)(global_end_time - global_start_time);
}

void print_statistic(struct StatisticTable*)
{
}

#include <stdlib.h>

#include "statistic.h"

struct StatisticTable* new_stat(size_t statistics_len, size_t generators_num, size_t devices_num)
{
  struct StatisticTable* new_stat = (struct StatisticTable*)malloc(sizeof(struct StatisticTable));
  if (new_stat != NULL)
  {
    new_stat->statistics_len = statistics_len;
    new_stat->generators_num = generators_num;
    new_stat->devices_num = devices_num;

    new_stat->statistics = (struct Request*)malloc(sizeof(struct Request) * statistics_len);
    new_stat->average_waiting_time = (u32*)malloc(sizeof(u32) * generators_num);
    new_stat->average_serving_time = (u32*)malloc(sizeof(u32) * generators_num);
    new_stat->average_total_time = (u32*)malloc(sizeof(u32) * generators_num);
    new_stat->average_total_time_device = (u32*)malloc(sizeof(u32) * devices_num);
    
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

    new_stat->total_realization_time = 0u;

  }
  return new_stat;
}

void start_statistic(struct StatisticTable*)
{
}

void collect_statistic(struct StatisticTable*, struct Request*, int)
{
}

void stop_statistic(struct StatisticTable*)
{
}

void print_statistic(struct StatisticTable*)
{
}
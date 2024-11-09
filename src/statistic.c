#include <time.h>
#include <stdlib.h>

#include "statistic.h"

extern clock_t global_start_time;
extern double global_current_time;
extern clock_t global_end_time;

struct StatisticTable* new_stat(size_t generators_num, size_t devices_num)
{
  struct GeneratorStatistic* generators;
  struct DeviceStatistic* devices;
  struct StatisticTable* stat = (struct StatisticTable*)malloc(sizeof(struct StatisticTable));
  if (stat != NULL)
  {
    generators = (struct GeneratorStatistic*)malloc(sizeof(struct GeneratorStatistic) * generators_num);
    devices = (struct DeviceStatistic*)malloc(sizeof(struct DeviceStatistic) * devices_num);

    if (generators != NULL && devices != NULL)
    {
      stat->generators = generators;
      stat->devices = devices;
      stat->generators_num = 0;
      stat->devices_num = 0;
      stat->total_realization_time = 0.0;
    }
    else
    {
      free(generators);
      free(devices);
      free(stat);
    }
  }
  return stat;
}

void start_statistic(struct StatisticTable* stat)
{
  size_t i = 0;

  for(i = 0; i < stat->generators_num; ++i)
  {
   stat->generators[i].total_amount = 0;
   stat->generators[i].rejected_amount = 0;
   stat->generators[i].average_waiting_time = 0.0;
   stat->generators[i].average_serving_time = 0.0;
   stat->generators[i].average_total_time = 0.0;
  }
  for(i = 0; i < stat->devices_num; ++i)
  {
    stat->devices[i].average_total_time = 0.0;
  }
  
  global_start_time = clock();
}

void collect_statistic(struct StatisticTable* stat, struct Request* request, int mode)
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

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "statistic.h"

extern double global_current_time;

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
      stat->generators_num = generators_num;
      stat->devices_num = devices_num;
      stat->total_realization_time = 0.0;
    }
    else
    {
      free(generators);
      free(devices);
      free(stat);
      stat = NULL;
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
}

void collect_statistic(struct StatisticTable* stat, struct Request* request, int mode)
{
  if (stat != NULL && request != NULL)
  {
    double waiting_time = 0.0;
    double serving_time = 0.0;
    double total_time = 0.0;
    switch (mode)
    {
      case SERVED_REQUEST:
        if (request->buf_time > 0)
        {
          waiting_time = request->buf_time - request->gen_time;
          serving_time = request->dev_time - request->buf_time;
        }
        else
        {
          waiting_time = 0.0;
          serving_time = request->dev_time - request->gen_time;
        }

        total_time = waiting_time + serving_time;

        stat->generators[request->gen_number].total_amount += 1;
        stat->generators[request->gen_number].average_waiting_time
          += waiting_time;
        stat->generators[request->gen_number].average_serving_time
          += serving_time;
        stat->generators[request->gen_number].average_total_time
          += total_time;
        break;
      case REJECTED_REQUEST:
        waiting_time = request->buf_time - request->gen_time;
        total_time = waiting_time;
        stat->generators[request->gen_number].total_amount += 1;
        stat->generators[request->gen_number].rejected_amount += 1;
        stat->generators[request->gen_number].average_waiting_time
          += waiting_time;
        stat->generators[request->gen_number].average_total_time
          += total_time;
        break;
      case HALTED_DEVICE:
        /*TODO: insert definition */
        break;
    }
  }
}

void collect_statistic_device(struct StatisticTable* stat, struct Device* device, size_t number)
{
  stat->devices[number].average_total_time += global_current_time - device->use_time;
}

void stop_statistic(struct StatisticTable* stat)
{
  stat->total_realization_time = global_current_time;
}

void print_statistic(struct StatisticTable* stat)
{
  size_t i = 0;
  double total_amount = 0.0;
  double waiting_time = 0.0;
  double serving_time = 0.0;
  double total_time = 0.0;
  double reject_probability = 0.0;
  double realization_time = stat->total_realization_time;

  printf("=================GENERATORS_STATISTIC=================\n");
  printf("|gen_№|total|p_reject|avr(wait)|avr(serve)|avr(total)|\n");
  printf("|-----+-----+--------+---------+----------+----------|\n");

  for (i = 0; i < stat->generators_num; ++i)
  {
    total_amount = (double)(stat->generators[i].total_amount);
    waiting_time = stat->generators[i].average_waiting_time
      / total_amount;
    serving_time = stat->generators[i].average_serving_time
      / total_amount;
    total_time = stat->generators[i].average_total_time
      / total_amount;

    reject_probability = (double)stat->generators[i].rejected_amount
      / total_amount;

    /*TODO: calculate dispersia of waiting_time and serving_time*/

    printf(
        "|%5d|%5d|%6lf|%9lf|%10lf|%10lf|\n",
        i,
        (int)total_amount,
        reject_probability,
        waiting_time,
        serving_time,
        total_time
        );
  }

  printf("======================================================\n");

  printf("\n");

  printf("====DEVICES_STATISTIC===\n");
  printf("|device_num|usage_ratio|\n");
  printf("|----------+-----------|\n");

  for (i = 0; i < stat->devices_num; ++i)
  {
    printf(
        "|%10d|%11lf|\n",
        i,
        stat->devices[i].average_total_time / realization_time
        );
  }

  printf("========================\n");
}

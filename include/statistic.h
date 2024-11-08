#ifndef STATISTIC
#define STATISTIC

#include "types.h"

#define REJECTED_REQUEST  (0)
#define SERVED_REQUEST    (1)

struct StatisticTable
{
};

struct StatisticTable* new_stat();

void start_statistic(struct StatisticTable*);
void collect_statistic(struct StatisticTable*, struct Request*, int);
void stop_statistic(struct StatisticTable*);

void print_statistic(struct StatisticTable*);

#endif

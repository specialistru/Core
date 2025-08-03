#pragma once
#include "types.h"
#include "enums.h"

// Представление ошибки
typedef struct {
  status_t code;
  const char *message;
  const char *function;
  int line;
} error_t;

// Диапазон времени
typedef struct {
  timestamp_ns_t start;
  timestamp_ns_t end;
} time_range_t;

// Событие трассировки
typedef struct {
  const char *label;
  timestamp_ns_t timestamp;
  const char *context;
} trace_event_t;

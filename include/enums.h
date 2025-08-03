#pragma once

// Коды статуса
typedef enum {
  STATUS_OK = 0,
  STATUS_ERR = 1,
  STATUS_INVALID_ARGUMENT = 2,
  STATUS_NOT_FOUND = 3,
  STATUS_TIMEOUT = 4,
  STATUS_CONFLICT = 5,
  STATUS_RETRY = 6,
  STATUS_CANCELLED = 7,
  STATUS_UNSUPPORTED = 8
} status_t;

// Уровни логирования
typedef enum {
  LOG_DEBUG = 0,
  LOG_INFO = 1,
  LOG_WARN = 2,
  LOG_ERROR = 3,
  LOG_FATAL = 4
} log_level_t;

// Типы плана
typedef enum {
  PLAN_TYPE_SELECT,
  PLAN_TYPE_INSERT,
  PLAN_TYPE_UPDATE,
  PLAN_TYPE_DELETE,
  PLAN_TYPE_MERGE,
  PLAN_TYPE_JOIN
} plan_type_t;

// Стратегии соединения
typedef enum {
  JOIN_HASH,
  JOIN_MERGE,
  JOIN_NESTED_LOOP,
  JOIN_GRACE_HASH
} join_strategy_t;

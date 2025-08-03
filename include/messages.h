#pragma once

// ERROR
#define ERR_MSG_ALLOC_FAILED       "Allocation failed"
#define ERR_MSG_INVALID_ARGUMENT   "Invalid argument"
#define ERR_MSG_NOT_FOUND          "Entity not found"
#define ERR_MSG_TIMEOUT            "Operation timed out"
#define ERR_MSG_CONFLICT           "Transaction conflict"
#define ERR_MSG_UNSUPPORTED        "Operation not supported"

// WARN
#define WARN_MSG_PLAN_SUBOPTIMAL   "Plan may be suboptimal"
#define WARN_MSG_GC_DELAYED        "GC cycle delayed due to load"

// INFO
#define INFO_MSG_PLAN_CACHED       "Execution plan reused"
#define INFO_MSG_PAGE_EVICTED      "Page evicted to tiered storage"

// DEBUG
#define DBG_MSG_TRACE_START        "Trace started for block"
#define DBG_MSG_METRIC_EMITTED     "Metric emitted from module"
#define DBG_MSG_REPLAN_TRIGGERED   "Replanning triggered"

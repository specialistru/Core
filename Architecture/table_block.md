| ID  | Название блока                | Внутренний идентификатор      |
| --- | ----------------------------- | ----------------------------- |
| 0.1 | Точка входа (main)            | main_entrypoint             |
| 0.2 | Загрузка конфигурации         | config_loader               |
| 0.3 | Инициализация логирования     | log_init                    |
| 0.4 | Инициализация подсистем       | subsystem_initializer       |
| 0.5 | Контроллер запуска и останова | startup_shutdown_controller |
| 0.6 | CLI-интерфейс администратора  | admin_cli_entrypoint        |
| 1.1  | Row Store (строчно-ориентированное хранение) | row_store              |
| 1.2  | Column Store (колоночное хранение)           | column_store           |
| 1.3  | JSON Store                                   | json_store             |
| 1.4  | Слой абстракции хранилища (Storage Layer)    | storage_layer          |
| 1.5  | Tiered Storage (In-Memory / NVMe / SSD)      | tiered_storage         |
| 1.6  | NUMA-aware Allocation                        | numa_allocator         |
| 1.7  | CPU Affinity / Cache Coloring                | cpu_cache_binding      |
| 1.8  | Prefetching Engine                           | prefetch_engine        |
| 1.9  | Sharding Manager                             | sharding_manager       |
| 1.10 | Partitioning Engine                          | partitioning_engine    |
| 1.11 | Auto Rebalancer                              | auto_rebalancer        |
| 1.12 | Distributed Planner                          | distributed_planner    |
| 1.13 | TTL Manager                                  | ttl_engine             |
| 1.14 | Eviction Manager                             | eviction_engine        |
| 1.15 | Auto-tiering Engine                          | auto_tiering_engine    |
| 1.16 | Поддержка большого объема памяти             | large_memory_support   |
| 1.17 | Ограничения по CPU/IOPS/Memory (per session) | resource_limits        |
| 1.18 | PAX / Apache Arrow Layouts                   | pax_arrow_engine       |
| 1.19 | Frame-of-Reference + Векторное сжатие        | frameofref_compression |
| 1.20 | Page Cache / Buffer Pool                     | buffer_pool            |
| 1.21 | Dirty Page Tracking                          | dirty_tracking         |
| 1.22 | Page Preload                                 | page_preloader         |
| 1.23 | Слой индексирования                          | index_layer            |
| 1.24 | B+ Tree Index                                | bplus_index            |
| 1.25 | Hash Index                                   | hash_index             |
| 1.26 | Bitmap Index                                 | bitmap_index           |
| 1.27 | Inverted Index                               | inverted_index         |
| 1.28 | Adaptive Indexing                            | adaptive_index         |
| 1.29 | Dictionary Compression                       | dict_compression       |
| 1.30 | Run-Length Encoding (RLE)                    | rle_compression        |
| 1.31 | Delta Encoding                               | delta_compression      |
| 1.32 | Vectorized Decompression                     | vector_decompressor    |
| 1.33 | Storage Metadata Engine                      | storage_metadata       |
| 1.34 | Datafile Format Layer                        | datafile_layer         |
| 1.35 | Hot/Cold Data Separator                      | hotcold_separator      |
| 1.36 | Auto-Tiering Policy Engine                   | tiering_policy_engine  |
| 1.37 | Table Catalog                                | table_catalog          |
| 1.38 | Table Engine (Insert/Delete/Update)          | table_engine           |
| 1.39 | Table Iterator Engine                        | table_iter_engine      |
| 2.1  | MVCC (Compressed Chains + Snapshot Isolation)         | mvcc_engine                |
| 2.2  | ACID-совместимость                                    | acid_compliance            |
| 2.3  | WAL (Write-Ahead Logging)                             | wal_engine                 |
| 2.4  | Snapshot Recovery                                     | snapshot_recovery          |
| 2.5  | Уровни изоляции транзакций (RC, RR, SI, SERIALIZABLE) | isolation_levels           |
| 2.6  | Transaction Coordinator / Manager                     | transaction_manager        |
| 2.7  | Undo/Redo логи                                        | undo_redo_logs             |
| 2.8  | Transaction Context per Session                       | tx_context                 |
| 2.9  | Transaction Metadata Table                            | tx_metadata_table          |
| 2.10 | Transaction Conflict Resolver                         | tx_conflict_resolver       |
| 2.11 | Version Chain Garbage Collector                       | tx_gc                      |
| 2.12 | Deadlock Detector                                     | deadlock_detector          |
| 2.13 | Two-Phase Commit (2PC)                                | 2pc_engine                 |
| 2.14 | Raft / Paxos Consensus for Replication                | consensus_replication      |
| 2.15 | Active-Active Replication                             | active_active_replication  |
| 2.16 | Active-Passive Replication                            | active_passive_replication |
| 2.17 | Multi-node Crash Recovery                             | multi_node_recovery        |
| 2.18 | Watchdog / Failover Service                           | watchdog_service           |
| 2.19 | Distributed Transaction Engine (XA / global tx)       | distributed_tx_engine      |
| 2.20 | Background Vacuum / Cleaner                           | tx_background_vacuum       |
| 2.21 | Timestamp Oracle / TSO                                | timestamp_oracle           |
| 2.22 | Snapshot Builder (MVCC snapshots per query/session)   | snapshot_builder           |
| 2.23 | Serializable Fallback Engine                          | serializable_fallback      |
| 2.24 | Log Compaction                                        | wal_log_compaction         |
| 2.25 | Transaction Audit / Journal                           | tx_audit_log               |
| 3.1  | SQL Ядро (ANSI SQL:2011+)                             | sql_core                |
| 3.2  | Парсер SQL                                            | sql_parser              |
| 3.3  | AST (Abstract Syntax Tree)                            | sql_ast                 |
| 3.4  | Планировщик запросов (Logical Planner)                | query_planner           |
| 3.5  | Оптимизатор: Cost-Based                               | optimizer_cost          |
| 3.6  | Оптимизатор: Rule-Based                               | optimizer_rule          |
| 3.7  | Оптимизатор: Adaptive / Reoptimization                | optimizer_adaptive      |
| 3.8  | Поддержка оконных функций (OVER/PARTITION BY/...)     | window_functions        |
| 3.9  | Поддержка recursive CTE                               | recursive_cte           |
| 3.10 | Поддержка LATERAL и UNNEST                            | sql_lateral             |
| 3.11 | MERGE и UPSERT операции                               | sql_merge_upsert        |
| 3.12 | Расширения: JSON-функции                              | sql_json_ext            |
| 3.13 | Расширения: FILTER, агрегаты и статистические функции | sql_aggregates_ext      |
| 3.14 | Расширения: MATCH\\_RECOGNIZE                          | sql_match_recognize     |
| 3.15 | UDF на Lua                                            | udf_lua                 |
| 3.16 | UDF на WASM                                           | udf_wasm                |
| 3.17 | UDF на JS / C / C++                                   | udf_native              |
| 3.18 | Сandbox для UDF                                       | udf_sandbox             |
| 3.19 | Система пользовательских агрегатов (UDAF)             | udaf_engine             |
| 3.20 | Встроенный DSL для транзакций (в стиле PL/SQL/ABAP)   | transaction_dsl         |
| 3.21 | Поддержка процедур и триггеров                        | sql_procedures_triggers |
| 3.22 | Execution Engine SQL                                  | sql_execution_engine    |
| 4.1  | JDBC-драйвер                               | jdbc_driver            |
| 4.2  | ODBC-драйвер                               | odbc_driver            |
| 4.3  | CLI-интерфейс                              | cli_interface          |
| 4.4  | REST API (OpenAPI)                         | rest_api               |
| 4.5  | gRPC API                                   | grpc_api               |
| 4.6  | WebSocket API                              | websocket_api          |
| 4.7  | GraphQL API                                | graphql_api            |
| 4.8  | JSON\\:API                                  | json_api_spec          |
| 4.9  | OpenAPI генератор схем                     | openapi_generator      |
| 4.10 | Webhooks Engine                            | webhook_engine         |
| 4.11 | Внешние процедуры (External Function Call) | external_procedure     |
| 4.12 | Async Event Processor                      | event_processor        |
| 4.13 | CDC (Change Data Capture)                  | cdc_engine             |
| 4.14 | Поддержка Kafka                            | kafka_connector        |
| 4.15 | Поддержка Apache Pulsar                    | pulsar_connector       |
| 4.16 | Поддержка Debezium                         | debezium_adapter       |
| 4.17 | Поддержка Redis Streams                    | redis_streams_adapter  |
| 4.18 | Поддержка S3 и внешних хранилищ            | s3_storage_adapter     |
| 4.19 | Интеграция с ERP/BI/BPM системами          | erp_bi_integration     |
| 5.1  | HTAP Engine (Hybrid Transactional/Analytical Processing) | htap_engine              |
| 5.2  | Виртуальные представления (Views)                        | sql_views                |
| 5.3  | Материализованные представления / Кубы                   | materialized_views       |
| 5.4  | Адаптивные агрегаты                                      | adaptive_aggregates      |
| 5.5  | ROLLUP, CUBE, GROUPING SETS                              | sql_rollup_cube          |
| 5.6  | Time-Series Engine (встроенные типы)                     | timeseries_engine        |
| 5.7  | Gap-Fill / Interpolation                                 | ts_gapfill_interpolation |
| 5.8  | Downsampling / Aggregation for TS                        | ts_downsampling          |
| 5.9  | Retention Policy Manager                                 | ts_retention_policy      |
| 5.10 | ML Inference Engine (ONNX, TensorFlow, PyTorch)          | ml_inference_engine      |
| 5.11 | SQL-интерфейс к ML-моделям                               | ml_sql_inference         |
| 5.12 | Поддержка ONNX Runtime                                   | ml_onnx_runtime          |
| 5.13 | Поддержка TensorFlow Runtime                             | ml_tensorflow_runtime    |
| 5.14 | Поддержка PyTorch Runtime                                | ml_pytorch_runtime       |
| 5.15 | Функции ранжирования, percentiles, correlation           | ml_sql_statistics        |
| 5.16 | Встроенные алгоритмы: кластеризация, прогнозирование     | ml_builtins              |
| 5.17 | SQL Scheduler для аналитических джобов                   | sql_job_scheduler        |
| 6.1  | RBAC (Role-Based Access Control)              | rbac_engine              |
| 6.2  | ABAC (Attribute-Based Access Control)         | abac_engine              |
| 6.3  | Row-Level Security                            | row_level_security       |
| 6.4  | Column-Level Security                         | column_level_security    |
| 6.5  | TLS / mTLS                                    | tls_stack                |
| 6.6  | JWT / Token-Based Auth                        | jwt_auth                 |
| 6.7  | Transparent Data Encryption (TDE)             | tde_engine               |
| 6.8  | Forward Secrecy                               | forward_secrecy          |
| 6.9  | ACL и сетевые правила                         | acl_rules                |
| 6.10 | Scoped Tokens                                 | scoped_tokens            |
| 6.11 | DML / DDL Audit Logging                       | audit_log_dml_ddl        |
| 6.12 | Immutable Audit Log                           | immutable_audit_log      |
| 6.13 | Access Policy Engine                          | access_policy_engine     |
| 6.14 | Защита от SQL-инъекций и sandbox для Lua/WASM | sql_injection_protection |
| 7.1  | Метрики (Prometheus, OpenMetrics)                    | metrics_engine            |
| 7.2  | IOPS / Latency / Query Duration Counters             | performance_counters      |
| 7.3  | Наносекундный трекинг времени                        | nanoscale_timing          |
| 7.4  | Структурированное логирование (JSON, syslog, binary) | structured_logger         |
| 7.5  | OpenTelemetry / Jaeger / Zipkin Integration          | tracing_exporter          |
| 7.6  | Query Tracing и Timeline Viewer                      | query_timeline_viewer     |
| 7.7  | Flamegraph Viewer                                    | flamegraph_viewer         |
| 7.8  | Трассировка выполнения запросов по плану             | execution_tracer          |
| 7.9  | Встроенный профилировщик                             | builtin_profiler          |
| 7.10 | CLI-инструменты диагностики                          | cli_diagnostics           |
| 7.11 | REST-интерфейс мониторинга                           | rest_monitoring_api       |
| 7.12 | Веб-панель наблюдаемости                             | web_monitoring_ui         |
| 7.13 | Интеграция с Alertmanager                            | alertmanager_hook         |
| 7.14 | Fuzzing Engine (libFuzzer, AFL++)                    | fuzzing_engine            |
| 7.15 | ASAN / TSAN / MSAN (Sanitizers)                      | sanitizer_hooks           |
| 7.16 | Fault Injection Framework                            | fault_injection_framework |
| 7.17 | Chaos Testing Controller                             | chaos_testing_controller  |
| 7.18 | Crash Recovery Validator                             | crash_recovery_validator  |
| 7.19 | CI/CD Pipeline Hooks                                 | cicd_hooks                |
| 7.20 | Mutation Testing Framework                           | mutation_testing          |
| 8.1  | Vectorized Execution Engine                        | vectorized_execution    |
| 8.2  | Параллельные пайплайны выполнения                  | parallel_pipelines      |
| 8.3  | Streaming Aggregator                               | streaming_aggregator    |
| 8.4  | Adaptive Hash Join                                 | adaptive_hash_join      |
| 8.5  | Merge Join                                         | merge_join              |
| 8.6  | Index-Nested Loop Join                             | index_nested_loop_join  |
| 8.7  | Grace Hash Join                                    | grace_hash_join         |
| 8.8  | Runtime Re-optimization                            | runtime_reoptimization  |
| 8.9  | Plan Caching                                       | plan_cache              |
| 8.10 | Selectivity Feedback Loop                          | selectivity_feedback    |
| 8.11 | Copy-on-Write Snapshots                            | copy_on_write_snapshots |
| 8.12 | Instant Backup / Restore                           | instant_backup_restore  |
| 8.13 | Clone Tables / Table Snapshots                     | table_clone_snapshots   |
| 8.14 | PAX Layout Support                                 | pax_layout_support      |
| 8.15 | Apache Arrow Layout Execution                      | arrow_layout_engine     |
| 8.16 | Format-aware Execution Pipeline                    | format_aware_executor   |
| 8.17 | Execution Plan Inliner                             | execution_plan_inliner  |
| 8.18 | JIT Engine                                         | jit_engine              |
| 8.19 | In-Memory Adaptive Buffers                         | adaptive_buffers        |
| 8.20 | Hot Operator Detection                             | hot_operator_detector   |
| 8.21 | Critical Path Analyzer (Latency < 500ns Optimizer) | critical_path_optimizer |
| 9.1  | Поддержка UDF (пользовательских функций)             | udf_support            |
| 9.2  | Поддержка UDAF (агрегатных пользовательских функций) | udaf_support           |
| 9.3  | Расширения на C/C++                                  | c_cpp_extensions       |
| 9.4  | Внешние интерфейсы: REST, gRPC и JSON\\:API           | ext_api_interfaces     |
| 9.5  | Песочница для Lua/WASM расширений                    | udf_sandbox_engine     |
| 9.6  | Загрузка внешних модулей (dlopen / LoadLibrary)      | dynamic_module_loader  |
| 9.7  | SQL-протокол: собственный бинарный формат            | binary_sql_protocol    |
| 9.8  | Совместимость с PostgreSQL wire protocol             | pgsql_wire_compat      |
| 9.9  | Расширяемый SQL API                                  | sql_api_extension      |
| 9.10 | JSON DSL и сценарии для ETL                          | etl_json_dsl           |
| 9.11 | ERP DSL (доменно-специфичный язык для ERP сценариев) | erp_dsl_engine         |
| 9.12 | Генератор ETL-процессов                              | etl_generator          |
| 9.13 | Планировщик пользовательских задач                   | custom_task_scheduler  |
| 9.14 | Расширения для внешних BI/ML/Streaming систем        | bi_ml_streaming_ext    |
| 9.15 | Интерфейс расширения ядра (внутренние хуки и API)    | core_extension_hooks   |
| 10.1  | Unit-тесты                                                 | unit_test_engine          |
| 10.2  | Интеграционные тесты                                       | integration_test_engine   |
| 10.3  | Fuzz-тестирование (на AST, SQL, транзакциях, бинарных API) | fuzz_testing_engine       |
| 10.4  | Стресс-тестирование (нагрузочные сценарии)                 | stress_testing_engine     |
| 10.5  | Soak-тестирование (длительные сценарии)                    | soak_testing_engine       |
| 10.6  | Мутационное тестирование                                   | mutation_testing_engine   |
| 10.7  | Покрытие кода (coverage engine)                            | coverage_engine           |
| 10.8  | Тестирование безопасности                                  | security_test_suite       |
| 10.9  | Проверка консистентности после краша                       | crash_consistency_checker |
| 10.10 | Инструменты CI/CD тестирования                             | ci_cd_test_hooks          |
| 10.11 | Анализ качества схемы / миграций                           | schema_migration_checker  |
| 11.1  | Планировщик задач (Task Scheduler)                    | task_scheduler         |
| 11.2  | Очередь отложенных заданий (Delay Queue)              | delay_queue_engine     |
| 11.3  | Cron-подобный триггер                                 | cron_trigger           |
| 11.4  | Планировщик фоновых задач (Background Job Engine)     | background_job_engine  |
| 11.5  | Очередь транзакций (Transaction Queue)                | tx_queue               |
| 11.6  | Планировщик аналитических джобов (OLAP Job Scheduler) | olap_job_scheduler     |
| 11.7  | Планировщик ETL задач                                 | etl_job_scheduler      |
| 11.8  | Планировщик SLA / Policy Based                        | policy_scheduler       |
| 11.9  | Поддержка приоритетов / QoS                           | priority_qos_manager   |
| 11.10 | Механизм метаданных для задач                         | task_metadata_engine   |
| 12.1  | Кластерный менеджер                                       | cluster_manager                |
| 12.2  | Балансировщик нагрузки (Load Balancer)                    | load_balancer                  |
| 12.3  | Поддержка шардирования (Sharding Layer)                   | sharding_layer                 |
| 12.4  | Поддержка re-sharding / Auto-rebalancing                  | resharding_engine              |
| 12.5  | Каталог кластерных узлов                                  | node_catalog                   |
| 12.6  | Согласование через Raft / Paxos                           | consensus_protocol             |
| 12.7  | Управление распределёнными транзакциями (XA / 2PC / Saga) | distributed_tx_controller      |
| 12.8  | Поддержка snapshot-репликации                             | snapshot_replication           |
| 12.9  | Сервис репликации (Active-Passive / Active-Active)        | replication_service            |
| 12.10 | Кросс-узловая доставка сообщений (Cluster Messaging Bus)  | cluster_message_bus            |
| 12.11 | Watchdog (детектор сбоев узлов)                           | watchdog_node_failure          |
| 12.12 | Quorum Manager                                            | quorum_manager                 |
| 12.13 | Переизбрание координатора кластера                        | cluster_leader_election        |
| 12.14 | Распределённый планировщик запросов                       | distributed_query_planner      |
| 12.15 | Распределённое выполнение (Query Fragment Dispatcher)     | query_fragment_dispatcher      |
| 12.16 | Репликация WAL и контроль консистентности                 | wal_replication_engine         |
| 12.17 | Планировщик кросс-узловых джобов                          | distributed_job_scheduler      |
| 12.18 | Балансировка горячих шардов                               | hot_shard_balancer             |
| 12.19 | Интеграция с облачными оркестраторами (K8s, Nomad, etc.)  | cloud_orchestrator_integration |
| 12.20 | Кластерный роутер SQL-запросов                            | sql_cluster_router             |
| 12.21 | Распределённое логирование и сбор метрик                  | cluster_log_metrics            |
| 12.22 | Служба инициализации и масштабирования узлов              | node_autoscaler                |
| 12.23 | Online Repartition Engine                                 | online_repartition_engine      |
| 12.24 | Cluster Snapshot Isolation                                | cluster_snapshot_isolation     |
| 12.25 | Cluster Bootstrap и Discovery                             | cluster_bootstrap_discovery    |
| 13.1 | Менеджер времени жизни данных (TTL Engine)          | ttl_engine                  |
| 13.2 | Политики автоудаления (Eviction Policies)           | eviction_policy_engine      |
| 13.3 | Маркировка холодных/горячих данных                  | data_temperature_marker     |
| 13.4 | Служба вытеснения данных из памяти                  | memory_eviction_service     |
| 13.5 | Анализ частоты доступа и классификатор данных       | data_access_analyzer        |
| 13.6 | Auto-tiering Engine (горячие → холодные → архивные) | auto_tiering_engine         |
| 13.7 | Миграция данных между уровнями хранения             | tier_migration_engine       |
| 13.8 | Интеграция с Page Cache / Storage Layer             | tiering_storage_integration |
| 14.1 | Поддержка SYSTEM VERSIONED таблиц             | system_versioned_tables   |
| 14.2 | Исторический MVCC Snapshot Engine             | historical_mvcc_engine    |
| 14.3 | Запросы AS OF (SQL:2011 Temporal Support)     | asof_query_engine         |
| 14.4 | Retention Policy for Historical Snapshots     | snapshot_retention_policy |
| 14.5 | Версионирование DDL (schema history tracking) | ddl_versioning_engine     |
| 14.6 | Исторический аудит изменений (DML + DDL)      | historical_audit_log      |
| 15.1 | Ограничения CPU по сессиям / задачам                    | cpu_limit_engine            |
| 15.2 | Ограничения памяти (Memory Quota)                       | memory_quota_manager        |
| 15.3 | Ограничения IOPS по пользовательским сессиям            | iops_limiter                |
| 15.4 | Менеджер изоляции namespace'ов (Multi-tenant Isolation) | namespace_isolation_manager |
| 15.5 | Контроль использования ресурсов в реальном времени      | realtime_resource_monitor   |
| 15.6 | Policy Engine для ограничений и лимитов                 | resource_policy_engine      |
| 15.7 | Поддержка конфигурации лимитов через UI/API             | resource_config_api         |
| 16.1 | Поддержка форматов данных (UTF-8, JSON, XML, Binary)           | data_format_support          |
| 16.2 | Типы данных: даты, время, таймстемпы с наносекундной точностью | timestamp_types_engine       |
| 16.3 | Поддержка числовых форматов и локализованных представлений     | numeric_format_engine        |
| 16.4 | ICU Collation / Локализованная сортировка строк                | icu_collation_engine         |
| 16.5 | Языковые функции и локализация                                 | language_localization_engine |
| 16.6 | Поддержка двунаправленного текста и символов Unicode           | unicode_bidi_engine          |
| 17.1  | Внутренний API ядра (Core API Layer)                          | core_api_layer            |
| 17.2  | Внутренние хуки выполнения операций                           | internal_exec_hooks       |
| 17.3  | API сервис-дискавери и маршрутизация внутренних компонентов   | service_discovery_api     |
| 17.4  | Шина сообщений между модулями (Internal Message Bus)          | internal_message_bus      |
| 17.5  | Асинхронные события и сигнализация                            | async_signal_bus          |
| 17.6  | Планировщик жизненного цикла (Init, Start, Stop, Fail)        | lifecycle_scheduler       |
| 17.7  | Контекст выполнения / Request Context                         | request_context_engine    |
| 17.8  | Мониторинг внутренних ресурсов (контексты, аллокаторы, таски) | internal_resource_monitor |
| 17.9  | Внутренние справочники и метаданные компонентов               | core_metadata_registry    |
| 17.10 | API для отладки и трассировки ядра                            | debug_trace_api           |
| 18.1 | Монитор исполнения запросов (Execution Monitor)                        | execution_monitor         |
| 18.2 | Визуализация плана выполнения (Execution Timeline Viewer)              | execution_timeline_viewer |
| 18.3 | План-фидбек (Plan Runtime Feedback Engine)                             | plan_feedback_engine      |
| 18.4 | Повторное выполнение плана (Plan Replay Engine)                        | plan_replay_engine        |
| 18.5 | Инспектор отклонений от оптимального плана (Plan Divergence Inspector) | plan_divergence_inspector |
| 18.6 | Контроль SLA-нарушений по плану выполнения                             | sla_violation_detector    |
| 19.1 | Счётчики производительности (Perf Counters API)           | perf_counters          |
| 19.2 | Трекинг wall-time и CPU-time                              | wall_cpu_tracker       |
| 19.3 | Nanoscale Time Profiler (наносекундная детализация)       | nanoscale_profiler     |
| 19.4 | Bottleneck Profiler (поиск узких мест выполнения)         | bottleneck_profiler    |
| 19.5 | Хуки к плану выполнения для профилировки                  | plan_hook_profiler     |
| 19.6 | Интеграция с системным perf/pmu                           | perf_pmu_integration   |
| 19.7 | Aggregated Stats Collector (по CPU/IO/Memory/Concurrency) | agg_stats_collector    |

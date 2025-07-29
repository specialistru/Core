| №  | Блок                           | Идентификатор | Входящие | Исходящие |
|----|--------------------------------|-------------------------|-----------|------------|
| 0.1| Точка входа                    | main                    | 0| 7
| 0.2| Загрузчик конфигурации         | config_loader           | 1| 2
| 0.3| Логгер (stderr/syslog/binary)  | logger                  | 3| 1
| 0.4| Инициализация платформы        | platform_init           | 1| 1
| 0.5| Контроллер запуска             | bootstrap_controller    | 1| 1
| 1.1| Распределение памяти           | memory_allocator        | 4| 2
| 1.2| Кэш страниц                    | page_cache              | 0| 1
| 1.3| Колонковое хранилище           | column_store            | 6| 4
| 1.4| Строковое хранилище            | row_store               | 5| 2
| 1.5| Хранилище JSON                 | json_store              | 0| 1
| 1.6| Управление шардами             | sharding_manager        | 1| 1
| 1.7| Управление партициями          | partition_manager       | 1| 1
| 1.8| Авто-ребалансировка            | auto_rebalancer         | 0| 1
| 1.9| Компрессия данных              | compression_engine      | 1| 0
|1.10| Индексы                        | index_manager           | 3| 0
|1.11| TTL и вытеснение               | ttl_eviction            | 0| 1
|1.12| Холодное хранилище (SSD)       | cold_storage_fallback   | 2| 0
|1.13| Буферный пул                   | buffer_pool             | 5| 0
|1.14| Arrow-слой                     | arrow_layout            | 1| 0
|1.15| FOR-кодирование                | for_encoding            | 1| 0
|1.16| Ограничения multi-tenant       | multi_tenant_limiter    | 1| 2
|1.17| Версионированные таблицы       | versioned_tables        | 1| 0
| 2.1| Движок MVCC                    | mvcc_engine             | 3| 3
| 2.2| Менеджер снимков               | snapshot_manager        | 2| 0
| 2.3| WAL-журнал                     | wal_manager             | 3| 0
| 2.4| Менеджер транзакций            | txn_manager             | 5| 4
| 2.5| Очистка (vacuum)               | gc_vacuum               | 1| 0
| 2.6| Уровни изоляции                | isolation_levels        | 1| 0
| 2.7| Механизм failover              | failover_engine         | 1| 0
| 2.8| Репликация (Raft/Paxos)        | replication             | 1| 1
| 2.9| Восстановление после сбоя      | crash_recovery          | 0| 1
| 3.1| SQL-парсер                     | sql_parser              | 5| 1
| 3.2| Планировщик SQL                | sql_planner             | 1| 1
| 3.3| Оптимизатор SQL                | sql_optimizer           | 1| 4
| 3.4| Исполнитель SQL                | sql_executor            | 17| 9
| 3.5| Движок UDF                     | udf_engine              | 0| 1
| 3.6| PL/SQL рантайм                 | pl_lang_runtime         | 1| 0
| 3.7| DSL-движок                     | dsl_engine              | 0| 1
| 3.8| Аналитический движок           | analytic_engine         | 2| 3
| 4.1| JDBC-сервер                    | jdbc_server             | 0| 1
| 4.2| ODBC-шлюз                      | odbc_gateway            | 0| 1
| 4.3| REST API                       | rest_api                | 4| 1
| 4.4| GraphQL API                    | graphql_api             | 0| 1
| 4.5| gRPC-интерфейс                 | grpc_interface          | 1| 1
| 4.6| CLI-оболочка                   | cli_shell               | 0| 1
| 4.7| Документация OpenAPI           | openapi_doc             | 0| 1
| 4.8| CDC и стриминг                 | cdc_streaming           | 0| 1
| 4.9| Загрузчик S3/CSV/Parquet       | s3_batch_loader         | 0| 2
|4.10| Внешние хуки                   | external_hooks          | 0| 1
| 5.1| Материализованные представления| materialized_views      | 1| 0
| 5.2| Адаптивные агрегаты            | adaptive_aggregates     | 1| 0
| 5.3| Обработка временных рядов      | time_series_engine      | 0| 2
| 5.4| ML-движок (ONNX/Tensorflow)    | ml_runtime              | 0| 1
| 5.5| Аналитические функции          | analytics_functions     | 2| 0
| 5.6| HTAP-модуль                    | htap_manager            | 0| 3
| 5.7| Адаптер ML                     | ml_adapter              | 1| 1
| 6.1| RBAC/ABAC движок               | rbac_abac_engine        | 0| 1
| 6.2| Безопасность строк/колонок     | row_col_security        | 1| 0
| 6.3| TLS/mTLS/JWT                   | tls_mtls_jwt            | 0| 2
| 6.4| Аудит логов                    | audit_logger            | 0| 3
| 6.5| Политики доступа               | access_policies         | 0| 1
| 6.6| Защита от SQL-инъекций         | sql_injection_guard     | 0| 1
| 7.1| Prometheus-экспортер           | prometheus_exporter     | 0| 3
| 7.2| Трейсинг (OpenTelemetry)       | opentelemetry_tracer    | 1| 1
| 7.3| Профилировщик (Flamegraph)     | flamegraph_profiler     | 0| 1
| 7.4| Структурированное логирование  | structured_logger       | 2| 1
| 7.5| Fuzzer-движок                  | fuzzer_engine           | 0| 2
| 7.6| CI/CD раннер                   | ci_cd_runner            | 0| 2
| 7.7| Эволюция схемы                 | schema_evolver          | 1| 2
| 8.1| Векторный исполнитель          | vectorized_executor     | 3| 0
| 8.2| Адаптивные соединения          | adaptive_joins          | 1| 0
| 8.3| Кэш планов                     | plan_cache              | 1| 0
| 8.4| Адаптивный оптимизатор         | runtime_optimizer       | 1| 0
| 8.5| Стриминговый агрегатор         | streaming_aggregator    | 1| 0
| 8.6| Arrow-исполнитель              | arrow_executor          | 1| 0
| 8.7| Мгновенный snapshot            | instant_snapshot        | 0| 3
| 9.1| Песочница UDF                  | udf_sandbox             | 1| 0
| 9.2| Загрузчик плагинов             | plugin_loader           | 1| 1
| 9.3| SQL wire-протокол              | sql_wire_protocol       | 0| 2
| 9.4| JSON API                       | json_api                | 0| 2
| 9.5| Планировщик DSL-задач          | dsl_task_scheduler      | 1| 0
|10.1| Unit-тесты                     | unit_test_engine        | 1| 0
|10.2| Интеграционные тесты           | integration_test_harness| 0| 2
|10.3| Стресс-тесты                   | stress_test_runner      | 0| 2
|10.4| Мутационное покрытие           | mutation_coverage       | 0| 2

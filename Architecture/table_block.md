| ID  | Название блока                | Внутренний идентификатор      |
| --- | ----------------------------- | ----------------------------- |
| 0.1 | Точка входа (main)            | `main\_entrypoint`             |
| 0.2 | Загрузка конфигурации         | `config\_loader`               |
| 0.3 | Инициализация логирования     | `log\_init`                    |
| 0.4 | Инициализация подсистем       | `subsystem\_initializer`       |
| 0.5 | Контроллер запуска и останова | `startup\_shutdown\_controller` |
| 0.6 | CLI-интерфейс администратора  | `admin\_cli\_entrypoint`        |
| 1.1  | Row Store (строчно-ориентированное хранение) | `row\_store`              |
| 1.2  | Column Store (колоночное хранение)           | `column\_store`           |
| 1.3  | JSON Store                                   | `json\_store`             |
| 1.4  | Слой абстракции хранилища (Storage Layer)    | `storage\_layer`          |
| 1.5  | Tiered Storage (In-Memory / NVMe / SSD)      | `tiered\_storage`         |
| 1.6  | NUMA-aware Allocation                        | `numa\_allocator`         |
| 1.7  | CPU Affinity / Cache Coloring                | `cpu\_cache\_binding`      |
| 1.8  | Prefetching Engine                           | `prefetch\_engine`        |
| 1.9  | Sharding Manager                             | `sharding\_manager`       |
| 1.10 | Partitioning Engine                          | `partitioning\_engine`    |
| 1.11 | Auto Rebalancer                              | `auto\_rebalancer`        |
| 1.12 | Distributed Planner                          | `distributed\_planner`    |
| 1.13 | TTL Manager                                  | `ttl\_engine`             |
| 1.14 | Eviction Manager                             | `eviction\_engine`        |
| 1.15 | Auto-tiering Engine                          | `auto\_tiering\_engine`    |
| 1.16 | Поддержка большого объема памяти             | `large\_memory\_support`   |
| 1.17 | Ограничения по CPU/IOPS/Memory (per session) | `resource\_limits`        |
| 1.18 | PAX / Apache Arrow Layouts                   | `pax\_arrow\_engine`       |
| 1.19 | Frame-of-Reference + Векторное сжатие        | `frameofref\_compression` |
| 1.20 | Page Cache / Buffer Pool                     | `buffer\_pool`            |
| 1.21 | Dirty Page Tracking                          | `dirty\_tracking`         |
| 1.22 | Page Preload                                 | `page\_preloader`         |
| 1.23 | Слой индексирования                          | `index\_layer`            |
| 1.24 | B+ Tree Index                                | `bplus\_index`            |
| 1.25 | Hash Index                                   | `hash\_index`             |
| 1.26 | Bitmap Index                                 | `bitmap\_index`           |
| 1.27 | Inverted Index                               | `inverted\_index`         |
| 1.28 | Adaptive Indexing                            | `adaptive\_index`         |
| 1.29 | Dictionary Compression                       | `dict\_compression`       |
| 1.30 | Run-Length Encoding (RLE)                    | `rle\_compression`        |
| 1.31 | Delta Encoding                               | `delta\_compression`      |
| 1.32 | Vectorized Decompression                     | `vector\_decompressor`    |
| 1.33 | Storage Metadata Engine                      | `storage\_metadata`       |
| 1.34 | Datafile Format Layer                        | `datafile\_layer`         |
| 1.35 | Hot/Cold Data Separator                      | `hotcold\_separator`      |
| 1.36 | Auto-Tiering Policy Engine                   | `tiering\_policy\_engine`  |
| 1.37 | Table Catalog                                | `table\_catalog`          |
| 1.38 | Table Engine (Insert/Delete/Update)          | `table\_engine`           |
| 1.39 | Table Iterator Engine                        | `table\_iter\_engine`      |
| 2.1  | MVCC (Compressed Chains + Snapshot Isolation)         | `mvcc\_engine`                |
| 2.2  | ACID-совместимость                                    | `acid\_compliance`            |
| 2.3  | WAL (Write-Ahead Logging)                             | `wal\_engine`                 |
| 2.4  | Snapshot Recovery                                     | `snapshot\_recovery`          |
| 2.5  | Уровни изоляции транзакций (RC, RR, SI, SERIALIZABLE) | `isolation\_levels`           |
| 2.6  | Transaction Coordinator / Manager                     | `transaction\_manager`        |
| 2.7  | Undo/Redo логи                                        | `undo\_redo\_logs`             |
| 2.8  | Transaction Context per Session                       | `tx\_context`                 |
| 2.9  | Transaction Metadata Table                            | `tx\_metadata\_table`          |
| 2.10 | Transaction Conflict Resolver                         | `tx\_conflict\_resolver`       |
| 2.11 | Version Chain Garbage Collector                       | `tx\_gc`                      |
| 2.12 | Deadlock Detector                                     | `deadlock\_detector`          |
| 2.13 | Two-Phase Commit (2PC)                                | `2pc\_engine`                 |
| 2.14 | Raft / Paxos Consensus for Replication                | `consensus\_replication`      |
| 2.15 | Active-Active Replication                             | `active\_active\_replication`  |
| 2.16 | Active-Passive Replication                            | `active\_passive\_replication` |
| 2.17 | Multi-node Crash Recovery                             | `multi\_node\_recovery`        |
| 2.18 | Watchdog / Failover Service                           | `watchdog\_service`           |
| 2.19 | Distributed Transaction Engine (XA / global tx)       | `distributed\_tx\_engine`      |
| 2.20 | Background Vacuum / Cleaner                           | `tx\_background\_vacuum`       |
| 2.21 | Timestamp Oracle / TSO                                | `timestamp\_oracle`           |
| 2.22 | Snapshot Builder (MVCC snapshots per query/session)   | `snapshot\_builder`           |
| 2.23 | Serializable Fallback Engine                          | `serializable\_fallback`      |
| 2.24 | Log Compaction                                        | `wal\_log\_compaction`         |
| 2.25 | Transaction Audit / Journal                           | `tx\_audit\_log`               |
| 3.1  | SQL Ядро (ANSI SQL:2011+)                             | `sql\_core`                |
| 3.2  | Парсер SQL                                            | `sql\_parser`              |
| 3.3  | AST (Abstract Syntax Tree)                            | `sql\_ast`                 |
| 3.4  | Планировщик запросов (Logical Planner)                | `query\_planner`           |
| 3.5  | Оптимизатор: Cost-Based                               | `optimizer\_cost`          |
| 3.6  | Оптимизатор: Rule-Based                               | `optimizer\_rule`          |
| 3.7  | Оптимизатор: Adaptive / Reoptimization                | `optimizer\_adaptive`      |
| 3.8  | Поддержка оконных функций (OVER/PARTITION BY/...)     | `window\_functions`        |
| 3.9  | Поддержка recursive CTE                               | `recursive\_cte`           |
| 3.10 | Поддержка LATERAL и UNNEST                            | `sql\_lateral`             |
| 3.11 | MERGE и UPSERT операции                               | `sql\_merge\_upsert`        |
| 3.12 | Расширения: JSON-функции                              | `sql\_json\_ext`            |
| 3.13 | Расширения: FILTER, агрегаты и статистические функции | `sql\_aggregates\_ext`      |
| 3.14 | Расширения: MATCH\\\_RECOGNIZE                          | `sql\_match\_recognize`     |
| 3.15 | UDF на Lua                                            | `udf\_lua`                 |
| 3.16 | UDF на WASM                                           | `udf\_wasm`                |
| 3.17 | UDF на JS / C / C++                                   | `udf\_native`              |
| 3.18 | Сandbox для UDF                                       | `udf\_sandbox`             |
| 3.19 | Система пользовательских агрегатов (UDAF)             | `udaf\_engine`             |
| 3.20 | Встроенный DSL для транзакций (в стиле PL/SQL/ABAP)   | `transaction\_dsl`         |
| 3.21 | Поддержка процедур и триггеров                        | `sql\_procedures\_triggers` |
| 3.22 | Execution Engine SQL                                  | `sql\_execution\_engine`    |
| 4.1  | JDBC-драйвер                               | `jdbc\_driver`            |
| 4.2  | ODBC-драйвер                               | `odbc\_driver`            |
| 4.3  | CLI-интерфейс                              | `cli\_interface`          |
| 4.4  | REST API (OpenAPI)                         | `rest\_api`               |
| 4.5  | gRPC API                                   | `grpc\_api`               |
| 4.6  | WebSocket API                              | `websocket\_api`          |
| 4.7  | GraphQL API                                | `graphql\_api`            |
| 4.8  | JSON\\:API                                  | `json\_api\_spec`          |
| 4.9  | OpenAPI генератор схем                     | `openapi\_generator`      |
| 4.10 | Webhooks Engine                            | `webhook\_engine`         |
| 4.11 | Внешние процедуры (External Function Call) | `external\_procedure`     |
| 4.12 | Async Event Processor                      | `event\_processor`        |
| 4.13 | CDC (Change Data Capture)                  | `cdc\_engine`             |
| 4.14 | Поддержка Kafka                            | `kafka\_connector`        |
| 4.15 | Поддержка Apache Pulsar                    | `pulsar\_connector`       |
| 4.16 | Поддержка Debezium                         | `debezium\_adapter`       |
| 4.17 | Поддержка Redis Streams                    | `redis\_streams\_adapter`  |
| 4.18 | Поддержка S3 и внешних хранилищ            | `s3\_storage\_adapter`     |
| 4.19 | Интеграция с ERP/BI/BPM системами          | `erp\_bi\_integration`     |
| 5.1  | HTAP Engine (Hybrid Transactional/Analytical Processing) | `htap\_engine`              |
| 5.2  | Виртуальные представления (Views)                        | `sql\_views`                |
| 5.3  | Материализованные представления / Кубы                   | `materialized\_views`       |
| 5.4  | Адаптивные агрегаты                                      | `adaptive\_aggregates`      |
| 5.5  | ROLLUP, CUBE, GROUPING SETS                              | `sql\_rollup\_cube`          |
| 5.6  | Time-Series Engine (встроенные типы)                     | `timeseries\_engine`        |
| 5.7  | Gap-Fill / Interpolation                                 | `ts\_gapfill\_interpolation` |
| 5.8  | Downsampling / Aggregation for TS                        | `ts\_downsampling`          |
| 5.9  | Retention Policy Manager                                 | `ts\_retention\_policy`      |
| 5.10 | ML Inference Engine (ONNX, TensorFlow, PyTorch)          | `ml\_inference\_engine`      |
| 5.11 | SQL-интерфейс к ML-моделям                               | `ml\_sql\_inference`         |
| 5.12 | Поддержка ONNX Runtime                                   | `ml\_onnx\_runtime`          |
| 5.13 | Поддержка TensorFlow Runtime                             | `ml\_tensorflow\_runtime`    |
| 5.14 | Поддержка PyTorch Runtime                                | `ml\_pytorch\_runtime`       |
| 5.15 | Функции ранжирования, percentiles, correlation           | `ml\_sql\_statistics`        |
| 5.16 | Встроенные алгоритмы: кластеризация, прогнозирование     | `ml\_builtins`              |
| 5.17 | SQL Scheduler для аналитических джобов                   | `sql\_job\_scheduler`        |
| 6.1  | RBAC (Role-Based Access Control)              | `rbac\_engine`              |
| 6.2  | ABAC (Attribute-Based Access Control)         | `abac\_engine`              |
| 6.3  | Row-Level Security                            | `row\_level\_security`       |
| 6.4  | Column-Level Security                         | `column\_level\_security`    |
| 6.5  | TLS / mTLS                                    | `tls\_stack`                |
| 6.6  | JWT / Token-Based Auth                        | `jwt\_auth`                 |
| 6.7  | Transparent Data Encryption (TDE)             | `tde\_engine`               |
| 6.8  | Forward Secrecy                               | `forward\_secrecy`          |
| 6.9  | ACL и сетевые правила                         | `acl\_rules`                |
| 6.10 | Scoped Tokens                                 | `scoped\_tokens`            |
| 6.11 | DML / DDL Audit Logging                       | `audit\_log\_dml\_ddl`        |
| 6.12 | Immutable Audit Log                           | `immutable\_audit\_log`      |
| 6.13 | Access Policy Engine                          | `access\_policy\_engine`     |
| 6.14 | Защита от SQL-инъекций и sandbox для Lua/WASM | `sql\_injection\_protection` |
| 7.1  | Метрики (Prometheus, OpenMetrics)                    | `metrics\_engine`            |
| 7.2  | IOPS / Latency / Query Duration Counters             | `performance\_counters`      |
| 7.3  | Наносекундный трекинг времени                        | `nanoscale\_timing`          |
| 7.4  | Структурированное логирование (JSON, syslog, binary) | `structured\_logger`         |
| 7.5  | OpenTelemetry / Jaeger / Zipkin Integration          | `tracing\_exporter`          |
| 7.6  | Query Tracing и Timeline Viewer                      | `query\_timeline\_viewer`     |
| 7.7  | Flamegraph Viewer                                    | `flamegraph\_viewer`         |
| 7.8  | Трассировка выполнения запросов по плану             | `execution\_tracer`          |
| 7.9  | Встроенный профилировщик                             | `builtin\_profiler`          |
| 7.10 | CLI-инструменты диагностики                          | `cli\_diagnostics`           |
| 7.11 | REST-интерфейс мониторинга                           | `rest\_monitoring\_api`       |
| 7.12 | Веб-панель наблюдаемости                             | `web\_monitoring\_ui`         |
| 7.13 | Интеграция с Alertmanager                            | `alertmanager\_hook`         |
| 7.14 | Fuzzing Engine (libFuzzer, AFL++)                    | `fuzzing\_engine`            |
| 7.15 | ASAN / TSAN / MSAN (Sanitizers)                      | `sanitizer\_hooks`           |
| 7.16 | Fault Injection Framework                            | `fault\_injection\_framework` |
| 7.17 | Chaos Testing Controller                             | `chaos\_testing\_controller`  |
| 7.18 | Crash Recovery Validator                             | `crash\_recovery\_validator`  |
| 7.19 | CI/CD Pipeline Hooks                                 | `cicd\_hooks`                |
| 7.20 | Mutation Testing Framework                           | `mutation\_testing`          |
| 8.1  | Vectorized Execution Engine                        | `vectorized\_execution`    |
| 8.2  | Параллельные пайплайны выполнения                  | `parallel\_pipelines`      |
| 8.3  | Streaming Aggregator                               | `streaming\_aggregator`    |
| 8.4  | Adaptive Hash Join                                 | `adaptive\_hash\_join`      |
| 8.5  | Merge Join                                         | `merge\_join`              |
| 8.6  | Index-Nested Loop Join                             | `index\_nested\_loop\_join`  |
| 8.7  | Grace Hash Join                                    | `grace\_hash\_join`         |
| 8.8  | Runtime Re-optimization                            | `runtime\_reoptimization`  |
| 8.9  | Plan Caching                                       | `plan\_cache`              |
| 8.10 | Selectivity Feedback Loop                          | `selectivity\_feedback`    |
| 8.11 | Copy-on-Write Snapshots                            | `copy\_on\_write\_snapshots` |
| 8.12 | Instant Backup / Restore                           | `instant\_backup\_restore`  |
| 8.13 | Clone Tables / Table Snapshots                     | `table\_clone\_snapshots`   |
| 8.14 | PAX Layout Support                                 | `pax\_layout\_support`      |
| 8.15 | Apache Arrow Layout Execution                      | `arrow\_layout\_engine`     |
| 8.16 | Format-aware Execution Pipeline                    | `format\_aware\_executor`   |
| 8.17 | Execution Plan Inliner                             | `execution\_plan\_inliner`  |
| 8.18 | JIT Engine                                         | `jit\_engine`              |
| 8.19 | In-Memory Adaptive Buffers                         | `adaptive\_buffers`        |
| 8.20 | Hot Operator Detection                             | `hot\_operator\_detector`   |
| 8.21 | Critical Path Analyzer (Latency < 500ns Optimizer) | `critical\_path\_optimizer` |
| 9.1  | Поддержка UDF (пользовательских функций)             | `udf\_support`            |
| 9.2  | Поддержка UDAF (агрегатных пользовательских функций) | `udaf\_support`           |
| 9.3  | Расширения на C/C++                                  | `c\_cpp\_extensions`       |
| 9.4  | Внешние интерфейсы: REST, gRPC и JSON\\:API           | `ext\_api\_interfaces`     |
| 9.5  | Песочница для Lua/WASM расширений                    | `udf\_sandbox\_engine`     |
| 9.6  | Загрузка внешних модулей (dlopen / LoadLibrary)      | `dynamic\_module\_loader`  |
| 9.7  | SQL-протокол: собственный бинарный формат            | `binary\_sql\_protocol`    |
| 9.8  | Совместимость с PostgreSQL wire protocol             | `pgsql\_wire\_compat`      |
| 9.9  | Расширяемый SQL API                                  | `sql\_api\_extension`      |
| 9.10 | JSON DSL и сценарии для ETL                          | `etl\_json\_dsl`           |
| 9.11 | ERP DSL (доменно-специфичный язык для ERP сценариев) | `erp\_dsl\_engine`         |
| 9.12 | Генератор ETL-процессов                              | `etl\_generator`          |
| 9.13 | Планировщик пользовательских задач                   | `custom\_task\_scheduler`  |
| 9.14 | Расширения для внешних BI/ML/Streaming систем        | `bi\_ml\_streaming\_ext`    |
| 9.15 | Интерфейс расширения ядра (внутренние хуки и API)    | `core\_extension\_hooks`   |
| 10.1  | Unit-тесты                                                 | `unit\_test\_engine`          |
| 10.2  | Интеграционные тесты                                       | `integration\_test\_engine`   |
| 10.3  | Fuzz-тестирование (на AST, SQL, транзакциях, бинарных API) | `fuzz\_testing\_engine`       |
| 10.4  | Стресс-тестирование (нагрузочные сценарии)                 | `stress\_testing\_engine`     |
| 10.5  | Soak-тестирование (длительные сценарии)                    | `soak\_testing\_engine`       |
| 10.6  | Мутационное тестирование                                   | `mutation\_testing\_engine`   |
| 10.7  | Покрытие кода (coverage engine)                            | `coverage\_engine`           |
| 10.8  | Тестирование безопасности                                  | `security\_test\_suite`       |
| 10.9  | Проверка консистентности после краша                       | `crash\_consistency\_checker` |
| 10.10 | Инструменты CI/CD тестирования                             | `ci\_cd\_test\_hooks`          |
| 10.11 | Анализ качества схемы / миграций                           | `schema\_migration\_checker`  |
| 11.1  | Планировщик задач (Task Scheduler)                    | `task\_scheduler`         |
| 11.2  | Очередь отложенных заданий (Delay Queue)              | `delay\_queue\_engine`     |
| 11.3  | Cron-подобный триггер                                 | `cron\_trigger`           |
| 11.4  | Планировщик фоновых задач (Background Job Engine)     | `background\_job\_engine`  |
| 11.5  | Очередь транзакций (Transaction Queue)                | `tx\_queue`               |
| 11.6  | Планировщик аналитических джобов (OLAP Job Scheduler) | `olap\_job\_scheduler`     |
| 11.7  | Планировщик ETL задач                                 | `etl\_job\_scheduler`      |
| 11.8  | Планировщик SLA / Policy Based                        | `policy\_scheduler`       |
| 11.9  | Поддержка приоритетов / QoS                           | `priority\_qos\_manager`   |
| 11.10 | Механизм метаданных для задач                         | `task\_metadata\_engine`   |
| 12.1  | Кластерный менеджер                                       | `cluster\_manager`                |
| 12.2  | Балансировщик нагрузки (Load Balancer)                    | `load\_balancer`                  |
| 12.3  | Поддержка шардирования (Sharding Layer)                   | `sharding\_layer`                 |
| 12.4  | Поддержка re-sharding / Auto-rebalancing                  | `resharding\_engine`              |
| 12.5  | Каталог кластерных узлов                                  | `node\_catalog`                   |
| 12.6  | Согласование через Raft / Paxos                           | `consensus\_protocol`             |
| 12.7  | Управление распределёнными транзакциями (XA / 2PC / Saga) | `distributed\_tx\_controller`      |
| 12.8  | Поддержка snapshot-репликации                             | `snapshot\_replication`           |
| 12.9  | Сервис репликации (Active-Passive / Active-Active)        | `replication\_service`            |
| 12.10 | Кросс-узловая доставка сообщений (Cluster Messaging Bus)  | `cluster\_message\_bus`            |
| 12.11 | Watchdog (детектор сбоев узлов)                           | `watchdog\_node\_failure`          |
| 12.12 | Quorum Manager                                            | `quorum\_manager`                 |
| 12.13 | Переизбрание координатора кластера                        | `cluster\_leader\_election`        |
| 12.14 | Распределённый планировщик запросов                       | `distributed\_query\_planner`      |
| 12.15 | Распределённое выполнение (Query Fragment Dispatcher)     | `query\_fragment\_dispatcher`      |
| 12.16 | Репликация WAL и контроль консистентности                 | `wal\_replication\_engine`         |
| 12.17 | Планировщик кросс-узловых джобов                          | `distributed\_job\_scheduler`      |
| 12.18 | Балансировка горячих шардов                               | `hot\_shard\_balancer`             |
| 12.19 | Интеграция с облачными оркестраторами (K8s, Nomad, etc.)  | `cloud\_orchestrator\_integration` |
| 12.20 | Кластерный роутер SQL-запросов                            | `sql\_cluster\_router`             |
| 12.21 | Распределённое логирование и сбор метрик                  | `cluster\_log\_metrics`            |
| 12.22 | Служба инициализации и масштабирования узлов              | `node\_autoscaler`                |
| 12.23 | Online Repartition Engine                                 | `online\_repartition\_engine`      |
| 12.24 | Cluster Snapshot Isolation                                | `cluster\_snapshot\_isolation`     |
| 12.25 | Cluster Bootstrap и Discovery                             | `cluster\_bootstrap\_discovery`    |
| 13.1 | Менеджер времени жизни данных (TTL Engine)          | `ttl\_engine`                  |
| 13.2 | Политики автоудаления (Eviction Policies)           | `eviction\_policy\_engine`      |
| 13.3 | Маркировка холодных/горячих данных                  | `data\_temperature\_marker`     |
| 13.4 | Служба вытеснения данных из памяти                  | `memory\_eviction\_service`     |
| 13.5 | Анализ частоты доступа и классификатор данных       | `data\_access\_analyzer`        |
| 13.6 | Auto-tiering Engine (горячие → холодные → архивные) | `auto\_tiering\_engine`         |
| 13.7 | Миграция данных между уровнями хранения             | `tier\_migration\_engine`       |
| 13.8 | Интеграция с Page Cache / Storage Layer             | `tiering\_storage\_integration` |
| 14.1 | Поддержка SYSTEM VERSIONED таблиц             | `system\_versioned\_tables`   |
| 14.2 | Исторический MVCC Snapshot Engine             | `historical\_mvcc\_engine`    |
| 14.3 | Запросы AS OF (SQL:2011 Temporal Support)     | `asof\_query\_engine`         |
| 14.4 | Retention Policy for Historical Snapshots     | `snapshot\_retention\_policy` |
| 14.5 | Версионирование DDL (schema history tracking) | `ddl\_versioning\_engine`     |
| 14.6 | Исторический аудит изменений (DML + DDL)      | `historical\_audit\_log`      |
| 15.1 | Ограничения CPU по сессиям / задачам                    | `cpu\_limit\_engine`            |
| 15.2 | Ограничения памяти (Memory Quota)                       | `memory\_quota\_manager`        |
| 15.3 | Ограничения IOPS по пользовательским сессиям            | `iops\_limiter`                |
| 15.4 | Менеджер изоляции namespace'ов (Multi-tenant Isolation) | `namespace\_isolation\_manager` |
| 15.5 | Контроль использования ресурсов в реальном времени      | `realtime\_resource\_monitor`   |
| 15.6 | Policy Engine для ограничений и лимитов                 | `resource\_policy\_engine`      |
| 15.7 | Поддержка конфигурации лимитов через UI/API             | `resource\_config\_api`         |
| 16.1 | Поддержка форматов данных (UTF-8, JSON, XML, Binary)           | `data\_format\_support`          |
| 16.2 | Типы данных: даты, время, таймстемпы с наносекундной точностью | `timestamp\_types\_engine`       |
| 16.3 | Поддержка числовых форматов и локализованных представлений     | `numeric\_format\_engine`        |
| 16.4 | ICU Collation / Локализованная сортировка строк                | `icu\_collation\_engine`         |
| 16.5 | Языковые функции и локализация                                 | `language\_localization\_engine` |
| 16.6 | Поддержка двунаправленного текста и символов Unicode           | `unicode\_bidi\_engine`          |
| 17.1  | Внутренний API ядра (Core API Layer)                          | `core\_api\_layer`            |
| 17.2  | Внутренние хуки выполнения операций                           | `internal\_exec\_hooks`       |
| 17.3  | API сервис-дискавери и маршрутизация внутренних компонентов   | `service\_discovery\_api`     |
| 17.4  | Шина сообщений между модулями (Internal Message Bus)          | `internal\_message\_bus`      |
| 17.5  | Асинхронные события и сигнализация                            | `async\_signal\_bus`          |
| 17.6  | Планировщик жизненного цикла (Init, Start, Stop, Fail)        | `lifecycle\_scheduler`       |
| 17.7  | Контекст выполнения / Request Context                         | `request\_context\_engine`    |
| 17.8  | Мониторинг внутренних ресурсов (контексты, аллокаторы, таски) | `internal\_resource\_monitor` |
| 17.9  | Внутренние справочники и метаданные компонентов               | `core\_metadata\_registry`    |
| 17.10 | API для отладки и трассировки ядра                            | `debug\_trace\_api`           |
| 18.1 | Монитор исполнения запросов (Execution Monitor)                        | `execution\_monitor`         |
| 18.2 | Визуализация плана выполнения (Execution Timeline Viewer)              | `execution\_timeline\_viewer` |
| 18.3 | План-фидбек (Plan Runtime Feedback Engine)                             | `plan\_feedback\_engine`      |
| 18.4 | Повторное выполнение плана (Plan Replay Engine)                        | `plan\_replay\_engine`        |
| 18.5 | Инспектор отклонений от оптимального плана (Plan Divergence Inspector) | `plan\_divergence\_inspector` |
| 18.6 | Контроль SLA-нарушений по плану выполнения                             | `sla\_violation\_detector`    |
| 19.1 | Счётчики производительности (Perf Counters API)           | `perf\_counters`          |
| 19.2 | Трекинг wall-time и CPU-time                              | `wall\_cpu\_tracker`       |
| 19.3 | Nanoscale Time Profiler (наносекундная детализация)       | `nanoscale\_profiler`     |
| 19.4 | Bottleneck Profiler (поиск узких мест выполнения)         | `bottleneck\_profiler`    |
| 19.5 | Хуки к плану выполнения для профилировки                  | `plan\_hook\_profiler`     |
| 19.6 | Интеграция с системным perf/pmu                           | `perf\_pmu\_integration`   |
| 19.7 | Aggregated Stats Collector (по CPU/IO/Memory/Concurrency) | `agg\_stats\_collector`    |

/**
 * @file    network_module.h
 * @brief   Интерфейс сетевого модуля для обмена данными.
 * @author  Иван Иванов <ivan.ivanov@example.com>
 * @date    2025-08-04 14:30
 * @version 1.3.0
 * @license MIT
 *
 * @note
 * Модуль предоставляет API для инициализации, отправки и получения сетевых данных
 * с поддержкой конфигурируемых таймаутов, повторных попыток и логирования.
 *
 * @par История изменений:
 * | Дата       | Время  | Автор       | Версия | Описание изменений                             |
 * |------------|--------|-------------|--------|-----------------------------------------------|
 * | 2025-07-15 | 09:20  | Иван Иванов | 1.0.0  | Инициализация модуля                           |
 * | 2025-07-25 | 16:45  | Иван Иванов | 1.1.0  | Добавлен enum NetworkStatus                    |
 * | 2025-08-04 | 14:30  | Иван Иванов | 1.2.0  | Расширена документация, добавлены таблицы     |
 * | 2025-08-05 | 11:00  | Иван Иванов | 1.3.0  | Добавлена потокобезопасность и рекомендации по памяти |
 *
 * @section thread_safety Потокобезопасность
 * Все функции, кроме network_init и network_cleanup, потокобезопасны и могут вызываться
 * из разных потоков одновременно.
 *
 * @section memory_management Управление памятью
 * Пользователь отвечает за выделение и освобождение памяти для буферов данных,
 * передаваемых в network_send и network_receive.
 * Модуль не копирует содержимое буферов, а использует их напрямую в пределах вызова.
 *
 * @section logging Логирование
 * Включение логирования осуществляется через поле enable_logging структуры NetworkConfig.
 * Логи записываются в системный журнал (syslog) или stdout в зависимости от платформы.
 * Логируются критические ошибки, попытки повторов, таймауты.
 *
 * @section tests Модульные тесты
 * Рекомендуется использовать файлы тестов из каталога tests/ для проверки корректности работы.
 * Интеграционные тесты покрывают сценарии подключения, отправки и получения данных,
 * включая ошибки сети.
 */

#ifndef NETWORK_MODULE_H
#define NETWORK_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>


/* Версионные макросы */
#define NETWORK_MODULE_VERSION       "1.3.0"
#define NETWORK_MODULE_VERSION_MAJOR 1
#define NETWORK_MODULE_VERSION_MINOR 3
#define NETWORK_MODULE_VERSION_PATCH 0


/**
 * @enum NetworkStatus
 * @brief Коды возврата сетевого модуля.
 *
 * | Имя                 | Значение | Описание                                    | Рекомендуемое действие              |
 * |---------------------|----------|---------------------------------------------|-----------------------------------|
 * | NETWORK_OK          | 0        | Операция выполнена успешно                   | Продолжить работу                  |
 * | NETWORK_ERROR       | -1       | Общая ошибка                                 | Завершить операцию                 |
 * | NETWORK_TIMEOUT     | -2       | Время ожидания истекло                        | Попробовать повторить              |
 * | NETWORK_INVALID_ARG | -3       | Некорректный аргумент                         | Логировать и возвратить ошибку     |
 * | NETWORK_DISCONNECTED| -4       | Потеряно соединение                           | Попытаться переподключиться       |
 * | NETWORK_BUSY        | -5       | Модуль занят, операция отклонена              | Попробовать позже                  |
 */
typedef enum {
    NETWORK_OK = 0,
    NETWORK_ERROR = -1,
    NETWORK_TIMEOUT = -2,
    NETWORK_INVALID_ARG = -3,
    NETWORK_DISCONNECTED = -4,
    NETWORK_BUSY = -5
} NetworkStatus;


/**
 * @struct NetworkConfig
 * @brief Конфигурация сетевого модуля.
 *
 * | Поле           | Тип       | Описание                              | Ограничения        | Значение по умолчанию | Примечания              |
 * |----------------|-----------|-------------------------------------|--------------------|-----------------------|-------------------------|
 * | timeout_ms     | uint32_t  | Таймаут операции в миллисекундах    | 0..60000           | 5000                  | Максимум 60 секунд       |
 * | max_retries    | int       | Максимальное число повторных попыток| >= 0               | 3                     |                         |
 * | enable_logging | bool      | Включить логирование                 | true/false         | false                 | Для отладки              |
 */
typedef struct {
    uint32_t timeout_ms;
    int max_retries;
    bool enable_logging;
} NetworkConfig;


/**
 * @brief Инициализация сетевого модуля.
 * @param config Указатель на структуру конфигурации.
 * @return Код результата NetworkStatus.
 *
 * @note
 * - Должна вызываться единожды до использования других функций.
 * - Если уже инициализирован, вызов вернёт NETWORK_BUSY.
 * - Потокобезопасна при условии внешней синхронизации инициализации.
 */
NetworkStatus network_init(const NetworkConfig *config);


/**
 * @brief Отправка данных по сети.
 * @param data Указатель на данные для отправки. Не NULL.
 * @param len Длина данных в байтах. > 0.
 * @return Код результата NetworkStatus.
 *
 * @note
 * - Функция блокирующая, может повторять попытки отправки до max_retries.
 * - При ошибках сетевого соединения может возвращать NETWORK_DISCONNECTED.
 * - Потокобезопасна.
 */
NetworkStatus network_send(const void *data, size_t len);


/**
 * @brief Получение данных из сети.
 * @param buffer Буфер для приема данных. Не NULL.
 * @param max_len Максимальный размер буфера. > 0.
 * @param received_len Указатель на переменную, куда записывается количество принятых байт. Не NULL.
 * @return Код результата NetworkStatus.
 *
 * @note
 * - Функция блокирующая, может возвращать NETWORK_TIMEOUT при отсутствии данных.
 * - Потокобезопасна.
 */
NetworkStatus network_receive(void *buffer, size_t max_len, size_t *received_len);


/**
 * @brief Завершение работы и освобождение ресурсов модуля.
 *
 * @note
 * - После вызова требуется повторная инициализация для дальнейшего использования.
 * - Не потокобезопасна, должна вызываться после завершения всех операций.
 */
void network_cleanup(void);


/**
 * @brief Таблица основных функций модуля и их назначение
 *
 * | Функция           | Параметры                                         | Возвращаемое значение | Краткое описание                         | Примечания                                 |
 * |-------------------|--------------------------------------------------|-----------------------|-----------------------------------------|--------------------------------------------|
 * | network_init      | const NetworkConfig *config                       | NetworkStatus          | Инициализация сетевого модуля            | Вызывать перед другими функциями           |
 * | network_send      | const void *data, size_t len                      | NetworkStatus          | Отправка данных                         | Блокирующая функция                         |
 * | network_receive   | void *buffer, size_t max_len, size_t *received_len| NetworkStatus         | Прием данных                           | Блокирующая функция                         |
 * | network_cleanup   | void                                              | void                  | Освобождение ресурсов модуля             | Вызывать при завершении работы              |
 *
 * @note
 * - Все функции, возвращающие NetworkStatus, возвращают NETWORK_OK при успешном выполнении.
 * - В случае ошибки возвращается соответствующий код из NetworkStatus.
 * - Параметры `data` и `buffer` не должны быть NULL.
 * - После вызова network_cleanup модуль должен быть повторно инициализирован перед использованием.
 */


/**
 * @example
 * // Пример инициализации, отправки и приема данных с проверкой ошибок
 * #include <stdio.h>
 * void example_usage(void) {
 *     NetworkConfig cfg = {
 *         .timeout_ms = 3000,
 *         .max_retries = 5,
 *         .enable_logging = true
 *     };
 *
 *     NetworkStatus status = network_init(&cfg);
 *     if (status != NETWORK_OK) {
 *         printf("Ошибка инициализации: %d\n", status);
 *         return;
 *     }
 *
 *     const char message[] = "Hello, network!";
 *     status = network_send(message, sizeof(message));
 *     if (status != NETWORK_OK) {
 *         printf("Ошибка отправки: %d\n", status);
 *         network_cleanup();
 *         return;
 *     }
 *
 *     char recv_buffer[256];
 *     size_t received = 0;
 *     status = network_receive(recv_buffer, sizeof(recv_buffer), &received);
 *     if (status == NETWORK_OK && received > 0) {
 *         printf("Принято %zu байт: %.*s\n", received, (int)received, recv_buffer);
 *     } else if (status == NETWORK_TIMEOUT) {
 *         printf("Таймаут приема данных.\n");
 *     } else {
 *         printf("Ошибка приема: %d\n", status);
 *     }
 *
 *     network_cleanup();
 * }
 */

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_MODULE_H */

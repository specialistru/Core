/*
 * File:        network_module.h
 * Description: Интерфейс для сетевого модуля.
 * Author:      Ivan Petrov <ivan.petrov@example.com>
 * Created:     2025-08-04 10:12
 * Version:     1.3.0
 * License:     MIT License
 *
 * Changelog:
 * ------------------------------------------------------------------------------
 * Date                Author                        Version     Description
 * ------------------- ----------------------------- ----------- ----------------
 * 2025-08-04 14:45    Ivan Petrov <ivan.petrov@...> 1.3.0       Сделана opaque-
 *                                                              структура, добавлена
 *                                                              версионность и
 *                                                              net_status_str().
 * 2025-08-04 10:12    Ivan Petrov <ivan.petrov@...> 1.2.0       Добавлена версия,
 *                                                              автор и история.
 * 2025-07-28 15:30    Ivan Petrov <ivan.petrov@...> 1.1.0       Добавлены net_send
 *                                                              и net_close.
 * 2025-07-20 09:45    Ivan Petrov <ivan.petrov@...> 1.0.0       Базовая реализация
 *                                                              net_connect и init.
 * ------------------------------------------------------------------------------
 */

#ifndef NETWORK_MODULE_H
#define NETWORK_MODULE_H

#include <stddef.h>   // size_t
#include <stdint.h>   // uint32_t

#ifdef __cplusplus
extern "C" {
#endif

// Версия интерфейса (для сравнения во время сборки)
#define NETWORK_MODULE_VERSION       "1.3.0"
#define NETWORK_MODULE_VERSION_MAJOR 1
#define NETWORK_MODULE_VERSION_MINOR 3
#define NETWORK_MODULE_VERSION_PATCH 0

/*
 * Коды возврата операций сетевого модуля.
 *
 * +-------------------+----------+------------------------------------+-------------------------+
 * | Имя               | Значение | Описание                           | Рекомендуемое поведение  |
 * +-------------------+----------+------------------------------------+-------------------------+
 * | NET_OK            | 0        | Успешное выполнение                | Продолжать работу        |
 * | NET_ERROR         | -1       | Неопределённая ошибка             | Завершить операцию       |
 * | NET_TIMEOUT       | -2       | Превышено время ожидания          | Повторить операцию       |
 * | NET_INVALID_ARG   | -3       | Недопустимый аргумент             | Логировать и игнорировать|
 * | NET_NOT_CONNECTED | -4       | Соединение не установлено         | Сообщить пользователю    |
 * +-------------------+----------+------------------------------------+-------------------------+
 */
typedef enum {
    NET_OK = 0,
    NET_ERROR = -1,
    NET_TIMEOUT = -2,
    NET_INVALID_ARG = -3,
    NET_NOT_CONNECTED = -4
} NetStatus;

/*
 * Получить текстовое описание статуса NetStatus.
 *
 * status - код возврата
 * 
 * Возвращает строку, соответствующую статусу (например, "NET_TIMEOUT").
 * Никогда не возвращает NULL.
 */
const char* net_status_str(NetStatus status);

/*
 * Структура конфигурации и состояния сетевого соединения (NetConnection).
 *
 * +----------------+--------------+----------------------------------------------+-------------------------+----------------+
 * | Поле           | Тип          | Описание                                     | Ограничения             | По умолчанию    |
 * +----------------+--------------+----------------------------------------------+-------------------------+----------------+
 * | socket_fd      | int          | Дескриптор сокета                            | >= 0 или -1 (нет сокета)| -1             |
 * | timeout_ms     | uint32_t     | Таймаут операций в миллисекундах             | 0..30000                | 5000           |
 * | is_connected   | int (bool)   | Флаг состояния соединения                     | 0 = нет, 1 = да         | 0              |
 * +----------------+--------------+----------------------------------------------+-------------------------+----------------+
 *
 * Примечание:
 * Эта структура скрыта (opaque) для пользователя. 
 * Представлена здесь для пояснения внутренней реализации.
 */
typedef struct NetConnection NetConnection;

/*
 * Инициализация сетевого модуля.
 * Возвращает NET_OK или код ошибки.
 */
NetStatus net_init(void);

/*
 * Завершение работы модуля и освобождение ресурсов.
 */
void net_shutdown(void);

/*
 * Создаёт и инициализирует новый объект NetConnection.
 *
 * timeout_ms - начальное значение таймаута (в мс)
 *
 * Возвращает указатель на NetConnection или NULL при ошибке.
 */
NetConnection* net_connection_create(uint32_t timeout_ms);

/*
 * Уничтожает объект NetConnection.
 *
 * conn - указатель на соединение (может быть NULL)
 */
void net_connection_destroy(NetConnection* conn);

/*
 * Устанавливает соединение с сервером.
 *
 * conn     - соединение
 * address  - IP-адрес или имя хоста
 * port     - порт для подключения
 *
 * Возвращает код NetStatus.
 */
NetStatus net_connect(NetConnection* conn, const char* address, int port);

/*
 * Отправка данных по соединению.
 *
 * conn - соединение
 * data - буфер с данными
 * len  - длина буфера
 *
 * Возвращает код NetStatus.
 */
NetStatus net_send(NetConnection* conn, const void* data, size_t len);

/*
 * Закрытие активного соединения (если есть).
 *
 * conn - соединение
 */
void net_close(NetConnection* conn);

/*
 * Устанавливает таймаут (в мс) для операций.
 *
 * conn        - соединение
 * timeout_ms  - значение таймаута
 *
 * Возвращает NET_OK или NET_INVALID_ARG.
 */
NetStatus net_set_timeout(NetConnection* conn, uint32_t timeout_ms);

/*
 * Получает текущий таймаут (в мс).
 *
 * conn - соединение
 *
 * Возвращает значение таймаута.
 * При ошибке возвращает 0.
 */
uint32_t net_get_timeout(const NetConnection* conn);

#ifdef __cplusplus
}
#endif

#endif // NETWORK_MODULE_H


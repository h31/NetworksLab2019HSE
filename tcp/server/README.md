## Типы пересылаемых данных
Каждое сообщение начинается идентификатором типа этого сообщение (8-битное беззнаковое число), после этого идут параметры в виде Null-terminated ASCII строк.

## Запросы от клиента
| Тип | Идентификатор (uint8_t)| Поле 1 | Поле 2 |
| ------ | ------ | ------ | ------ |
| LOGIN | 0x00 | username (string) | --- |
| SINGLE_MESSAGE | 0x01 | username (string) | message (string) |
| BROADCAST_MESSAGE | 0x02 | message (string) | --- |

- **LOGIN** -- залогиниться с именем *username*
- **SINGLE_MESSAGE** -- отправить пользователю *username* сообщение *message*
- **BROADCAST_MESSAGE** -- отправить всем активным пользователям сообщение *message*

## Запросы от сервера
| Тип | Идентификатор (uint8_t)| Поле 1 | Поле 2 |
| ------ | ------ | ------ | ------ |
| LOGIN_SUCCESS | 0xFF | --- | --- |
| SINGLE_MESSAGE | 0xFE | username (string) | message (string) |
| BROADCAST_MESSAGE | 0xFD | username (string) | message (string) |
| ERROR | 0xFC | errmessage (string) | --- |
| DISCONNECT | 0xFB | --- | --- |

- **LOGIN_SUCCESS** -- пользователь успешно залогинился
- **SINGLE_MESSAGE** -- получено личное сообщение *message* от пользователя *username*
- **BROADCAST_MESSAGE** -- получено публичное сообщение *message* от пользователя *username*
- **ERROR** -- произошла ошибка при обработке предыдущего запроса, сообщение об ошибке: *errmessage*
- **DISCONNECT** -- пользователь принудительно отключен от сервера

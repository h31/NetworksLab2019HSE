## Сообщения от клиента:

CONNECT | name : string

NEW_RATING | name : string | cnt : uint8_t

DELETE_RATING | id : uint32_t

OPEN_RATING | id : uint32_t

CLOSE_RATING | id : uint32_t

ADD_CHOICE | id : uint32_t | choice : string

RATING_LIST

SHOW_RATING | id : uint32_t

VOTE_RATING | id : uint32_t | choice : uint8_t

### Подробности

CONNECT | name : string

	answer:
		SUCCESS

NEW_RATING | name : string | cnt : uint8_t

	answer:
		SUCCESS

DELETE_RATING | id : uint32_t

	answer:
		SUCCESS
		ERROR "There is no such rating"

OPEN_RATING | id : uint32_t

	answer:
		SUCCESS
		ERROR "There is no such rating"

CLOSE_RATING | id : uint32_t

	answer:
		SUCCESS
		ERROR "There is no such rating"
		
ADD_CHOICE | id : uint32_t | choice : string

	answer:
		SUCCESS
		ERROR "There is no such rating id"
		ERROR "The rating is full"

RATING_LIST

	answer:
		RATING_LIST ...

SHOW_RATING | id : uint32_t

	answer:
		RATING_STATS ...
		ERROR "There is no such rating id"

VOTE_RATING | id : uint32_t | choice : uint8_t

	anser:
		SUCCESS
		ERROR "There is no such rating"
		ERROR "There is no such choice option"

## Собщения от сервера:

SUCCESS

ERROR | message : string

DISCONNECT

RATING_LIST | cnt : uint32_t | {name : string | id : uint32_t | status : uint8_t}[cnt]

RATING_STATS | name : string | status : uint8_t | cnt : uint8_t | {name : string | votes : uint32_t}[cnt]

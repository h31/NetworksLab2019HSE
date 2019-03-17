# Протокол для обмена сообщениями в системе "Vacancy service"

## Общие замечания

Все запросы и ответы передаются в формате _json_. Общий вид всех сообщений:
- 4 байта -- размер сообщения (без учёта этих 4-х байт). `uint_32` в формате _little endian_.
- Строка -- тело сообщения в формате _json_

В случае, если во время обработки запроса происходит ошибка (`success != 0`), сервер отвечает стандартным ответом с ошибкой. 

## Описание запросов и ответов

### Добавление специальности

#### Запрос

- `uint32_t requestType = 1`
- `string speciality` -- название специализации

#### Ответ

- `int success = 0`

---

### Добавление вакансии

#### Запрос

- `uint32_t requestType = 2`
- `VacancyInfo vacancy`

Где `VacancyInfo`:
- `int32_t specialityId`
- `string company`
- `string position` (должность)
- `int32_t minAge`
- `int32_t maxAge`
- `int32_t salary`

#### Ответ

- `int success = 0`
- `uint32_t id` (id новой вакансии)

---

### Удаление вакансии

#### Запрос

- `uint32_t requestType = 3`
- `uint32_t id`

#### Ответ

- `int success = 0`

---

### Список специальностей

#### Запрос

- `uint32_t requestType = 4`

#### Ответ

- `int success = 0`
- `[SpecialityInfo] specialities`

Где `SpecialityInfo`:
- `uint32_t id`
- `string speciality`

---

### Список вакансий

#### Запрос

В данном запросе все занчения по-умолчанию (кроме поля `requestType`) означают, что пользователь не ввёл данные для этого поля

- `uint32_t requestType = 5`
- `int32_t specialityId = -1`
- `int32_t age = -1`
- `int32_t salary = -1`

#### Ответ

- `int success = 0`
- `[VacancyInfo] vacancies`

---

### Ответ с ошибкой

- `int success != 0`
- `string cause`

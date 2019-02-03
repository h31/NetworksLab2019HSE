# Протокол для обмена сообщениями в системе "Vacancy service"

## Общие замечания

Все запросы и ответы передаются в формате _json_. Общий вид всех сообщений:
- 4 байта -- размер сообщения (без учёта этих 4-х байт). `uint_32` в формате _little endian_.
- Строка -- тело сообщения в формате _json_

В случае, если во время обработки запроса происходит ошибка, сервер отвечает стандартным ответом с ошибкой. 

## Описание запросов и ответов

### Добавление специальности

#### Запрос

- `uint32_t requestType = 1`
- `string speciality` -- название специализации

#### Ответ

- `bool success = true`

---

### Добавление вакансии

#### Запрос

- `uint32_t requestType = 2`
- `uint32_t id`
- `VacancyInfo vacancy`

Где `VacancyInfo`:
- `string company`
- `string position` (должность)
- `uint32_t minAge`
- `uint32_t maxAge`
- `uint32_t salary`

#### Ответ

- `bool success = true`
- `uint32_t id` (id новой вакансии)

---

### Удаление вакансии

#### Запрос

- `uint32_t requestType = 3`
- `uint32_t id`

#### Ответ

- `bool success = true`

---

### Список специальностей

#### Запрос

- `uint32_t requestType = 4`

#### Ответ

- `bool success = true`
- `[SpecialityInfo] specialities`

Где `SpecialityInfo`:
- `uint32_t id`
- `string speciality`

---

### Список вакансий

#### Запрос

В данном запросе все занчения по-умолчанию (кроме поля `requestType`) означают, что пользователь не ввёл данные для этого поля

- `uint32_t requestType = 5`
- `uint32_t id = -1` (id специальности)
- `uint32_t age = -1`
- `uint32_t salary = -1`

#### Ответ

- `bool success = true`
- `[VacancyInfo] vacancies`

---

### Ответ с ошибкой

- `string cause`

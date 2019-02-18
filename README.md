# Сервис учета ошибок "Bug tracking"

## Формат сообщения

      header: int message type
      body: content depends on message type

## Описание запросов

Разработчики и тестеры идентифицируются по id. Каждая зарегистрированная ошибка также имеет id

- Запрос на авторизацю пользователя

      type: 100
      content: int user_id
 
  - Успех

        type: 101
        content: int user_role
        
  - Пользователь не найден

        type: 102
        content: int user_id

  - Пользователь уже авторизован

        type: 103
        content: int user_id
                 int user_role

- Выдача тестеру списка ошибок: bug_status - 0 - активная, 1 - закрытая

      type: 200
      content: bug_status

  - Успех
  
        type: 201
        content: int number of bugs
                 [
                    int bug_id
                    int project_id
                    int description length
                    char[] description
                 ]

- Подтверждение/отклонение исправления активной ошибки:  verification_code - 0 - отклонение, 1 - подтверждение

      type: 300
      content: int bug_id
               int verification_code
                 
  - Успех

        type: 301
        content: int bug_id
                 int verification_code
        
  - Ошибка уже не активна
        
        type: 302
        content: int bug_status
        
  - Ошибки с таким кодом не существует
        
        type: 303
        content: int bug_id

  - Ошибки еще не исправлена
        
        type: 304
        content: int bug_id
        
  - Неизвстный код верификации
        
        type: 305
        content: int verification_code


- Выдача ошибок разработчику

      type: 400
      content:
      
  - Успех

        type: 401
        content: int number of bugs
                 [
                    int bug_id
                    int description length
                    char[] description
                 ]
  - Неизвестный id разработчика
         
        type: 402
        content: int user_id
        
- Исправления активной ошибки разработчиком

      type: 500
      content: int bug_id
                 
  - Успех

        type: 501
        content: int bug_id
        
  - Задача уже закрыта

        type: 502
        content: int bug_status

  - Задача не найдена

        type: 503
        content: int bug_id

- Прием новой ошибки

      type: 600
      content:
               int bug_id
               int developer_id
               int project_id
               int description length
               char[] description
             
  - Успех

        type: 601
        content: int bug_id
        
  - Ошибка с таким id уже существует

        type: 602
        content: int bug_id
        
- Отключение клиента

      type: 700
      content:
     
- Сообщение о неизвестном типе запроса

      type: 1
      content: int type
      
- Сообщение об отутсвии авторизации

      type: 2
      
- Сообщение об отутсвии прав (неверная роль; 0 - developer, 1 - tester)

      type: 3
      content: required role

# Система передачи файлов

## Формат сообщения

      header: int message type
              int body length
      body:   char[] content

## Описание запросов

Все пути в запросах считаются относительными

- Запрос на загрузку файла

      type: 100
      content: int destination url length
               char[] destination url
               int file size
               char[] file content

  - Успех

        type: 101
        content: char[] destination url

  - Расположение недостуно

        type: 102
        content: char[] destination url

- Запрос на скачивание файла

      type:
      content: char[] file url

  - Успех

        type: 201
        content: char[] content

  - Файл не найден

        type: 202
        content: char[] destination url


- Запрос на получение списка файлов

      type: 300

  - Успех

        type: 301
        content: int number of files
                 [
                    int filename length
                    char[] filename
                 ]                 

- Запрос на перемещение по файловой системе

      type: 400
      content: char[] destination url
      
      
  - Успех

        type: 401
        content: char[] current url

  - Расположение не найдено

        type: 402
        content: char[] destination url

# Система поиска/публикации новостей

## Получение списка тем
Формат запроса:
* <**1**: int>

Формат ответа:
* <**topics_amount**: size_t>(<**topic_length**: size_t><**topic**: string>)*
* **topics_amount** — количество тем
* **topic_length** — длина строки **topic**
* **topic** — название темы

## Получение новостей по теме
Формат запроса:
* <**2**: int><**topic_length**: size_t><**topic**: string>
* **topic_length** — длина строки **topic**
* **topic** — название темы

Формат ответа:
* <**news_amount**: size_t>(<**id**: int><**news_title_length**: size_t><**news_title**: string>)*
* **news_amount** — количество новостей по теме **topic**
* **id** — идентификатор новости
* **news_title_length** — длина строки **news_title**
* **news_title** — тема новости

## Получение текста новости
Формат запроса:
* <**3**: int><**id**: int>
* **id** — идентификатор новости

Формат ответа:
* <**news_content_length**: size_t><**news_content**: string>
* **news_content_length** — длина строки **news_content**, если новость с таким **id** существует, и `0` иначе
* **news_content** — текст новости

## Добавление новости
Формат сообщения:
* <**4**: int><**topic_length**: size_t><**topic**: string><**news_title_length**: size_t><**news_title**: string><**news_content_length**: size_t><**news_content**: string>
* **topic_length** — длина строки **topic**
* **topic** — название темы
* **news_title_length** — длина строки **news_title**
* **news_title** — тема новости
* **news_content_length** — длина строки **news_content**
* **news_content** — текст новости

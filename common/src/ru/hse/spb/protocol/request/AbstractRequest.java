package ru.hse.spb.protocol.request;

import ru.hse.spb.protocol.SerializationException;

/*
 * TODO: по-хорошему, надо было сделать разные иерархии для запросов между
 *  клиентом и сервером и между клиентами, но я и с одной иерахией утомился
 */
public interface AbstractRequest {
    byte getType();

    void accept(RequestVisitor visitor) throws SerializationException;
}

package ru.ifmo.torrent.messages;

import java.io.DataInputStream;
import java.io.IOException;

public abstract class Request<T extends Response> implements NetworkMessage {

    public abstract byte marker();

    public abstract T getEmptyResponse();

    public static Request readFromDataInputStream(DataInputStream in, Class<? extends Request> cls) throws IllegalAccessException, InstantiationException, IOException {
        Request request = cls.newInstance();
        request.read(in);
        return request;
    }

}

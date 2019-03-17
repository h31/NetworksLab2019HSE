package ru.ifmo.torrent.util;

import java.io.PrintStream;

public class TorrentException extends Exception {
    private final String message;
    private final Throwable exception;

    public TorrentException(String message) {
        super(message);
        this.message = message;
        exception = null;
    }

    public TorrentException(String message, Throwable e) {
        super(message);
        this.message = message;
        exception = e;
    }

    public String getMassage() {
        return message;
    }

    public Throwable getException() {
        return exception;
    }

    public void write(PrintStream printer) {
        printer.println(message);
    }
}

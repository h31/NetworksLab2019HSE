package ru.hse.alyokhina.server.exception;

public class NotAuthorizedException extends Exception {
    public NotAuthorizedException(String msg) {
        super(msg);
    }
}

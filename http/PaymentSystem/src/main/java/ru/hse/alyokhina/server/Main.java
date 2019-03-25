package ru.hse.alyokhina.server;

import java.io.IOException;

public class Main {
    public static void main(String[] args) throws IOException {
        final Server server = new Server(8000);
        server.start();
    }
}

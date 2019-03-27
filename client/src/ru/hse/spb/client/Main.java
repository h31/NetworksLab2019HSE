package ru.hse.spb.client;

import java.io.IOException;

public class Main {
    public static void main(String[] args) throws IOException {
        Client client = new Client("localhost");
        client.run();
    }
}

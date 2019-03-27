package ru.spb.hse.httpproxy;

import ru.spb.hse.httpproxy.parser.HTTPMessage;

import java.io.IOException;
import java.util.Arrays;

public class Main {
    public static void main(String[] args) {
        int port = Integer.parseInt(args[0]);
        int time = Integer.parseInt(args[1]);
        int size = Integer.parseInt(args[2]);
        try(ProxyServer server = new ProxyServer(port, time, size)) {
            server.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

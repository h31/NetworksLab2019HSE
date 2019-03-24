package ru.hse.alyokhina.server;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class Server {
    private final int port;
    private final ServerSocket serverSocket;

    public Server(int port) throws IOException {
        this.port = port;
        this.serverSocket = new ServerSocket(port);
    }

    public void start() throws IOException {
        final Socket client = serverSocket.accept();
        final Thread clientThread = new Thread(new ClientHandler(client));
        clientThread.start();
    }

    public int getPort() {
        return port;
    }
}

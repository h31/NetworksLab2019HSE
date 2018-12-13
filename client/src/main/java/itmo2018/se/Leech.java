package itmo2018.se;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;

public class Leech implements Runnable {
    private String metaData;
    private int fileId;
    private Client client;

    public Leech(int fileId, Client client, String workingDir) {
        this.fileId = fileId;
        this.client = client;
        this.metaData = workingDir + "/.metadata";
    }

    @Override
    public void run() {
        try {
            Socket socket = new Socket();
            InetSocketAddress address = client.getSources(fileId).get(0);
            socket.connect(address);
            DataInputStream socketIn = new DataInputStream(socket.getInputStream());
            DataOutputStream socketOut = new DataOutputStream(socket.getOutputStream());
            socketOut.writeByte(1);
            socketOut.writeInt(4);
            socketOut.flush();
        } catch (IOException e) {
            System.out.println("can't connect to seeder");
        }
    }
}

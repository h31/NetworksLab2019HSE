package itmo2018.se;

import com.sun.xml.internal.messaging.saaj.util.ByteOutputStream;

import java.io.*;
import java.net.Socket;
import java.nio.ByteBuffer;

public class Client {
    public static void main(String[] args) throws IOException, InterruptedException {
        Socket socket = new Socket("localhost", 8081);
        DataOutputStream out = new DataOutputStream(socket.getOutputStream());
        out.writeInt(1);
        out.writeByte(10);
        out.flush();
        out.writeInt(2);
        out.writeByte(23);
        out.writeByte(44);
        out.flush();
        socket.close();
    }
}

package itmo2018.se;

import com.sun.xml.internal.messaging.saaj.util.ByteOutputStream;

import java.io.*;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class Client {
    public static void main(String[] args) throws IOException, InterruptedException {
        Socket socket = new Socket("127.0.0.1", 8081);
        ByteArrayOutputStream byteOutput = new ByteArrayOutputStream();
        DataOutputStream byteDataOut = new DataOutputStream(byteOutput);


        DataOutputStream out = new DataOutputStream(socket.getOutputStream());
        System.out.println(socket.getLocalSocketAddress());
        Thread.sleep(4000);
        byteDataOut.writeByte(2);
        byteDataOut.writeUTF("name");
        byteDataOut.writeLong(191);
        System.out.println(byteDataOut.size());
        System.out.println(byteOutput.size());
        out.writeInt(byteDataOut.size());
        out.write(byteOutput.toByteArray());
        out.flush();
        out.writeInt(7);
        out.writeByte(4);
        out.writeShort(0);
        out.writeInt(0);
        out.flush();
        Thread.sleep(4000);
        out.writeInt(1);
        out.writeByte(1);
        out.writeInt(7);
        out.writeByte(4);
        out.writeShort(0);
        out.writeInt(0);
        out.flush();
        out.writeInt(5);
        out.writeByte(3);
        out.writeInt(0);
        Thread.sleep(15000);

//        socket.close();
//        ScheduledExecutorService executorService = Executors.newScheduledThreadPool(1);
//        executorService.schedule(new A(), 10, TimeUnit.SECONDS);
//        executorService.schedule(new B(), 5, TimeUnit.SECONDS);
    }
}

//class A implements Runnable {
//
//    @Override
//    public void run() {
//        System.out.println("A");
//    }
//}
//
//class B implements Runnable {
//
//    @Override
//    public void run() {
//        System.out.println("B");
//    }
//}
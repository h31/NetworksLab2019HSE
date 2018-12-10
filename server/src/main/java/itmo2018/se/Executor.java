//package itmo2018.se;
//
//import java.io.ByteArrayInputStream;
//import java.io.DataInputStream;
//import java.io.DataOutputStream;
//import java.util.concurrent.Callable;
//
//public class Executor implements Callable<Void> {
//    private DataInputStream content;
//
//    public Executor(byte[] bytes) {
//        this.content = new DataInputStream(new ByteArrayInputStream(bytes));
//    }
//
//    @Override
//    public Void call() throws Exception {
//        byte cmd = content.readByte();
//        switch (cmd) {
//            case 4:
//
//        }
//    }
//}

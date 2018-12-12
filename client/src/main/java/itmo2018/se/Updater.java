package itmo2018.se;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class Updater implements Runnable {
    private final DataInputStream socketIn;
    private final DataOutputStream socketOut;

    public Updater(DataInputStream socketIn, DataOutputStream socketOut) {
        this.socketIn = socketIn;
        this.socketOut = socketOut;
    }

    @Override
    public void run() {
        try {
            synchronized (socketOut) {
                socketOut.writeInt(7);
                socketOut.writeByte(4);
                socketOut.writeShort(0);
                socketOut.writeInt(0);
                socketOut.flush();

                socketIn.readByte();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

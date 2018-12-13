package itmo2018.se;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class Updater implements Runnable {
    private final DataInputStream socketIn;
    private final DataOutputStream socketOut;
    private Path metaData;
    private short seederPort;

    public Updater(String workingDir, short seederPort, DataInputStream socketIn, DataOutputStream socketOut) {
        this.socketIn = socketIn;
        this.socketOut = socketOut;
        this.metaData = Paths.get(workingDir + "/.metadata");
        this.seederPort = seederPort;
    }

    @Override
    public void run() {
        try {
            synchronized (socketOut) {
                int filesNumber = (int) Files.lines(metaData).count();
                System.out.println("filesNumber: " + filesNumber);
                socketOut.writeInt(1 + 2 + (filesNumber + 1) * 4);
                socketOut.writeByte(4);
                socketOut.writeShort(seederPort);
                socketOut.writeInt(filesNumber);
                Files.lines(metaData).map(it -> Integer.parseInt(it.split("\t")[0]))
                        .forEach(it -> {
                            try {
                                socketOut.writeInt(it);
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        });
                socketOut.flush();
                socketIn.readByte();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

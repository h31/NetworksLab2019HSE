package itmo2018.se;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.Callable;

public class Executor implements Callable<Void> {
    private DataInputStream content;
    private ClientDataHolder holder;
    private FileManager fileManager;

    public Executor(byte[] bytes, ClientDataHolder holder, FileManager fileManager) {
        this.content = new DataInputStream(new ByteArrayInputStream(bytes));
        this.holder = holder;
        this.fileManager = fileManager;
    }

    @Override
    public Void call() throws Exception {
        byte cmd = content.readByte();
        switch (cmd) {
            case 4:
                executeUpdate();
                break;
        }
        return null;
    }

    private void executeUpdate() throws IOException {
        System.out.println("update");
        ClientInfo client = holder.getClientInfo();
        client.updateCloseTask();
        short port = content.readShort();
        client.updateSharingPort(port);
        int count = content.readInt();
        for (int i = 0; i < count; i++) {
            int fileId = content.readInt();
            fileManager.getFile(fileId).addOwner(client);
        }
        ByteBuffer response = ByteBuffer.allocate(1);
        response.put((byte) 1);
        response.flip();
        holder.addResponse(response);
    }
}

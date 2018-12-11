package itmo2018.se;

import java.util.Arrays;
import java.util.Objects;
import java.util.concurrent.Future;

public class ClientInfo {
    private Future<Void> closeTask;
    private byte[] ip;
    private short port;

    public ClientInfo(byte[] ip, short port, Future<Void> closeTask) {
        this.ip = ip;
        this.port = port;
        this.closeTask = closeTask;
    }

    public byte[] getIp() {
        return ip;
    }

    public short getPort() {
        return port;
    }

    public void updateCloseTask(Future<Void> newCloseTask) {
        closeTask.cancel(false);
        closeTask = newCloseTask;
    }

    public boolean isOnline() {
        return !closeTask.isDone();
    }

    public void disconect() {
        closeTask.cancel(false);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        ClientInfo that = (ClientInfo) o;
        return Arrays.equals(ip, that.ip) &&
                Objects.equals(port, that.port);
    }

    @Override
    public int hashCode() {
        return Objects.hash(ip, port);
    }
}

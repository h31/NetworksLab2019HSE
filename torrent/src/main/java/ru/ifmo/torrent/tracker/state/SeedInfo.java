package ru.ifmo.torrent.tracker.state;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Objects;

public class SeedInfo {

    private final short port;
    private final InetAddress inetAddress;

    public SeedInfo(short port, byte[] IP) throws UnknownHostException {
        this.port = port;
        this.inetAddress = InetAddress.getByAddress(IP);
    }

    public SeedInfo(short port, InetAddress inetAddress) {
        this.port = port;
        this.inetAddress = inetAddress;
    }

    public byte[] getIP() {
        return inetAddress.getAddress();
    }

    public short getPort() {
        return port;
    }

    public InetAddress getInetAddress() {
        return inetAddress;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        SeedInfo seedInfo = (SeedInfo) o;
        return port == seedInfo.port &&
            Objects.equals(inetAddress, seedInfo.inetAddress);
    }

    @Override
    public int hashCode() {
        return Objects.hash(port, inetAddress);
    }

}

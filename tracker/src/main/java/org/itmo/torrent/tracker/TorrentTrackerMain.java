package org.itmo.torrent.tracker;

import java.nio.file.Paths;

public class TorrentTrackerMain {

    private TorrentTrackerMain() {
    }

    public static void main(String[] args) {
        try (TorrentTracker tracker = new TorrentTracker(Paths.get("."))) {
            System.out.println("Press Enter key to shutdown...");
            System.in.read();
            System.exit(0);  // wtf
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

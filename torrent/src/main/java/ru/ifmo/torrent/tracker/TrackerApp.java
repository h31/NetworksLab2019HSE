package ru.ifmo.torrent.tracker;

import ru.ifmo.torrent.util.TorrentException;

import java.io.IOException;
import java.util.Scanner;

public class TrackerApp {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        try (Tracker tracker = new Tracker(TrackerConfig.TRACKER_PORT, TrackerConfig.getMetaDir())) {
            tracker.run();
            System.out.printf("tracker started at getPort %d%n", TrackerConfig.TRACKER_PORT);
            System.out.println("enter 'exit' to shutdown tracker");
            while (scanner.hasNext()) {
                String command = scanner.next();
                if (command.equals("exit")) {
                    System.out.println("shutting down tracker");
                    break;
                }
            }
        } catch (TorrentException e) {
            System.out.println(e.getMassage());
            if(e.getException() != null) e.getException().printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

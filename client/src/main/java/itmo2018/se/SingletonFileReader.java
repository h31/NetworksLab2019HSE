package itmo2018.se;

import java.io.Closeable;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

public class SingletonFileReader {
    private static final Map<String, FileHolder> files = new ConcurrentHashMap<>();

    private SingletonFileReader() {
    }

    public static FileHolder getFile(String path) throws FileNotFoundException {
        if (!files.containsKey(path)) {
            synchronized (files) {
                if (!files.containsKey(path)) {
                    files.put(path, new FileHolder(path));
                } else {
                    files.get(path).referenceCount.incrementAndGet();
                }
            }
        } else {
            files.get(path).referenceCount.incrementAndGet();
        }
        return files.get(path);
    }

    public static class FileHolder implements Closeable {
        private RandomAccessFile file;
        private AtomicInteger referenceCount;
        private String path;

        private FileHolder(String path) throws FileNotFoundException {
            this.file = new RandomAccessFile(path, "r");
            this.referenceCount = new AtomicInteger(1);
            this.path = path;
        }

        public synchronized void read(long offset, byte[] bytes) throws IOException {
            file.seek(offset);
            file.read(bytes);
        }

        public long length() throws IOException {
            return file.length();
        }

        public void close() throws IOException {
            synchronized (files) {
                FileHolder holder = files.get(path);
                if (holder.referenceCount.get() == 1) {
                    holder.file.close();
                    files.remove(path);
                } else {
                    holder.referenceCount.decrementAndGet();
                }
            }
        }
    }
}

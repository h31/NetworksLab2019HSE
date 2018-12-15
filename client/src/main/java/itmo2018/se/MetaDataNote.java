package itmo2018.se;

import java.io.File;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

public class MetaDataNote implements Iterable<Integer> {
    private int id;
    private String name;
    private long size;
    private Set<Integer> parts;
    private boolean finish = false;

    public MetaDataNote(int id, String name, long size) {
        this.id = id;
        this.name = name;
        this.size = size;
        this.finish = true;
    }

    public MetaDataNote(int id, String name, long size, Set<Integer> parts) {
        this.id = id;
        this.name = name;
        this.size = size;
        this.parts = parts;
    }

    public int getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public long getSize() {
        return size;
    }

    public Set<Integer> getParts() {
        if (parts == null) {
            initParts();
        }
        return parts;
    }

    public int partsCount() {
        if (parts == null) {
            initParts();
        }
        return parts.size();
    }

    public boolean isFinish() {
        return finish;
    }

    public boolean existFile() {
        return new File(name).exists();
    }

    @Override
    public Iterator<Integer> iterator() {
        return parts.iterator();
    }

    private void initParts() {
        this.parts = new HashSet<>();
        int partSize = 1024 * 1024 * 5;
        int partsCount = (int) ((size - 1) / partSize + 1);
        for (int i = 0; i < partsCount; i++) {
            parts.add(i);
        }
    }
}

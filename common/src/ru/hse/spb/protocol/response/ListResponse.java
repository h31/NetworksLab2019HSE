package ru.hse.spb.protocol.response;

import ru.hse.spb.protocol.Constants;

import java.util.List;

public class ListResponse implements AbstractResponse {
    public static class FileInfo {
        private final int id;
        private final String name;
        private final long size;

        public FileInfo(int id, String name, long size) {
            this.id = id;
            this.name = name;
            this.size = size;
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

        public long getSizeInMb() {
//            return size / 1024 / 1024;
            return size;
        }

        public int getNumberOfParts() {
            return (int) Math.ceil((double) size / Constants.FILE_PART_SIZE);
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            FileInfo fileInfo = (FileInfo) o;
            return id == fileInfo.id;
        }

        @Override
        public int hashCode() {
            return id;
        }
    }

    private final List<FileInfo> fileInfos;

    public ListResponse(List<FileInfo> fileInfos) {
        this.fileInfos = fileInfos;
    }

    public int getCount() {
        return fileInfos.size();
    }

    public List<FileInfo> getFileInfos() {
        return fileInfos;
    }
}

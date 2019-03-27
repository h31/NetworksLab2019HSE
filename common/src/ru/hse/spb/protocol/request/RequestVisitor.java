package ru.hse.spb.protocol.request;

public interface RequestVisitor {
    void visitGetRequest(GetRequest request);

    void visitListRequest(ListRequest request);

    void visitSourcesRequest(SourcesRequest request);

    void visitStatRequest(StatRequest request);

    void visitUpdateRequest(UpdateRequest request);

    void visitUploadRequest(UploadRequest request);
}

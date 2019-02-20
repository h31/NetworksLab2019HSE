#ifndef BUG_TRACKING_SERVER_BUG_H
#define BUG_TRACKING_SERVER_BUG_H


struct Bug {
    enum BugStatus {
        NEW, QA, FIXED
    };

    uint32_t id;
    uint32_t developer_id;
    uint32_t project_id;
    BugStatus status;
    std::string description;
};


#endif

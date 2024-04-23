#ifndef ERRORS_H_
#define ERRORS_H_

#define MAKE_STATUS_CODE(group, code) (((group * 100L) + (code)))

#define COMMON_GROUP_ID 0U
#define SCHEMA_GROUP_ID 1U
#define OPS_GROUP_ID 3U
#define BP_GROUP_ID 4U

typedef enum {
    kStatus_Success = MAKE_STATUS_CODE(COMMON_GROUP_ID, 0),
    kStatus_Fail = MAKE_STATUS_CODE(COMMON_GROUP_ID, 1),
    kStatus_InvalidArgument = MAKE_STATUS_CODE(COMMON_GROUP_ID, 2),

    kStatus_Schema_UnknownError = MAKE_STATUS_CODE(SCHEMA_GROUP_ID, 0),
    kStatus_Schema_UnknownTableId = MAKE_STATUS_CODE(SCHEMA_GROUP_ID, 1),
    kStatus_Schema_UnknownColumn = MAKE_STATUS_CODE(SCHEMA_GROUP_ID, 2),
} status_t;

#endif
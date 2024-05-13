/**
 * @file errors.h
 * @author Jon McLean (13515869)
 */

#ifndef ERRORS_H_
#define ERRORS_H_

#define MAKE_STATUS_CODE(group, code) (((group * 100L) + (code)))

#define COMMON_GROUP_ID 0U
#define SCHEMA_GROUP_ID 1U
#define OPS_GROUP_ID 3U
#define BP_GROUP_ID 4U
#define IO_GROUP_ID 5U

typedef enum {
    kStatus_Success = MAKE_STATUS_CODE(COMMON_GROUP_ID, 0),
    kStatus_Fail = MAKE_STATUS_CODE(COMMON_GROUP_ID, 1),
    kStatus_InvalidArgument = MAKE_STATUS_CODE(COMMON_GROUP_ID, 2),
    kStatus_AllocError = MAKE_STATUS_CODE(COMMON_GROUP_ID, 3),

    kStatus_Schema_UnknownError = MAKE_STATUS_CODE(SCHEMA_GROUP_ID, 0),
    kStatus_Schema_UnknownTableId = MAKE_STATUS_CODE(SCHEMA_GROUP_ID, 1),
    kStatus_Schema_UnknownColumn = MAKE_STATUS_CODE(SCHEMA_GROUP_ID, 2),
    kStatus_Schema_NoTableWithName = MAKE_STATUS_CODE(SCHEMA_GROUP_ID, 3),

    kStatus_IO_UnknownError = MAKE_STATUS_CODE(IO_GROUP_ID, 0),
    kStatus_IO_FileNotFound = MAKE_STATUS_CODE(IO_GROUP_ID, 1),
    kStatus_IO_BadMagicNumber = MAKE_STATUS_CODE(IO_GROUP_ID, 2),
    kStatus_IO_MissingKey = MAKE_STATUS_CODE(IO_GROUP_ID, 3),
    kStatus_IO_BadKey = MAKE_STATUS_CODE(IO_GROUP_ID, 4),
} status_t;

#endif

/* Purpose: Define sturcts, enums, errors for use wtih the SMB library
 * 
 * Written by Terrance Hutchinson
 * Copyright 2012, Hellfire Storage 
 * contact me with any questions at: terrance.hutchinson@helffirestorage.com
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>


enum SMB_COMMAND {
	SMB2_NEGOTIATE = 0x0000,
	SMB2_SESSION_SETUP = 0x0001,
	SMB2_LOGOFF = 0x0002,
	SMB2_TREE_CONNECT = 0x0003,
	SMB2_TREE_DISCONNECT = 0x0004,
	SMB2_CREATE = 0x0005,
	SMB2_CLOSE = 0x0006,
	SMB2_FLUSH = 0x0007,
	SMB2_READ = 0x0008,
	SMB2_WRITE = 0x0009,
	SMB2_LOCK = 0x000A,
	SMB2_IOCTL = 0x000B,
	SMB2_CANCEL = 0x000C,
	SMB2_ECHO = 0x000D,
	SMB2_QUERY_DIRECTORY = 0x000E,
	SMB2_CHANGE_NOTIFY = 0x000F,
	SMB2_QUERY_INFO = 0x0010,
	SMB2_SET_INFO = 0x0011,
	SMB2_OPLOCK_BREAK = 0x0012
};

enum SMB_FLAGS {
	SMB2_FLAGS_SERVER_TO_REDIR = 0x00000001,
	SMB2_FLAGS_ASYNC_COMMAND = 0x00000002,
	SMB2_FLAGS_RELATED_OPERATIONS = 0x00000004,
	SMB2_FLAGS_SIGNED = 0x00000008,
	SMB2_FLAGS_DFS_OPERATIONS = 0x10000000,
	SMB2_FLGS_REPLAY_OPERATION = 0x20000000 // only valid for SMB3 dialect
};

typedef struct SMB2_Negotiate_Request {
	uint16_t structure_size;
	uint16_t dialect_count;
	uint16_t security_mode;
	uint16_t reserved;
	uint32_t capabilities;
	uint64_t client_guid;
	uint32_t client_start_time;
	
	// The following variable can store one or more 16bit integers that
	// specif the supported SMB revision.
	uint16_t *dialects[2];
}SMB2_NEGOTIATE_REQUEST, PSMB2_NEGOTIATE_REQUEST;

/*
 * The following defines are used to specify the protocols capabilities for the  * server. The capabilities field must use the defines below.
 */
#define SMB2_GLOBAL_CAP_DFS 0x00000001
#define SMB2_GLOBAL_CAP_LEASING 0x00000002
#define SMB2_GLOBAL_CAP_LARGE_MTU 0x00000004
#define SMB2_GLOBAL_CAP_MULTI_CHANNEL 0x00000008
#define SMB2_GLOBAL_CAP_PERSISTENT_HANDLES 0x00000010
#define SMB2_GLOBAL_CAP_DIRECTORY_LEASING 0x00000020
#define SMB2_GLOBAL_CAP_ENCRYPTION 0x00000040

typedef struct SMB2_Negotiate_Response {
	uint16_t structure_size;
	uint16_t security_mode;
	uint16_t dialect_revision;
	uint16_t reserved;
	uint128_t server_guid;
	uint32_t capabilities;	
	uint32_t max_transact_size;
	uint32_t max_read_size;
	uint32_t max_write_size;
	uint64_t system_time; /* must be specified in FILETIME format */
	uint64_t server_start_time; /* Mus be specified in FILETIME format */
	/* The offset (in bytes) from the beginning of the SMB2 header to the 
	 * security buffer
	 */
	uint16_t security_buffer_offset;
	uint16_t security_buffer_length;
	uint32_t reserved;
	/* variable-length buffer, contains security_buffer_offset and
	 * security_buffer_length. The buffer should contain a token as given
	 * by the GSS protocol. If security_buffer_lenght is 0, this field is
	 * then client-initiated authentication with and authentication protocol 	 * of the client's choice, will be used instead of server-initiated
	 * SPNEGO authentication.
	 */
	void *buffer;
}SMB2_NEGOTIATE_RESPONSE, PSMB2_NEGOTIATE_RESPONSE;

/* This define is used in the flag field for the SMB2_Session_Setup_Request 
 * struct
 */
#define SMB2_SESSION_FLAG_BINDING 0x01

/* Defines for use in the Security mode field in SMB2_Session_Setup_Request
 * struct.
 */
#define SMB2_NEGOTIATE_SIGNING_ENABLED 0x01
#define SMB2_NEGOTIATE_SIGNING_REQUIRED 0x02

/* 
 * These defines should be used in the capabilities field in the following
 * struct. The field specifies protocol capabilities for the client. The
 * SMB2_GLOBAL_CAPS_DFS has already been defined earlier in the file. The below
 * defines SHOULD be set to 0 and the server MUST IGNORE them.
 */
#define SMB2_GLOBAL_CAP_UNUSED1 0x00000002
#define SMB2_GLOBAL_CAP_UNUSED2 0x00000004
#define SMB2_GLOBAL_CAP_UNUSED3 0x00000008

/*
 * The SMB2 SESSION_SETUP Reaquest packet is sent by the client to request a new * authenticated session within a new or existing SMB2 protocol transport
 * connection to the server. This request is composed of an SMB2 header.
 */
typedef struct SMB2_Session_Setup_Request {
	uint16_t structure_size;
	/* If the client implements SMB3.0, the flags field MUST be set to
	 * combination of zero or more of the following values. Otherwise it 
	 * MUST be set to 0.
	 */
	uint16_t flags:8;
	uint16_t security_mode:8;
	uint32_t capabilities;
	/* The channel field MUST NOT be used and MUST be reserved. The client
	 * MUST set this to 0 and the server must ignore it on reciept.
	 */
	uint32_t channel;
	uint16_t security_buffer_offset;
	uint16_t security_buffer_length;
	uint64_t previous_session_id;
	/* variable-length buffer, contains security buffer for the request, as
	 * specified by security_buffer_offset and security_buffer_length. If 
	 * server initiated authentication using SPNEGO, the buffer must 
	 * contain a token the GSS protocol. The buffer SHOULD contain a token 
	 * produced by an authentication protocol of client's choice.
	 */
	void *buffer;
}SMB2_SESSION_SETUP_REQUEST, PSMB2_SESSION_SETUP_REQUEST;

// This flag, if set means client has bee auth'd as a guest user
#define SMB2_SESSION_FLAG_IS_GUEST 0x0001
#define SMB2_SESSION_FLAG_IS_NULL 0x0002
#define SMB2_SESSION_FLAG_ENCRYPT_DATA 0x0004

typedef SMB2_Session_Setup_Response {
	uint16_t structure_size; // For server, this field MUST be set to 9
	uint16_t session_flag;
	uint16_t secuirty_buffer_offset;
	uint16_t security_buffer_length;
	/* variable-length, contains security buffer as response. Specified by
	 * security_buffer_offset and security_buffer_length. If server
	 * initiated authentication using SPNEGO, the buffer must contain a
	 * token from the GSS protocol. If the client initiated the 
	 * authentication, the buffer SHOULD contain the token of the client's
	 * choice
	 */
	void *buffer;
}SMB2_SESSION_SETUP_RESPONSE, PSMB2_SESSION_SETUP_RESPONSE;

typedef struct SMB2_Logoff_Request {
	uint16_t structure_size;
	uint16_t reserved;
}SMB2_LOGOFF_REQUEST, PSMB2_LOGOFF_REQUEST;

typedef struct SMB2_Logoff_Response {
	uint16_t structure_size;
	uint16_t reserved;
}SMB2_LOGOFF_RESPONSE, PSMB2_LOGOFF_RESPONSE;

/* The TREE_CONNECT Request packet is sent by a client to request access to a 
 * particular share on the server. The request is composed of an SMB2 Packet
 * Header that is followed by the below struct.
 */
typedef struct SMB2_Tree_Connect_Request {
	uint16_t structure_size;
	uint16_t reserved;
	/* The offset, in bytes, of the full share path name from the beginning
	 * of the header.
	 */
	uint16_t path_offset;
	uint16_t path_length; /* length of path name, in bytes */
	/*
	 * Variable-length buffer that contains the path name of the share in 
	 * Unicode in the form "\\server\share" for the request, as described
	 * by path_offset and path_length. The server component of the path 
	 * MUST be less than 256 characters in length, and it MUST be a NetBIOS
	 * name, FQDN, or a textual IPv4/IPv6 address. The share component of 
	 * the path MUST be less than or equal to 80 characters in length. The 
	 * share name MUST NOT contain invalid characters.
	 */
	void *buffer;
}SMB2_TREE_CONNECT_REQUEST, PSMB2_TREE_CONNECT_REQUEST;

/* SMB2/3 share types */
#define SMB2_SHARE_TYPE_DISK 0x01
#define SMB2_SHARE_TYPE_PIPE 0x02
#define SMB2_SHARE_TYPE_PRINT 0x03

/* SMB2/3 share flags */
#define SMB2_SHAREFLAG_MANUAL_CACHING 0x00000000
#define SMB2_SHAREFLAG_AUTO_CACHING 0x00000010
#define SMB2_SHAREFLAG_VDO_CACHING 0x00000020
#define SMB2_SHAREFLAG_NO_CACHING 0x00000030
#define SMB2_SHAREFLAG_DFS 0x00000001
#define SMB2_SHAREFLAG_DFS_ROOT 0x00000002
#define SMB2_SHAREFLAG_RESTRICT_EXCLUSIVE_OPENS 0x00000100
#define SMB2_SHAREFLAG_FORCE_SHARED_DELETE 0x00000200
#define SMB2_SHAREFLAG_ALLOW_NAMESPACE_CACHING 0x00000400
#define SMB2_SHAREFLAG_ACCESS_BASED_DIR_ENUM 0x00000800
#define SMB2_SHAREFLAG_FORCE_LVL2_OPLOCK 0x00001000
#define SMB2_SHAREFLAG_ENABLED_HASH_V1 0x00002000
#define SMB2_SHAREFLAG_ENABLE_HASH_V2 0x00004000
#define SMB2_SHAREFLAG_ENCRYPT_DATA 0x00008000

/* More SMB2 capabiliy values */
#define SMB2_SHARE_CAP_DFS 0x00000008
#define SMB2_SHARE_CAP_CONTINUOUS_AVAILABILITY 0x00000010
#define SMB2_SHARE_CAP_SCALEOUT 0x00000020
#define SMB2_SHARE_CAP_CLUSTER 0x00000040

/*
 * The SMB2 TREE_CONNECT Response packet is sent by the server when an SMB2 
 * Tree Connect request is processed successfully by the server. The server 
 * MUST set the tree_id of the newly created tree connect in the SMB2 protocol
 * header of the response. The response is composed of an SMB2 Packet header 
 * followed by the response struct below.
 */
typedef struct SMB2_Tree_Connect_Response {
	uint16_t structure_size;
	uint16_t share_type:8;
	uint16_t reserved:8;
	uint32_t share_flags;
	uint32_t capabilities;
	uint32_t maximal_access;
}SMB2_TREE_CONNECT_RESPONSE, PSMB2_TREE_CONNECT_RESPONSE;

/*
 * The SMB2 TREE_DISCONNECT Request packet is sent by the client to request
 * that the tree connect that is specified in the tree_id within the SMB2
 * header be disconnected. This request is composed of an SMB2 header.
 */
typedef struct SMB2_Tree_Disconnect_Request {
	uint16_t structure_size;
	uint16_t reserved;
}SMB2_TREE_DISCONNECT_REQUEST, PSMB2_TREE_DISCONNECT_REQUEST;

/*
 * The response packet is sent by the server to confirm the request was
 * successfully processed. This response is composed of an SMB2 header that is
 * then followed by this structure.
 */
typedef struct SMB2_Tree_Disconnect_Response {
	uint16_t structure_size;
	uint16_t reserved;
}SMB2_TREE_DISCONNECT_RESPONSE, PSMB2_TREE_DISCONNECT_RESPONSE;

/* The different SMB2 OPLOCK types */
#define SMB2_OPLOCK_LEVEL_NON 0x00
#define SMB2_OPLOCK_LEVEL_II 0x01
#define SMB2_OPLOCK_LEVEL_EXCLUSIVE 0x08
#define SMB2_OPLOCK_LEVEL_BATCH 0x09
#define SMB2_OPLOCK_LEVEL_LEASE 0xFF

/* Impersonation level types, used by server to validate creation request */
#define IMPERSONATION_ANONYMOUS 0x00000000
#define IMPERSONATION_IDENTIFICATION 0x00000001
#define IMPERSONATION_IMPERSONATION 0x00000002
#define IMPERSONATION_DELEGATE 0x00000003

/* SMB2 FILE SHARE access values */
#define FILE_SHARE_READ 0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define FILE_SHARE_DELETE 0x00000004

/* SMB2 Disposition values */
// If file exists, supersede it, othwerise create file. NOT for use with 
// printer object
#define FILE_SUPERSEDE 0x00000000
// If the exists, return success; otherwise, create the file. NOT for use with
// printer objects
#define FILE_OPEN 0x00000001
// If file exists, fail operation; otherwise, create the file
#define FILE_CREATE 0x00000002
// Open the file if it exists; otherwise, fail the operation. MUST NOT be 
// used for a printer object
#define FILE_OPEN_IF 0x00000003
// Overwrite the file if it exists; otherwie, create the file. This should
// not be used for a printer object.
#define FILE_OVERWRITE_IF 0x00000004

/* Valid values for the create_options field */
#define FILE_DIRECTORY_FILE 0x000000001
#define FILE_WRITE_THROUGH 0x00000002
#define FILE_SEQUENTIAL_ONLY 0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING 0x00000008
#define FILE_SYNCHRONOUS_IO_ALERT 0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT 0x00000020
#define FILE_NON_DIRECTORY_FILE 0x00000040
#define FILE_COMPLETE_IF_OPLOCKED 0x00000100
#define FILE_NO_EA_KNOWLEDGE 0x00000200
#define FILE_RANDOM_ACCESS 0x00000800
#define FILE_DELETE_ON_CLOSE 0x00001000
#define FILE_OPEN_BY_FILE_ID 0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT 0x00004000
#define FILE_NO_COMPRESSION 0x00008000
#define FILE_RESERVE_OPFILTER 0x00100000
#define FILE_OPEN_REPARSE_POINT 0x00200000
#define FILE_OPEN_NO_RECALL 0x00400000
#define FILE_OPEN_FOR_FREE_SPACE_QUERY 0x00800000

/*
 * The SMB2_Create_Request packet is sent by a client to request either
 * creation of or access to a file. In case of a named pipe or printer, the
 * server MUST create a new file. This request is composed of an SMB2 Packet
 * header and this request structure.
 */
typedef struct SMB2_Create_Request {
	uint16_t structure_size;
	uint16_t security_flags:8;
	uint16_t requested_oplock_lvl:8;
	uint32_t impersonation_lvl;
	uint64_t smb_create_flags; // MUST NOT BE USED, server ignore, client 0
	uint64_t reserved;
	uint32_t desired_access;
	uint32_t file_attr;
	uint32_t share_access;
	uint32_t create_disposition; // defines server required actions
	uint32_t create_options;
	uint16_t name_offset;
	uint16_t name_length;
	uint32_t create_contexts_offset;
	uint32_t create_contexts_length;
	/*
	 * variable-length buffer that contains the unicode file name and
	 * create context list, as defined by name_offset, name_length, 
	 * create_contexts_offset, and create_contexts_length. The buffer 
	 * field MUST be at least 1 byte in length. The file name (after DFS
	 * normalization if needed) MUST conform to the relative pathname 
	 * specified in the MS-FCC specification
	 */
	void *buffer;
}SMB2_CREATE_REQUEST, PSMB2_CREATE_REQUEST;

/* SMB2_ACCESS_MASK_ENCODING
 * The SMB2 Access Mask encoding in SMB2 is a 4-byte bit field value that
 * contains an array of flags. An access mask can specify access for one of
 * two basic groups: either for a file, pipe, or printer or for a directory.
 * Each access mask MUST be a combination of zero or more of the bit positions.
 */
// different mask values
#define FILE_READ_DATA 0x00000001
#define FILE_WRITE_DATA 0x00000002
#define FILE_APPEND_DATA 0x000000004
#define FILE_READ_EA 0x00000008
#define FILE_WRITE_EA 0x00000010
#define FILE_EXECUTE 0x00000020
#define FILE_READ_ATTRIBUTES 0x00000080
#define FILE_WRITE_ATTRIBUTES 0x00000100
#define FILE_DELETE 0x00010000
#define READ_CONTROL 0x00020000
#define WRITE_DAC 0x00040000
#define WRITE_OWNER 0x00080000
#define SYNCRHONIZE 0x00100000
#define ACCESS_SYSTEM_SECURITY 0x01000000
#define MAXIMUM_ALLOWED 0x02000000
#define GENERIC_ALL 0x10000000
#define GENERIC_EXECUTE 0x20000000
#define GENERIC_READ 0x80000000
#define FILE_LIST_DIRECTORY 0x00000001
#define FILE_ADD_FILE 0x00000002
#define FILE_ADD_SUBDIRECTORY 0x00000004
#define FILE_TRAVERSE 0x00000020
#define FILE_DELETE_CHILD 0x00000040

/* FILE_PIPE_PRINTER_ACCESS_MASK */
uint32_t FILE_PIPE_PRINTER_ACCESS_MASK;
/* DIRECTORY_ACCESS_MASK */
uint32_t DIRECTORY_ACCESS_MASK;

/* valid Create Context values */
#define SMB2_CREATE_EA_BUFFER 0x45787441
#define SMB2_CREATE_SD_BUFFER 0x53656344
#define SMB2_CREATE_DURABLE_HANDLE_REQUEST 0x44486e51
#define SMB2_CREATE_DURABLE_HANDLE_RECONNECT 0x44486e43
#define SMB2_CREATE_ALLOCATION_SIZE 0x416c5369
#define SMB2_CREATE_QUERY_MAXIMAL_ACCESS_REQUEST 0x4d784163
#define SMB2_CREATE_TIMEWARP_TOKEN 0x54577270
#define SMB2_CREATE_QUERY_ON_DISK_ID 0x51466964
#define SMB2_CREATE_REQUEST_LEASE 0x52714c73
#define SMB2_CREATE_REQUEST_LEASE_V2 0x52714c73
#define SMB2_CREATE_DURABLE_HANDLE_REQUEST_V2 0x44483251
#define SMB2_CREATE_DURABLE_HANDLE_RECONNECT_V2 0x44483243
#define SMB2_CREATE_APP_INSTANCE_ID 0x45BCA66AEFA7F74A9008FA462E144D74

/* SMB2_Create_Context_Request
 * This structure is used by SMB2_Create_Request and the SMB2_Create_Response
 * to encode additional flags and attributes: in requests to specify ow the 
 * CREATE request MUST be processed, and in responses to specify how the CREATE
 * request was in fact processed. There is no required ordering when the
 * multiple Create_Context structures are used. Server MUST support recieving
 * the contexts in any order. (FIFO) 
 */
typedef struct SMB2_Create_Context_Request {
	uint32_t next;
	uint16_t name_offset;
	uint16_t name_length;
	uint16_t reserved;
	uint16_t data_offset;
	uint16_t data_length;
	/*
	 * variable-length buffer that contains the name and data fields, as
	 * defined by name_offset, name_length, data_offset, and data_length.
	 * The name is represented as four or more octets and MUST be one of
	 * the context values defined above. The structure name indicates what
	 * info is encoded by the data payload.
	 */
	void *buffer;
}SMB2_CREATE_CONTEXT_REQUEST, PSMB2_CREATE_CONTEXT_REQUEST;

typedef struct SMB2_Create_Durable_Handle_Request {
	/* 16 byte field that MUST NOT be used and MUST be reserved. Must be
	 * set to 0 by client and ignored by server.
	 */
	uint128_t durable_request;
}SMB2_CREATE_DURABLE_HANDLE_REQUEST, PSMB2_CREATE_DURABLE_HANDLE_REQUEST;

typedef struct SMB2_Create_Durable_Handle_Request {
	SMB2_FILEID data; // needs to be an SMB2_FILEID structure
}SMB2_CREATE_DURABLE_HANDLE_REQUEST, PSMB2_CREATE_DURABLE_HANDLE_REQUEST;

typedef struct SMB2_Create_Durable_Handle_Reconnect {
	SMB2_FILEID data;
}SMB2_CREATE_DURABLE_HANDLE_RECONNECT, PSMB2_CREATE_DURABLE_HANDLE_RECONNECT;

/*
 * When the client is requesting the server to retrieve maximal access info as
 * part of processing the open. The Data in the buffer field of the
 * SMB2_CREATE_CONTEXT MUST either contain the following structure or empty
 */
typedef struct SMB2_Create_Query_Maximal_Access_Request {
	uint64_t timestamp;
}SMB2_CREATE_QUERY_MAX_ACCESS_REQUEST, PSMB2_CREATE_QUERY_MAX_ACCESS_REQUEST;

/*
 * The SMB2_CREATE_ALLOCATION_SIZE context is specified on an
 * SMB2_CREATE_REQUEST when the client is setting the allocation size of a file
 * that is being newly created or overwritten. The data in the buffer field of 
 * the SMB2_CREATE_CONTEXT MUST be the following structure.
 */
typedef struct SMB2_Create_Allocation_Size {
	uint32_t allocation_size;
}SMB2_CREATE_ALLOCATION_SIZE, PSMB2_CREATE_ALLOCATION_SIZE;

/*
 * The SMB2_CREATE_TIMEWARP_TOKEN context is specified on an
 * SMB2_CREATE_REQUEST when the client is requesting the server to open a
 * version of the file at a previous point in time. The Data in the buffer
 * field of the SMB2_CREATE_CONTEXT MUST contain the following structure.
 */
typedef struct SMB2_Create_Timewarp_Token {
	uint32_t timestamp;
}SMB2_CREATE_TIMEWARP_TOKEN, PSMB2_CREATE_TIMEWARP_TOKEN;

/*
 * The SMB2_CREATE_REQUEST_LEASE context is specfied on an SMB2_CREATE_REQUEST
 * packet when the client is requesting the server to return a lease. The 
 * value is not valid for the SMB 2.002 dialect. The data in the buffer field
 * of the SMB2_CREATE_CONTEXT structure MUST contain the following structure.
 */
#define SMB2_LEASE_NONE 0x00
#define SMB2_LEASE_READ_CACHING 0x01
#define SMB2_LEASE_HANDLE_CACHING 0x02
#define SMB2_LEASE_WRITE_CACHING 0x04

typedef struct SMB2_Create_Request_Lease {
	uint128_t lease_key;
	uint32_t lease_state;
	uint32_t lease_flags;
	uint64_t lease_duration;
}SMB2_CREATE_REQUEST_LEASE, PSMB2_CREATE_REQUEST_LEASE;

/*
 * The SMB2_CREATE_REQUEST_LEASE_V2 context is specified on an
 * SMB2_CREATE_REQUEST when the client is requesting the server to return
 * a lease on a file or a directory. This is only valid for the SMB3.0 dialect.
 * The data is the buffer field of the SMB2_CREATE_CONTEXT structure MUST
 * contain the following structure.
 */
#define SMB2_LEASE_FLAG_PARENT_LEASE_KEY_SET 0x00000004

typedef struct SMB2_Create_Request_Lease_V2 {
	uint128_t lease_key;
	uint32_t lease_state;
	uint32_t flags;
	uint64_t lease_duration;
	uint128_t parent_lease_key;
	uint16_t epoch;
	uint16_t reserved;
}SMB2_CREATE_REQUEST_LEASE_V2, PSMB2_CREATE_REQUEST_LEASE_V2;

/*
 * The SMB2_CREATE_DURABLE_HANDLE_REQUEST_V2 context is only valid for the 
 * SMB3.0 dialect. When the client is not requesting a persistant handle, the
 * client SHOULD also request a batch OPLOCK or a handle caching lease. The
 * format of the data in the buffer field for this SMB2_CREATE_CONTEXT MUST be
 * as the following struct.
 */
#define SMB2_DHANDLE_FLAG_PERSISTENT 0x00000002

typedef struct SMB2_Create_Durable_Handle_Request_V2 {
	uint32_t timeout;
	uint32_t flags;
	uint64_t reserved;
	uint128_t create_guid;
}SMB2_CREATE_DHANDLE_REQUEST_V2, PSMB2_CREATE_DHANDLE_REQUEST_V2;

/*
 * The SMB2_CREATE_DURABLE_HANDLE_RECONNECT_V2 context is specified when the 
 * client is attempting to reestablish a durable open. The 
 * SMB2_CREATE_DURABLE_HANDLE_RECONNECT_V2 context is only valid for the SMB3.0
 * dialect.
 */
typedef struct SMB2_Create_Durable_Handle_Reconnect_V2 {
	SMB2_FILEID file_id;
	uint128_t create_guid;
	uint16_t flags;
}SMB2_CREATE_DHANDLE_RECONNECT_V2, PSMB2_CREATE_DHANDLE_RECONNECT_V2;

/*
 * The SMB2_CREATE_APP_INSTANCE_ID context is specified on an 
 * SMB2_CREATE_REQUEST when the client is supplying an identifier provided
 * by an application. The SMB2_CREATE_APP_INSTANCE_ID context is only valid for
 * the SMB3.0 dialect. The client should also request a durable handle by using
 * an SMB2_CREATE_DURABLE_HANDLE_REQUEST_V2 or
 *  SMB2_CREATE_DURABLE_HANDLE_RECONNECT_V2 create context.
 */
typedef struct SMB2_Create_App_Instance_Id {
	uint16_t structure_size:8;
	uint16_t reserved:8;
	uint128_t app_instance_id;
}SMB2_CREATE_APP_INSTANCE_ID, PSMB2_CREATE_APP_INSTANCE_ID;

/*
 * The SMB2 CREATE response packet is sent by the server to notify the client of
 * the client of the status of its SMB2_CREATE_REQUEST. This response is 
 * composed of an SMB2 header followed by this response structure.
 */
#define SMB2_CREATE_FLAG_REPARSEPOINT 0x01

typedef struct SMB2_Create_Response {
	uint16_t structure_size; // The server MUST set this field to 89
	uint16_t oplock_lvl:8;
	uint16_t flags:8;
	uint32_t create_action;
	uint64_t creation_time;
	uint64_t last_access_time;
	uint64_t last_write_time;
	uint64_t change_time;
	uint64_t allocation_size;
	uint64_t end_of_file;
	uint32_t file_attributes;
	uint32_t reserved2;
	SMB2_FILEID file_id;
	uint32_t create_contexts_offset;
	uint32_t create_contexts_length;
	/*
	 * variable-length buffer that contains the list of create contexts
	 * that are contained in this response, as described by
	 * create_contexts_offset and create_contexts_length. This takes the
	 * form of a list SMB2_CREATE_CONTEXT_RESPONSE values.
	 */
	void *buffer;
}SMB2_CREATE_RESPONSE, PSMB2_CREATE_RESPONSE;

/*
 * The SMB2 FILEID is used to represent an open to a file
 */
typedef struct SMB2_File_Id {
	/* A file handle that remains persistent when an open is reconnected
	 * after being lost on a disconnect. The server MUST return this file
	 * handle as part of an SMB2_CREATE_RESPONSE. If the open is a durable
	 * open, this value MUST be globally unique. If the open is not a
	 * durable open, this value MUST be unique for all persistent handles
	 * on that SMB2 transport connection.
	 */
	uint64_t persistent;
	/* A file handle that can be changed when an open is reconnected
	 * after being lost on a disconnect. The server MUST return this file
	 * handle as part of an SMB2_CREATE_RESPONSE. This value MUST NOT
	 * change unless a reconnection is performed. This value MUST be
	 * unique for all volatile handles on the SMB2 transport connection.
	 */
	uint64_t volatile;
}SMB2_FILE_ID, PSMB2_FILE_ID;



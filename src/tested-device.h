// tested-device.h
#ifndef TESTED_DEVICE_H
#define TESTED_DEVICE_H

#if !defined CONFIG_UUID_HIGH || !defined CONFIG_UUID_LOW
#error ERROR !! PLEASE PLEASE PUT FS UUID PLEASE
#endif

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>

#define J3_I(a,b,c) a##b##c
#define J3(a,b,c) J3_I(a,b,c)

#define UUID_HIGH	J3(0x,CONFIG_UUID_HIGH,ULL)
#define UUID_LOW	J3(0x,CONFIG_UUID_LOW,ULL)

#ifdef __FILE_NAME__
#define FILE_NAME __FILE_NAME__
#else
#define FILE_NAME __FILE__
#endif

#define S_I(x) #x
#define S(x) S_I(x)

#define LOG(f, ...) ((void)printf("<" FILE_NAME ":" S(__LINE__) "> " f "\n", ##__VA_ARGS__))

#define ERR(f, ...) do { \
	LOG(f, ##__VA_ARGS__); \
	return -1; \
} while (0)

#define ERRIF(b, f, ...) do { \
	if ((b)) \
		ERR(f, ##__VA_ARGS__); \
} while (0)

#define ERRIFEQ(a, b, f, ...) ERRIF((a) == (b), f, ##__VA_ARGS__)
#define ERRIFNEQ(a, b, f, ...) ERRIF((a) != (b), f, ##__VA_ARGS__)

#define CERR(f, ...) do { \
	LOG(f " | %s", ##__VA_ARGS__, strerror(errno)); \
	return -1; \
} while (0)

#define CERRIF(b, f, ...) do { \
	if ((b)) \
		CERR(f, ##__VA_ARGS__); \
} while (0)

#define CERRIFEQ(a, b, f, ...) CERRIF((a) == (b), f, ##__VA_ARGS__)
#define CERRIFNEQ(a, b, f, ...) CERRIF((a) != (b), f, ##__VA_ARGS__)

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef ssize_t isize;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;

#define EXT2_MAGIC 0xef53

typedef u8 uuid_t[16];

struct ext2 {
	u32 inodes_count;
	u32 blocks_count;
	u32 r_blocks_count;
	u32 free_blocks_count;
	u32 free_inodes_count;
	u32 first_data_block;
	u32 log_block_size;
	u32 log_cluster_size;
	u32 blocks_per_group;
	u32 clusters_per_group;
	u32 inodes_per_group;
	u32 mtime;
	u32 wtime;
	u16 mnt_count;
	u16 max_mnt_count;
	u16 magic; // IMPORTANT
	u16 state;
	u16 errors;
	u16 minor_rev_level;
	u32 lastcheck;
	u32 checkinterval;
	u32 creator_os;
	u32 rev_level;
	u16 def_resuid;
	u16 s_def_resgid;
	u32 first_ino;
	u16 inode_size;
	u16 block_group;
	u32 feature_compat;
	u32 feature_incompat;
	u32 feature_ro_compat;
	uuid_t uuid; // IMPORTANT
	// don't need anything after this
};

// switchroot.c
i32 switch_root(void);

#endif


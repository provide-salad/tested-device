// start.c
#include "tested-device.h"

#include <unistd.h>
#include <dirent.h>
#include <endian.h>
#include <stdio.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/mount.h>

static i32 try_dir(DIR *const dir, const int devfs_fd, const int devfd, const char name[const]) {
	if (lseek(devfd, 0x400, SEEK_SET) == -1)
		return 0;
	struct ext2 superblock;
	const isize bytes = read(devfd, &superblock, sizeof(superblock));
	if (bytes < (isize)sizeof(superblock))
		return 0;
	if (le16toh(superblock.magic) != EXT2_MAGIC)
		return 0;
	u64 expected_uuid[2] = { htobe64(UUID_LOW), htobe64(UUID_HIGH) };
	if (memcmp(expected_uuid, superblock.uuid, sizeof(uuid_t)))
		return 0;
	CERRIFEQ(-1, close(devfd), "Failed to close '/dev/%s' | (&%d)/'%s' | (&%d)", name, devfs_fd, name, devfd);
	CERRIFEQ(-1, mkdir("/newroot", 0770), "Failed to mkdir '/newroot'");
	CERRIFEQ(-1, fchdir(devfs_fd), "Failed to chdir into '/dev' (&%d)", devfs_fd);
	CERRIFEQ(-1, closedir(dir), "Failed to closedir '/dev' (&%d)", devfs_fd);
	CERRIFEQ(-1, mount(name, "/newroot", "ext4", MS_RELATIME, NULL),
		"Failed to mount '/dev/%s' | (&%d)/'%s' | (&%d) on '/newroot'",
		name, devfs_fd, name, devfd);
	CERRIFEQ(-1, chdir("/"), "Failed to chdir into '/'");
	(void)switch_root();
	CERR("Failed to switch_root into '/newroot'");
}

static i32 init(void) {
	const int devfs_fd = open("/dev", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
	CERRIFEQ(-1, devfs_fd, "Failed to open '/dev' for reading directory");
	DIR *const dir = fdopendir(devfs_fd);
	CERRIFEQ(NULL, dir, "Failed to opendir '/dev' (&%d)", devfs_fd);
	for (struct dirent *dent = readdir(dir); dent != NULL; dent = readdir(dir)) {
		if (strncmp("sd", dent->d_name, 2))
			continue;
		struct stat st;
		CERRIFEQ(-1, fstatat(devfs_fd, dent->d_name, &st, AT_SYMLINK_NOFOLLOW),
			"Failed to stat '/dev/%s' | (&%d)/'%s'",
			dent->d_name, devfs_fd, dent->d_name);
		if (!S_ISBLK(st.st_mode))
			continue;
		const int devfd = openat(devfs_fd, dent->d_name, O_RDONLY | O_CLOEXEC);
		CERRIFEQ(-1, devfd, "Failed to open '/dev/%s' | (&%d)/'%s' for reading",
			dent->d_name, devfs_fd, dent->d_name);
		char name[sizeof(dent->d_name)];
		strncpy(name, dent->d_name, sizeof(dent->d_name));
		ERRIF(try_dir(dir, devfs_fd, devfd, name),
			"Failed to chainload '/dev/%s' | (&%d)/'%s' | (&%d)",
			name, devfs_fd, name, devfd);
		CERRIFEQ(-1, close(devfd), "Failed to close '/dev/%s' | (&%d)/'%s' | (&%d)",
			dent->d_name, devfs_fd, name, devfd);
	}

	(void)closedir(dir);
	ERR("Failed to find EXT2 filesystem with UUID { " S(UUID_HIGH) ", " S(UUID_LOW) " }");
}

int main(void) {
	(void)init();
	while (1) {
		i32 x;
		LOG("FAILSAFE | If this is init it will crash if your terminate");
		(void)read(STDIN_FILENO, &x, 1);
	}
}



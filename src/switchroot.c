// switchroot.c
// This is basically what busybox does, right?
#include "tested-device.h"

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/mount.h>

__attribute__ ((noinline))
static i32 maybe_stat(dev_t *const dev_ptr, mode_t *const mode_ptr, const int fd, const char name[const]) {
	struct stat st;
	CERRIFEQ(-1, fstatat(fd, name, &st, AT_SYMLINK_NOFOLLOW),
		"Failed to stat (&%d)/'%s'", fd, name);
	*dev_ptr = st.st_dev;
	*mode_ptr = st.st_mode;
	return 0;
}

static i32 del_recursive(const int dir_fd, const dev_t root_dev, i32 *const deleted_all_ptr) {
	DIR *const dir = fdopendir(dir_fd);
	CERRIFEQ(NULL, dir, "Failed to opendir (&%d)", dir_fd);
	i32 parent_deleted_all = 1;
	for (const struct dirent *dent = readdir(dir); dent != NULL; dent = readdir(dir)) {
		if (dent->d_name[0] == '.' && (dent->d_name[1] == 0 || !strcmp(".", &dent->d_name[1])))
			continue;
		dev_t st_dev;
		mode_t st_mode;
		CERRIF(maybe_stat(&st_dev, &st_mode, dir_fd, dent->d_name),
			"Failed to maybe_stat (&%d)/'%s'", dir_fd, dent->d_name);
		if (st_dev != root_dev) {
			parent_deleted_all = 0;
			continue;
		}

		if (!S_ISDIR(st_mode)) {
			CERRIFEQ(-1, unlinkat(dir_fd, dent->d_name, 0),
				"Failed to unlink (&%d)/'%s'", dir_fd, dent->d_name);
			continue;
		}

		const int fd = openat(dir_fd, dent->d_name, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
		CERRIFEQ(-1, fd, "Failed to open (&%d)/'%s' for reading directory", dir_fd, dent->d_name);
		int child_deleted_all;
		CERRIF(del_recursive(fd, root_dev, &child_deleted_all), // !!
			"Failed to recursively delete (&%d)/'%s' (&%d)", dir_fd, dent->d_name, fd);
		if (!child_deleted_all) {
			parent_deleted_all = 0;
			continue;
		}

		CERRIFEQ(-1, unlinkat(dir_fd, dent->d_name, AT_REMOVEDIR),
			"Failed to rmdir (&%d)/'%s'", dir_fd, dent->d_name);
	}

	CERRIFEQ(-1, closedir(dir), "Failed to closedir (&%d)", dir_fd);
	if (deleted_all_ptr != NULL)
		*deleted_all_ptr = parent_deleted_all;
	return 0;
}

i32 switch_root(void) {
	const pid_t pid = getpid();
	CERRIFEQ(-1, pid, "Failed to getpid");
	ERRIFNEQ(1, pid, "PID is not 1. Goodbye");
	const int root_fd = open("/", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
	CERRIFEQ(-1, root_fd, "Failed to open '/' for reading directory");
	struct stat root_stat;
	CERRIFEQ(-1, fstat(root_fd, &root_stat), "Failed to stat '/' (&%d)", root_fd);
	ERRIF(del_recursive(root_fd, root_stat.st_dev, NULL), "Failed to recursively delete '/' (&%d)", root_fd);
	CERRIFEQ(-1, chdir("/newroot"), "Failed to chdir into '/newroot'");
	CERRIFEQ(-1, mount(".", "/", NULL, MS_MOVE, NULL), "Failed to move_mount '/' to '/newroot'");
	CERRIFEQ(-1, chroot("."), "Failed to chroot into '/newroot'");
	CERRIFEQ(-1, chdir("/"), "Failed to chdir into '/' ('/newroot')");
	(void)execl("/sbin/init", "/sbin/init", NULL);
	CERR("Failed to execl '/sbin/init' ('/newroot/sbin/init')");
}


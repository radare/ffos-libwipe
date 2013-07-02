/*
 * libwipe
 *
 * NOTE: This is an incomplete version of libwipe. It's just for testing
 *       purposes, so, it works, but it just zeroes the file, this is not
 *       a secure deletion method, and it just wraps the unlink() syscall.
 *
 * TODO: Implement better wiping algorithms
 *       Handle ^C :) sigign? or just warn 2 times? or prompt?
 *       Use mmap instead of open/lseek/write/close
 *       Wrap truncate(2) and ftruncate(2)
 *       Wrap rmdir(?)
 *       Make use of the errno
 *       Use environ for runtime configuration
 *         LIBWIPE_METHOD
 *         LIBWIPE_VERBOSE
 *         LIBWIPE_DISABLE
 *         ...
 *
 * BUILD: gcc -shared -ldl libwipe.c -o libwipe.so
 *
 * INSTALL: cp libwipe.so /lib && echo /lib/libwipe.so >> /etc/ld.so.preload
 *
 * author: pancake <pancake@youterm.com>
 * description: wipe as a library
 * 
 */

#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static int verbose = 1;

#define p(x) write(2,x,strlen(x))
#define pf(x,y) fprintf(stderr,x,y)

static int *(*__REAL_unlink__)(const char *pathname) = NULL;

static void _libwipe_init() __attribute__ ((constructor));
static void _libwipe_init() {   
	char *ptr = NULL;
	if (getenv != NULL)
		getenv ("LIBWIPE_VERBOSE");
	if (ptr) verbose = atoi (ptr);
	if (!__REAL_unlink__) {
		__REAL_unlink__ = dlsym (RTLD_NEXT, "unlink");
		pf ("[libwipe] dlsym unlink %p\n", __REAL_unlink__);
		if (__REAL_unlink__ == NULL) {
			p ("[libwipe] ERROR Library initialization failed.\n");
			//exit(1);
		}
	}
}

#if 0
static void _libwipe_fini() __attribute__ ((destructor));
static void _libwipe_fini() {
	/* do something here */
}
#endif

static inline void randset(void *buf, size_t size) {
	int fd = open ("/dev/urandom",O_RDONLY);
	if (fd != -1) {
		read (fd, buf, size);
		close (fd);
	} else {
		memset (buf, 0, size);
	}
}

static int do_wipe(const char *pathname, blksize_t size) {
	off_t offset;
	char buf[4096];
	register int i;
	int fd;

	if (size<1) size = 4096;
	randset ((void *)buf, (size_t)size);

	fd = open (pathname, O_WRONLY);
	if (fd>=0) {
		offset = lseek (fd, 0, SEEK_END);
		if (offset<1) {
			pf ("[libwipe] pathname is %s\n", pathname);
			pf ("[libwipe] Invalid filesize %ld bytes.\n", offset);
			close (fd);
			return -1;
		}
		fprintf (stderr, "[libwipe] Wiping %s (%ld bytes).\n", pathname, offset);
		lseek (fd, 0, SEEK_SET);
		for (i=0; i<offset; i+=size)
			write (fd, buf, size);
		close (fd);
		sync ();
		//puts("[libwipe] done\n");
	} else {
		pf ("[libwipe] ERROR Cannot open '%s' for writing.\n", pathname);
		// only for SECURE mode,
		return -1;
	}
	return 0;
}

int unlink (const char *pathname) {
	struct stat st;

	if (!__REAL_unlink__)
		_libwipe_init ();
	lstat (pathname, &st);
	//printf("%x %d %d\n", st.st_mode, S_ISLNK(st.st_mode), S_ISREG(st.st_mode));
	if (S_ISREG (st.st_mode)==1) {
		pf ("[libwipe] wipe %s\n", pathname);
		if (do_wipe (pathname, st.st_blksize)) {
			p ("[libwipe] wipe has failed\n");
			return -1;
		}
	}
	if (__REAL_unlink__) {
		int ret = (int)(__REAL_unlink__ (pathname));
		pf ("[libwipe] rm %s\n", pathname);
		return ret;
	}
	pf ("[libwipe] real unlink not resolved %p\n", __REAL_unlink__);
	return 0;
}

/* Unit test for skynet_daemon write_pid failure path.
 *
 * When fprintf to the pidfile fails, the FILE* opened with fdopen() must be
 * closed (which also closes the underlying fd).  The previous code called
 * close(fd) directly, leaking the stdio buffer and risking a double close.
 *
 * We force fprintf to fail by making the stream unbuffered through a macro
 * override of fdopen (portable; no GNU ld --wrap needed) and pointing the
 * pidfile at /dev/full, where every write fails.  write_pid must then return
 * 0 (the failure path) without crashing.
 */
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

/* Override fdopen inside skynet_daemon.c so its streams are unbuffered. */
FILE *test_daemon_fdopen(int fd, const char *mode);
#define fdopen test_daemon_fdopen
#include "../skynet-src/skynet_daemon.c"
#undef fdopen

FILE *
test_daemon_fdopen(int fd, const char *mode) {
	FILE *f = fdopen(fd, mode);
	if (f) {
		setvbuf(f, NULL, _IONBF, 0);
	}
	return f;
}

int
main(void) {
	int fd = open("/dev/full", O_RDWR);
	if (fd == -1) {
		printf("daemon: /dev/full unavailable, skipped\n");
		return 0;
	}
	close(fd);

	int rc = write_pid("/dev/full");
	assert(rc == 0);

	printf("daemon: all assertions passed\n");
	return 0;
}

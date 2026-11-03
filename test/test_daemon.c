/* Unit test for skynet_daemon write_pid failure path.
 *
 * When fprintf to the pidfile fails, the FILE* opened with fdopen() must be
 * closed (which also closes the underlying fd).  The previous code called
 * close(fd) directly, leaking the stdio buffer and risking a double close.
 *
 * We force fprintf to fail by wrapping fdopen to make the stream unbuffered
 * and pointing the pidfile at /dev/full, where every write fails.  write_pid
 * must then return 0 (the failure path) without crashing.
 */
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

extern FILE *__real_fdopen(int fd, const char *mode);

FILE *
__wrap_fdopen(int fd, const char *mode) {
	FILE *f = __real_fdopen(fd, mode);
	if (f) {
		setvbuf(f, NULL, _IONBF, 0);
	}
	return f;
}

#include "../skynet-src/skynet_daemon.c"

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

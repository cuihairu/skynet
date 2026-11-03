/* Unit test for service_logger: a failed SIGHUP reopen must not leave the
 * logger with a NULL handle.
 *
 * logger_cb(PTYPE_SYSTEM) used freopen() and assigned the result straight to
 * inst->handle.  freopen returns NULL on failure (and closes the old stream),
 * so a failed reopen left a NULL handle; the next PTYPE_TEXT write then ran
 * fprintf/fwrite through a NULL FILE*, which is undefined behaviour.
 *
 * The fix opens a fresh stream and swaps it in only on success, keeping the
 * previous handle valid on failure.  This test opens a real log file, forces
 * a reopen against an unwritable path, and verifies the handle is still valid
 * and a subsequent text write reaches the original file.
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct skynet_context;
typedef int (*skynet_cb)(struct skynet_context *, void *, int, int, uint32_t, const void *, size_t);

/* Stubs for symbols referenced by service_logger.c. */
void skynet_error(struct skynet_context *ctx, const char *fmt, ...) {
	(void)ctx;
	(void)fmt;
}
uint64_t skynet_now(void) { return 0; }
const char * skynet_command(struct skynet_context *ctx, const char *cmd, const char *parm) {
	(void)ctx; (void)cmd; (void)parm;
	static const char * zero = "0";
	return zero;
}
void skynet_callback(struct skynet_context *ctx, void *ud, skynet_cb cb) {
	(void)ctx; (void)ud; (void)cb;
}

#include "../service-src/service_logger.c"

int
main(void) {
	char path[] = "/tmp/skynet_logger_test_XXXXXX";
	int fd = mkstemp(path);
	assert(fd >= 0);
	close(fd);

	struct logger * inst = logger_create();
	assert(inst);
	assert(logger_init(inst, NULL, path) == 0);

	/* Force a reopen against a path that cannot be opened.  The old stream
	 * must stay valid. */
	logger_cb(NULL, inst, PTYPE_SYSTEM, 0, 0, "/nonexistent/dir/log", 0);
	assert(inst->handle != NULL);

	/* Writing after the failed reopen must still land in the original file. */
	const char * msg = "post-failed-reopen";
	logger_cb(NULL, inst, PTYPE_TEXT, 0, 0, msg, strlen(msg));

	fflush(inst->handle);
	assert(ferror(inst->handle) == 0);
	logger_release(inst);

	/* The message must be present in the file. */
	FILE * f = fopen(path, "r");
	assert(f);
	char buf[256];
	int got = 0;
	while (fgets(buf, sizeof(buf), f)) {
		if (strstr(buf, msg)) {
			got = 1;
			break;
		}
	}
	fclose(f);
	unlink(path);
	assert(got);

	printf("logger: all assertions passed\n");
	return 0;
}

/* Unit test for skynet_error: the cached logger handle must be accessed
 * atomically.
 *
 * The previous code used a plain static uint32_t for the logger handle,
 * so concurrent skynet_error() calls from multiple threads could race on
 * the read/write.  This test hammers skynet_error() from several threads
 * while making skynet_handle_findname return a valid handle, and asserts
 * that the message is pushed exactly once per call with the same handle.
 */
#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdatomic.h>

#define THREADS 8
#define CALLS 1000

static atomic_int find_calls = 0;
static atomic_int push_count = 0;
static atomic_int push_handle_ok = 1;

#define LOG_MESSAGE_SIZE 256
#include "../skynet-src/skynet_error.c"

uint32_t skynet_handle_findname(const char *name) {
	(void)name;
	atomic_fetch_add(&find_calls, 1);
	return 0x1234;
}

int skynet_context_push(uint32_t handle, struct skynet_message *message) {
	(void)message;
	if (handle != 0x1234) {
		atomic_store(&push_handle_ok, 0);
	}
	atomic_fetch_add(&push_count, 1);
	return 0;
}

uint32_t skynet_context_handle(struct skynet_context *ctx) { (void)ctx; return 0; }

static void *
worker(void *arg) {
	(void)arg;
	for (int i = 0; i < CALLS; i++) {
		skynet_error(NULL, "msg %d", i);
	}
	return NULL;
}

int
main(void) {
	pthread_t th[THREADS];
	for (int i = 0; i < THREADS; i++) {
		pthread_create(&th[i], NULL, worker, NULL);
	}
	for (int i = 0; i < THREADS; i++) {
		pthread_join(th[i], NULL);
	}

	assert(push_count == THREADS * CALLS);
	assert(push_handle_ok == 1);

	printf("error: all assertions passed (pushes=%d find_calls=%d)\n",
		push_count, find_calls);
	return 0;
}

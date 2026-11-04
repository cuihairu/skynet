/* Unit test for the compat-mingw dlfcn shim (cross-compiled, not run).
 *
 * The shim replaces <dlfcn.h> under MinGW.  POSIX declares dlclose as
 * `int dlclose(void*)`; callers in skynet-src/skynet_module.c and
 * 3rd/lua/loadlib.c use that signature.  This file is compiled with the
 * mingw cross-compiler to prove the shim matches POSIX and that a caller
 * checking `if (dlclose(h) != 0)` compiles.  It is never executed on the
 * host; the Makefile builds it as a compile-only check.
 */
#include <assert.h>
#include <stddef.h>

#include "dlfcn.h"

/* POSIX signature: int dlclose(void *). */
static void
check_signature(void) {
	void *h = NULL;
	int rc = dlclose(h);
	(void)rc;
}

/* A caller that checks the return value must compile. */
static void
caller(void *h) {
	if (dlclose(h) != 0) {
		/* close failed */
	}
}

int
main(void) {
	check_signature();
	caller(NULL);
	return 0;
}

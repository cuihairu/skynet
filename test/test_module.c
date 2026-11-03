#include <assert.h>
#include <stdio.h>

#include "../skynet-src/skynet_module.c"

int
main(void) {
	skynet_module_init("./test/?.so");

	assert(skynet_module_query("module_without_init") == NULL);
	assert(M->count == 0);
	assert(M->m[0].module == NULL);
	assert(M->m[0].name == NULL);

	struct skynet_module *module = skynet_module_query("module_with_init");
	assert(module != NULL);
	assert(module->init != NULL);
	assert(M->count == 1);

	printf("module: all assertions passed\n");
	return 0;
}

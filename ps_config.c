
#include "ps_config.h"

PSConfig ps_config;

int InitConfig(RedisModuleString **argv, int argc) {
    REDISMODULE_NOT_USED(argv);
    REDISMODULE_NOT_USED(argc);

	ps_config.update_threshold = 0.1;

	return REDISMODULE_OK;
}

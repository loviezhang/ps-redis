#ifndef __PS_CONFIG_H__
#define __PS_CONFIG_H__

#include "redismodule.h"

typedef struct {
    float update_threshold;
} PSConfig;

extern PSConfig ps_config;

int InitConfig(RedisModuleString **argv, int argc);

#endif

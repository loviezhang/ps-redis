#ifndef __PUBLISH_H__
#define __PUBLISH_H__

#include "redismodule.h"

int Publish(RedisModuleCtx *ctx, RedisModuleString *name, float weight);

#endif

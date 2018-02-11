#ifndef __PS_TYPE_H__
#define __PS_TYPE_H__

#include "object.h"
#include "redismodule.h"

void *PSTypeRdbLoad(RedisModuleIO *rdb, int encver);
void PSTypeRdbSave(RedisModuleIO *rdb, void *value);
void PSTypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value);
size_t PSTypeMemUsage(const void *value);
void PSTypeFree(void *value);

#endif

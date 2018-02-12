#ifndef __PS_OBJECT_H__
#define __PS_OBJECT_H__

#include "redismodule.h"

typedef struct {
    float old_weight;
    float weight;
} PSObject;

PSObject *CreatePSObject(void);

void UpdatePSObject(RedisModuleCtx *ctx, PSObject *o, float weight, int publish_flag);

void *PSTypeRdbLoad(RedisModuleIO *rdb, int encver);

void PSTypeRdbSave(RedisModuleIO *rdb, void *value);

void PSTypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value);

size_t PSTypeMemUsage(const void *value);

void PSTypeFree(void *value);

#endif

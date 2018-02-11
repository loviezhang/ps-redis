#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "redismodule.h"
#include "ps_object.h"
#include "ps_type.h"

static RedisModuleType *PSType;

int PSPullOne(RedisModuleCtx *ctx, RedisModuleString *key) {
    RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1], REDISMODULE_READ);
    int type = RedisModule_KeyType(key);
	if (type == REDISMODULE_KEYTYPE_EMPTY) {
		return RedisModule_ReplyWithNull();
	}
	if (RedisModule_ModuleTypeGetType(key) != PSType) {
		return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
	}

    struct PSObject *hto = RedisModule_ModuleTypeGetValue(key);
	if (hto == NULL) {
		return RedisModule_ReplyWithNull();
	}

	return RedisModule_ReplyWithDouble(hto->weight);
}

/* PS.PULL key [key ...] */
int PSTypePull_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);

	RedisModule_ReplyWithArray(ctr, argc-1);

	for (int i = 1; i < argc; i++) {
		PSPullOne(ctr, argv[i]);
	}

    return REDISMODULE_OK;
}

int PSPushOne(RedisModuleCtx *ctx, RedisModuleString *key, RedisModuleString *value) {
    RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
			REDISMODULE_READ|REDISMODULE_WRITE);
    int type = RedisModule_KeyType(key);
	if (type == REDISMODULE_KEYTYPE_EMPTY) {
		return RedisModule_ReplyWithNull();
	}
	if (type != REDISMODULE_KEYTYPE_EMPTY &&
			RedisModule_ModuleTypeGetType(key) != PSType) {
		return REDISMODULE_ERR;
	}

    struct HelloTypeObject *hto;
    if (type == REDISMODULE_KEYTYPE_EMPTY) {
        hto = createHelloTypeObject();
    } else {
        hto = RedisModule_ModuleTypeGetValue(key);
    }
	if (hto == NULL) {
		return REDISMODULE_ERR;
	}

	hto->weight = 

	return RedisModule_ReplyWithDouble(hto->weight);
}

/* PS.PUSH key value [key value ...] [AUTO|OFF|FORCE] */
int PSTypePush_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);

	RedisModule_ReplyWithArray(ctr, argc-1);

	for (int i = 1; i < argc; i++) {
		PSPullOne(ctr, argv[i]);
	}

    return REDISMODULE_OK;
}

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
	REDISMODULE_NOT_USED(argv);
	REDISMODULE_NOT_USED(argc);

	if (RedisModule_Init(ctx, "psredis", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
		return REDISMODULE_ERR;
	}

	RedisModuleTypeMethods tm = {
		.version = REDISMODULE_TYPE_METHOD_VERSION,
		.rdb_load = PSTypeRdbLoad,
		.rdb_save = PSTypeRdbSave,
		.aof_rewrite = PSTypeAofRewrite,
		.mem_usage = PSTypeMemUsage,
		.free = PSTypeFree,
	};

	PSType = RedisModule_CreateDataType(ctx, "ps-kbteam", 0, &tm);
	if (PSType == NULL){
		return REDISMODULE_ERR;
	}

	if (RedisModule_CreateCommand(ctx, "ps.push",
				PSTypePush_RedisCommand, "write deny-oom", 1, -1, 2) == REDISMODULE_ERR) {
		return REDISMODULE_ERR;
	}

	if (RedisModule_CreateCommand(ctx, "ps.pull",
				PSTypePull_RedisCommand, "readonly", 1, -1, 1) == REDISMODULE_ERR) {
		return REDISMODULE_ERR;
	}

	return REDISMODULE_OK;
}

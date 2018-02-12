#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include "redismodule.h"
#include "ps_object.h"
#include "constant.h"
#include "ps_config.h"

static RedisModuleType *PSType;

int PSPullOne(RedisModuleCtx *ctx, RedisModuleString *name) {
	//RedisModule_Log(ctx, "debug", "pull %s", RedisModule_StringPtrLen(name, NULL));

    RedisModuleKey *key = RedisModule_OpenKey(ctx, name, REDISMODULE_READ);
    int type = RedisModule_KeyType(key);
	if (type == REDISMODULE_KEYTYPE_EMPTY) {
		return RedisModule_ReplyWithNull(ctx);
	}
	if (RedisModule_ModuleTypeGetType(key) != PSType) {
		return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
	}

    PSObject *hto = RedisModule_ModuleTypeGetValue(key);
	if (hto == NULL) {
		return RedisModule_ReplyWithNull(ctx);
	}

	return RedisModule_ReplyWithDouble(ctx, hto->weight);
}

/* PS.PULL key [key ...] */
int PSTypePull_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);

	if (argc < 2) {
        return RedisModule_ReplyWithError(ctx,
				"ERR wrong number of arguments for PS.PULL");
	}

	RedisModule_ReplyWithArray(ctx, argc-1);

	for (int i = 1; i < argc; i++) {
		PSPullOne(ctx, argv[i]);
	}

    return REDISMODULE_OK;
}

int PSPushOne(RedisModuleCtx *ctx,
		RedisModuleString *name,
		RedisModuleString *value,
		int publish_flag) {
	double weight;
    if ((RedisModule_StringToDouble(value, &weight)
				!= REDISMODULE_OK)) {
        return REDISMODULE_ERR;
    }

    RedisModuleKey *key = RedisModule_OpenKey(ctx, name,
			REDISMODULE_READ|REDISMODULE_WRITE);
    int type = RedisModule_KeyType(key);
	if (type != REDISMODULE_KEYTYPE_EMPTY &&
			RedisModule_ModuleTypeGetType(key) != PSType) {
		return REDISMODULE_ERR;
	}

    PSObject *hto;
    if (type == REDISMODULE_KEYTYPE_EMPTY) {
        hto = CreatePSObject();
        RedisModule_ModuleTypeSetValue(key, PSType, hto);
    } else {
        hto = RedisModule_ModuleTypeGetValue(key);
    }
	if (hto == NULL) {
		return REDISMODULE_ERR;
	}

	UpdatePSObject(ctx, hto, weight, publish_flag);

	return REDISMODULE_OK;
}

int GetPublishFlag(RedisModuleCtx *ctx,
		RedisModuleString **argv, int argc, int *flag) {
	if ((RedisModule_GetContextFlags(ctx) & REDISMODULE_CTX_FLAGS_SLAVE) != 0) {
		*flag = PUBLISH_OFF;
		return REDISMODULE_OK;
	}

	if (argc % 2 == 1) {
		*flag = PUBLISH_AUTO;
		return REDISMODULE_OK;
	}
	const char *param = RedisModule_StringPtrLen(argv[argc-1], NULL);
	if (strncasecmp(param, "force", strlen("force")) == 0) {
		*flag = PUBLISH_FORCE;
		return REDISMODULE_OK;
	} else if (strncasecmp(param, "auto", strlen("auto")) == 0) {
		*flag = PUBLISH_AUTO;
		return REDISMODULE_OK;
	} else if (strncasecmp(param, "off", strlen("off")) == 0) {
		*flag = PUBLISH_OFF;
		return REDISMODULE_OK;
	}

	return REDISMODULE_ERR;
}

/* PS.PUSH key value [key value ...] [AUTO|OFF|FORCE] */
int PSTypePush_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx);

	if (argc < 3) {
        return RedisModule_ReplyWithError(ctx,
				"ERR wrong number of arguments for PS.PUSH");
	}

	int publish_flag;
	if (GetPublishFlag(ctx, argv, argc, &publish_flag) != REDISMODULE_OK) {
		return RedisModule_ReplyWithError(ctx,
				"ERR invalid flag: must be auto/off/force");
	}

	int total = argc % 2 == 1 ? (argc-1)/2 : (argc-2)/2;
	int succ = 0;
	for (int i = 0; i < total; i++) {
		if (PSPushOne(ctx, argv[i*2+1], argv[i*2+2], publish_flag)
				== REDISMODULE_OK) {
			succ++;
		}
	}

	if (succ == total) {
		RedisModule_ReplyWithSimpleString(ctx, "OK");
	} else {
		RedisModule_ReplyWithLongLong(ctx, succ);
	}
	
    RedisModule_ReplicateVerbatim(ctx);
	return REDISMODULE_OK;
}

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
	if (InitConfig(argv, argc) != REDISMODULE_OK) {
		return REDISMODULE_ERR;
	}

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

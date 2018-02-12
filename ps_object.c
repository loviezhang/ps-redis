#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "ps_object.h"
#include "ps_config.h"
#include "constant.h"

PSObject *CreatePSObject(void) {
    PSObject *o;
    o = RedisModule_Alloc(sizeof(*o));
	o->old_weight = 0;
	o->weight = 0;
    return o;
}

void UpdatePSObject(RedisModuleCtx *ctx,
		PSObject *o, float weight, int publish_flag) {
    REDISMODULE_NOT_USED(publish_flag);

	o->weight = weight;

	RedisModule_Log(ctx, "debug", "new %.4f, old %.4f",
			o->weight,
			o->old_weight);
	if (o->weight - o->old_weight > ps_config.update_threshold ||
			o->old_weight - o->weight > ps_config.update_threshold) {
		o->old_weight = o->weight;
	}

	RedisModule_Log(ctx, "debug", "new %.4f, old %.4f",
			o->weight,
			o->old_weight);
}

void *PSTypeRdbLoad(RedisModuleIO *rdb, int encver) {
	if (encver != 0) {
		return NULL;
	}
	float weight = RedisModule_LoadFloat(rdb);
	float old_weight = RedisModule_LoadFloat(rdb);
	PSObject *hto = CreatePSObject();
	hto->weight = weight;
	hto->old_weight = old_weight;

	return hto;
}

void PSTypeRdbSave(RedisModuleIO *rdb, void *value) {
    PSObject *hto = value;
    RedisModule_SaveFloat(rdb, hto->weight);
    RedisModule_SaveFloat(rdb, hto->old_weight);
}

void PSTypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {
	PSObject *hto = value;
	RedisModule_EmitAOF(aof, "PS.PUSH", "sbc", key,
			(char*)&hto->weight,
			sizeof(hto->weight),
			"OFF");
}

size_t PSTypeMemUsage(const void *value) {
    REDISMODULE_NOT_USED(value);
	return sizeof(PSObject);
}

void PSTypeFree(void *value) {
    RedisModule_Free(value);
}

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "ps_type.h"

void *PSTypeRdbLoad(RedisModuleIO *rdb, int encver) {
	if (encver != 0) {
		RedisModule_Log("warning","Can't load data with version %d", encver);
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
    struct PSObject *hto = value;
    RedisModule_SaveFloat(rdb, hto->weight);
    RedisModule_SaveFloat(rdb, hto->old_weight);
}

void PSTypeAofRewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {
	struct PSObject *hto = value;
	/*
	RedisModule_EmitAOF(aof, "PS.PUSH", "sbc", key,
			(char*)&hto->weight,
			sizeof(hto->weight),
			"NOPUBLISH");
	*/
	RedisModule_EmitAOF(aof, "PS.PUSH", "sb", key,
			(char*)&hto->weight,
			sizeof(hto->weight));
}

size_t PSTypeMemUsage(const void *value) {
	return sizeof(PSTypeNode);
}

void PSTypeFree(void *value) {
    RedisModule_Free(value);
}

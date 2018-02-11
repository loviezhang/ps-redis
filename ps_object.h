#ifndef __PS_OBJECT_H__
#define __PS_OBJECT_H__

#include "redismodule.h"

typedef struct {
	float old_weight;
	float weight;
} PSObject;

struct PSObject *CreatePSObject(void);

#endif

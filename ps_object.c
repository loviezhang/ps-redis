#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "ps_object.h"

struct PSObject *CreatePSObject(void) {
    PSObject *o;
    o = RedisModule_Alloc(sizeof(*o));
	o->old_weight = 0;
	o->weight = 0;
    return o;
}

int UpdatePSObject(void

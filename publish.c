#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include "publish.h"

// TODO
int Publish(RedisModuleCtx *ctx,
        RedisModuleString *name, float weight) {
    size_t name_len;
    const char *name_ptr = RedisModule_StringPtrLen(name, &name_len);
    char *msg = RedisModule_Alloc(name_len+1+sizeof(weight));
    memcpy(msg, name_ptr, name_len);
    msg[name_len] = '\n';
    memcpy(msg+name_len+1, &weight, sizeof(weight));

    RedisModuleCallReply *reply;
    reply = RedisModule_Call(ctx, "PUBLISH",
            "cb", "ps:ch_update", msg, name_len+1+sizeof(weight));
    if (reply == NULL) {
        RedisModule_Log(ctx, "error", "publish failed, "
                "command: PUBLISH ps:ch_update %s %.4f, "
                "message: %s",
                name_ptr,
                weight,
                strerror(errno));
        RedisModule_Free(msg);
        return REDISMODULE_ERR;
    }

    RedisModule_Free(msg);
    if (RedisModule_CallReplyType(reply) == REDISMODULE_REPLY_ERROR) {
        RedisModule_Log(ctx, "error", "publish failed, "
                "command: PUBLISH ps:ch_update %s %.4f, "
                "message: %s",
                name_ptr,
                weight,
                RedisModule_CallReplyStringPtr(reply, NULL));
        RedisModule_FreeCallReply(reply);
        return REDISMODULE_ERR;
    }

    RedisModule_Log(ctx, "debug", 
            "command: PUBLISH ps:ch_update %s %.4f",
            name_ptr,
            weight);

    RedisModule_FreeCallReply(reply);
    return REDISMODULE_OK;
}

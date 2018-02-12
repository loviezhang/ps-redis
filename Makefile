
SHOBJ_CFLAGS ?= -W -Wall -g -ggdb -std=c99 -O2
SHOBJ_LDFLAGS ?= -shared

.SUFFIXES: .c .so .xo .o

.c.xo:
	$(CC) -I. $(CFLAGS) $(SHOBJ_CFLAGS) -fPIC -c $< -o $@

ps-redis.so: ps.xo ps_object.xo ps_config.xo
	$(LD) -o $@ $^ $(SHOBJ_LDFLAGS) $(LIBS) -lc

ps.xo: ./redismodule.h

ps_object.xo: ./redismodule.h

ps_config.xo: ./redismodule.h

clean:
	rm -rf *.xo *.so

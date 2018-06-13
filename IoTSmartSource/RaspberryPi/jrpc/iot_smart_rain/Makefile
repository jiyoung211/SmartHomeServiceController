PROJECT_NAME	= main
JSON_LIB	= jsonrpc-c
BT_LIB		= hbe-bt
INCLUDE		= include/
JRPC_SRC_DIR	= lib/jrpc/
JRPC_LIB_NAME	= $(JRPC_SRC_DIR)jsonrpc-c.a
BT_SRC_DIR	= lib/bt/
BT_LIB_NAME	= $(BT_SRC_DIR)hbe-bt.a
COMPILER	= $(CROSS_COMPILE)$(CC)

FLAG=
ifneq ($(CROSS_COMPILE),)
	FLAG	= CROSS_COMPILE=$(CROSS_COMPILE)
endif
#CFLAGS 		= -I$(INCLUDE) -Wall -Werror -O2 -lbluetooth -lpthread
CFLAGS 		= -I$(INCLUDE) -O2 -lbluetooth -lpthread
JRPC_CFLAGS 	= -I$(INCLUDE)
BT_CFLAGS 	= -I$(INCLUDE) -lbluetooth -lpthread
LDFLAGS		= -ldl $(JRPC_LIB_NAME) $(BT_LIB_NAME) -lm -lbluetooth -lpthread
ARCHIVER	= ar rvs

JRPC_SRC_FILES = $(wildcard $(JRPC_SRC_DIR)*.c)
JRPC_OBJS = $(JRPC_SRC_FILES:.c=.o)
BT_SRC_FILES = $(wildcard $(BT_SRC_DIR)*.c)
BT_OBJS = $(BT_SRC_FILES:.c=.o)

all: $(JSON_LIB) $(BT_LIB) $(PROJECT_NAME)

$(PROJECT_NAME): main.o
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(JSON_LIB): $(JRPC_OBJS)
	$(ARCHIVER) $(JRPC_LIB_NAME) $(JRPC_OBJS)

$(BT_LIB): $(BT_OBJS)
	$(ARCHIVER) $(BT_LIB_NAME) $(BT_OBJS)

lib/jrpc/%.o: lib/jrpc/%.c
	$(COMPILER) $(JRPC_CFLAGS) -c -o $@ $<

lib/bt/%.o: lib/bt/%.c
	$(COMPILER) $(BT_CFLAGS) -c -o $@ $<

main.o: main.c
	$(COMPILER) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	-rm $(JRPC_SRC_DIR)*.o $(JRPC_SRC_DIR)$(JSON_LIB).a
	-rm $(BT_SRC_DIR)*.o $(BT_SRC_DIR)$(BT_LIB).a
	-rm $(PROJECT_NAME) *.o

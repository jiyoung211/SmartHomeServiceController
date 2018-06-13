PROJECT_NAME	= main
BT_LIB		= hbe-bt
INCLUDE		= include/
BT_SRC_DIR	= lib/bt/
BT_LIB_NAME	= $(BT_SRC_DIR)hbe-bt.a
COMPILER	= $(CROSS_COMPILE)$(CC)

FLAG=
ifneq ($(CROSS_COMPILE),)
	FLAG	= CROSS_COMPILE=$(CROSS_COMPILE)
endif
CFLAGS 		= -I$(INCLUDE) -O2 -lbluetooth -lpthread
BT_CFLAGS 	= -I$(INCLUDE) -lbluetooth -lpthread
LDFLAGS		= -ldl $(BT_LIB_NAME) -lm -lbluetooth -lpthread
ARCHIVER	= ar rvs

BT_SRC_FILES = $(wildcard $(BT_SRC_DIR)*.c)
BT_OBJS = $(BT_SRC_FILES:.c=.o)

all: $(BT_LIB) $(PROJECT_NAME)

$(PROJECT_NAME): main.o
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(BT_LIB): $(BT_OBJS)
	$(ARCHIVER) $(BT_LIB_NAME) $(BT_OBJS)

lib/bt/%.o: lib/bt/%.c
	$(COMPILER) $(BT_CFLAGS) -c -o $@ $<

main.o: main.c
	$(COMPILER) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	-rm $(BT_SRC_DIR)*.o $(BT_SRC_DIR)$(BT_LIB).a
	-rm $(PROJECT_NAME) *.o

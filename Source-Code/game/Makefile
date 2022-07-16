# paths
LIB = ../lib
INCLUDE = ../include
MODULES = ../modules

# compiler
CC = gcc

# Compile options.
CFLAGS = -Wall -Werror -g -I$(INCLUDE)
LDFLAGS = -lm

OBJS = kiki.o kiki_interface.o kikistate.o $(MODULES)/UsingADTList/set_utils.o $(LIB)/k08.a $(LIB)/libraylib.a

EXEC = kiki

include $(LIB)/libraylib.mk

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)
	@if [ -f $(EXEC).exe ]; then ln -sf $(EXEC).exe $(EXEC); fi

$(LIB)/%.a:
	$(MAKE) -C $(LIB) $*.a

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)

valgrind: $(EXEC)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)
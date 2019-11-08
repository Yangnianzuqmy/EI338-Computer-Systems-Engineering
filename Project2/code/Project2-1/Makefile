CC=gcc
SRC_FILE=$(wildcard *.c)
OBJ_FILE=$(patsubst %.c, %.o ,$(SRC_FILE))
target=Unix_shell
all : $(target)
$(target) : $(OBJ_FILE)
	$(CC) -o $@ $^
$(OBJ_FILE) : $(SRC_FILE)
	$(CC) -o $@ -c $^
clean:
	$(RM) $(target) $(OBJ_FILE)
	

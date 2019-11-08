CC=gcc
LDFLAGS += -lpthread
SRC=sort.c
OBJ=sort.o
TARGET=sort

all : $(TARGET)

$(TARGET) : $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ) : $(SRC)
	$(CC) -o $@ -c $^

clean:
	$(RM) $(TARGET) $(OBJ)

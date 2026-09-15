TAR = chenwentao
OBJ = daemon.o error.o


$(TAR) : $(OBJ)
	gcc -o $@ $^

CFLAGS += -I..

%.o: %.c
	gcc $(CFLAGS) -c $< -std=c99


clean:
	rm $(TAR) $(OBJ)

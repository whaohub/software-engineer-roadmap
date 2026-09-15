TAR = write
OBJ = write.o error.o


$(TAR) : $(OBJ)
	gcc -o $@ $^


%.o: %.c
	gcc $(CFLAGS) -c $< -std=c99


clean:
	rm $(TAR) $(OBJ)

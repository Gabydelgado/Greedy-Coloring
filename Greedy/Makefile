FLAGS = -Wall -Werror -Wextra -DNDEBUG -O3 -g -std=c99 #-fPIC
OBJECTS = main.o JonSnow.o tuple.o shell.o


all:
	gcc $(FLAGS) -c *.c
	gcc $(FLAGS) -o test *.o
clean:
	@rm -f test *.o *~
indent:
	@find *.c *.h -exec indent -kr -brs -brf --no-tabs {} + 
	#estilo K&R, brace right struct, brace right function, spaces instead of tabs 
headers:
	@find *.h -exec gedit {} +
sources:
	@find *.c -exec gedit {} +

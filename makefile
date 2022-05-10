CC=g++ -static
CFLAGS=-c
OFLAGS=-o
LDFLAGS=-Llib
INCLUDE=-Iinclude
# it's needed to be fixed
LIBFLAGES=-lglfw3 -lGL -lfreetype -lX11
RM=rm

# if OS = Windows
ifdef OS
	RM=del
	LIBFLAGES=-lopengl32 -lwinmm -lgdi32 -mwindows
	LDFLAGS=-Llib
	INCLUDE=-Iinclude
endif

all: main

main: main.o
	$(CC) $(OFLAGS) main main.o $(INCLUDE) $(LDFLAGS) $(LIBFLAGES)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp $(INCLUDE) $(LIBFLAGES)

clean:
	$(RM) -f *.o main main.exe
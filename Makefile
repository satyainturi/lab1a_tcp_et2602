 # Makefile

CPP      = g++
CC       = gcc

LIBS     = 
INCS     = 
BINSER	 = server
BINCLI	 = client
CFLAGS   = $(INCS) 
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: clean all-before $(BINSER) $(BINCLI) all-after

clean: clean-custom
	${RM} server.o $(BINSER) client.o $(BINCLI) server.exe client.exe

$(BINSER): proc.o server.o
	$(CC) proc.o server.o -o $(BINSER) $(LIBS)

server.o: server.c
	$(CC) -c server.c -o server.o $(CFLAGS)

proc.o: proc.c
	$(CC) -c proc.c -o proc.o $(CFLAGS)

$(BINCLI): client.o
	$(CC) client.o -o $(BINCLI) $(LIBS)

client.o: client.c
	$(CC) -c client.c -o client.o $(CFLAGS)

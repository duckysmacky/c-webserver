libs = -lwsock32
args = -Wall

all: server

server:
	gcc src/server/server.c -o bin/server $(libs) $(args)
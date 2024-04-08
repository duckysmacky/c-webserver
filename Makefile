libs = -lwsock32
args = -Wall

all: clean server

server:
	gcc src/server/server.c -o bin/server $(libs) $(args)

clean:
	rm dir/*
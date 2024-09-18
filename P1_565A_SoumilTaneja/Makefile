# Targets to build both client and server
all: p1client p1server

# Compile the client program
p1client: p1client.c
	gcc -o p1client p1client.c

# Compile the server program
p1server: p1server.c
	gcc -o p1server p1server.c -pthread

# Clean up the compiled binaries
clean:
	rm -f p1client p1server

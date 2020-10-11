CC = gcc
CFLAGS = -I -Wall
RM = /bin/rm

sinfo_client: sinfo_client.c
	$(CC) $(CFLAGS) sinfo_client.c -o sinfo_client

sinfo_server: sinfo_server.c
	$(CC) $(CFLAGS) sinfo_server.c -o sinfo_server

all: sinfo_client.c sinfo_server.c
	$(CC) $(CFLAGS) sinfo_client.c -o sinfo_client
	$(CC) $(CFLAGS) sinfo_server.c -o sinfo_server

clean: sinfo_client sinfo_server
	$(RM) sinfo_client sinfo_server

install: sinfo_client sinfo_server
	install -m 755 sinfo_client /usr/local/bin
	install -m 755 sinfo_server /usr/local/bin
	install -m 644 stats.txt /usr/local/bin

uninstall:
	$(RM) /usr/local/bin/sinfo_client
	$(RM) /usr/local/bin/sinfo_server
	$(RM) /usr/local/bin/stats.txt

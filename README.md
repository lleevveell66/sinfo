# sinfo - simple socket stream info sharing

**Current Version:** 1.0<br>
**Last Update:** 10/11/20<br>
**Author:** level6<br>
**URL:** https://github.com/lleevveell66/sinfo<br>
This is sinfo, which is just the most basic example of a TCP server that will share the contents of a local file 
with the client which connects, written in C.<br>

## Description:

This was built mostly to demonstrate to new C socket programmers how easily a file can be shared over a simple
TCP socketed connection.  It consists of three parts:

- sinfo_server : This program will listen on port 1411 for connections, reading and sending the contents of stats.txt when each is made.
- sinfo_client : This is the client which will connect to port 1411 and display everything that it receives.
- stats.txt : This is the local file whose contents are transferred.  Feel free to cron something up to overwrite this file to turn this into something fairly usefull for monitoring/reporting/alerting.

## Installation:

```
cd /usr/local/src
git clone https://github.com/lleevveell66/sinfo
cd sinfo
make all
make install
```

## Usage:

```
vi stats.txt              # add whatever you want to this file
./sinfo_server            # in one shell (or, it can be backgrounded
./sinfo_client 127.0.0.1  # in another shell
```

Of course, change 127.0.0.1 to whatever IP address you're running sinfo_server on.  This example assumes they are both on the same machine.

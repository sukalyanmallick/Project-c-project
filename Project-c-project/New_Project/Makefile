# Makefile — Root build orchestrator
#
# Delegates to the server and client sub-makefiles.
#
# Usage:
#   make            — build both server and client
#   make server     — build only the server
#   make client     — build only the client
#   make clean      — clean both

.PHONY: all server client clean

all: server client

server:
	$(MAKE) -C server

client:
	$(MAKE) -C client

clean:
	$(MAKE) -C server clean
	$(MAKE) -C client clean

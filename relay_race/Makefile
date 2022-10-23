APPNAME=$(shell basename $(shell pwd))

CC=cc
CXX=g++
FLAGS=-g -Wall -Wextra
CFLAGS=$(FLAGS)
CXXFLAGS=$(FLAGS)
LIBS=-pthread
CLIBS=$(LIBS)
CXXLIBS=$(LIBS)

all: $(APPNAME)

%: %.c
	$(CC) $(CFLAGS) $^ -o $@ $(CLIBS)

%: %.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(CXXLIBS)

.PHONY: gitignore
gitignore:
	echo $(APPNAME) > .gitignore

.PHONY: clean
clean:
	rm -f $(APPNAME)
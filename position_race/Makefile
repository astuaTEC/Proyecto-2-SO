APPNAME=$(shell basename $(shell pwd))

$(APPNAME): $(APPNAME).c
	cc -g -Wall -Wextra $(APPNAME).c -o $(APPNAME) -pthread

.PHONY: gitignore
gitignore:
	echo $(APPNAME) > .gitignore

.PHONY: clean
clean:
	rm -f $(APPNAME)
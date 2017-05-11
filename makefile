all:
	$(MAKE) -C ./cp-command/
	$(MAKE) -C ./cat-command/
	$(MAKE) -C ./groups-command/
	$(MAKE) -C ./ls-command/
	export BIN_PATH="${PWD}/bin:${PATH}"; $(MAKE) -e -C ./shell-command/
	mkdir -p bin
	cp ./cp-command/cp ./bin
	cp ./cat-command/cat ./bin
	cp ./groups-command/groups ./bin
	cp ./ls-command/ls ./bin



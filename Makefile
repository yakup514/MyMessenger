all: install_server install_client
	

install_server:
	mkdir -p build && mkdir -p app_serv \
	&& cd build && cmake -DEXECUTABLE_OUTPUT_PATH="../app_serv" ../Server && cmake --build .
	rm -rf build
install_client:
	mkdir -p build && mkdir -p app_cli \
	&& cd build && cmake -DEXECUTABLE_OUTPUT_PATH="../app_cli" ../Client && cmake --build .
	rm -rf build

dvi:
	doxygen Doxyfile
	open doc/html/index.html
git:
	git add client/*.cpp client/*.h client/*.txt
	git add server/*.cpp server/*.h server/*.txt
	git add Makefile

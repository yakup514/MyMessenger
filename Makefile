all: install_server install_client
	

install_server:
	mkdir server_app && cd server_app && cmake ../server .. && cmake . && sudo make install
install_client:
	mkdir client_app && cd client_app && cmake ../client .. && cmake . && sudo make install

dvi:
	doxygen Doxyfile
	open doc/html/index.html
git:
	git add client/*.cpp client/*.h client/*.txt
	git add server/*.cpp server/*.h server/*.txt
	git add Makefile

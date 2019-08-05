cloudBkp_server:cloudBkp_server.cc cloudBackupSer.hpp compressTool.hpp view.hpp
	g++ cloudBkp_server.cc -o cloudBkp_server -std=c++11 -lpthread -lboost_filesystem -lboost_system -lctemplate -I ~/third_part/include -L ~/third_part/lib -lz -lssl -lcrypto

.PHONY:clean
clean:
	rm cloudBkp_server

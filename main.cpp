#include <iostream>
#include <y-pcap-service.h>

int main(int argc, char *argv[]) {

	std::string *dev = NULL;
	
	if (argc > 1) {
		dev = new std::string(argv[1]);
	}

	YPcap0Service service;
	int result = service.Run(dev);
	return result;
}


#ifndef H_Y_PCAP_0_SERVICE
#define H_Y_PCAP_0_SERVICE

#include <iostream>
#include "pcap.h"

class YPcap0Service {
	public:
		YPcap0Service();
		~YPcap0Service();
		int Run(std::string *dev) const;
	private:
		int GetDevice(std::string **dev, bpf_u_int32 *net, bpf_u_int32 *mask) const;
		int PrepareSniffer(pcap_t **handle, std::string *filter_exp, std::string *dev, bpf_u_int32 *net, bpf_u_int32 *mask) const;
		int ProcessNext(pcap_t *handle) const;
		int Process(pcap_t *handle, int cnt) const;
		static void HandlePacket(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes);
};

#endif


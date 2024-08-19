#ifndef H_Y_PCAP_0_SERVICE
#define H_Y_PCAP_0_SERVICE

#include <iostream>
#include "pcap.h"

class Sniffer {
    public:
        Sniffer(const bool isVerbose);
        ~Sniffer();
        int Run(std::string *dev) const;
        void HandlePacket(const struct pcap_pkthdr *h, const u_char *bytes);

    private:
        bool _isVerbose;
        int GetDevice(std::string **dev, bpf_u_int32 *net, bpf_u_int32 *mask) const;
        int Prepare(pcap_t **handle, std::string *filter_exp, std::string *dev, bpf_u_int32 *net, bpf_u_int32 *mask) const;
        int ProcessNext(pcap_t *handle) const;
        int Process(pcap_t *handle, int cnt) const;
        // wrap HandlerPacket method to call it from pcap_loop that can only call back a static method
        static void HandlePacketWrapper(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes);
};

#endif


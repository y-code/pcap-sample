#include <boost/beast/core/detail/base64.hpp>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include "packet.pb.h"
#include "y-pcap-service.h"

YPcap0Service::YPcap0Service() {
    std::cout << "Constructing " << typeid(this).name() << "..." << std::endl;
}

YPcap0Service::~YPcap0Service() {
    std::cout << "Destructing " << typeid(this).name() << "..." << std::endl;
}

int YPcap0Service::Run(std::string *dev) const {
    pcap_t *handle = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];
    int result;
    bpf_u_int32 mask;                   /* The netmask of our sniffing device */
    bpf_u_int32 net;                    /* The IP of the sniffing device */
    //std::string filter_exp = "dst port 443"; /* The filter expression */
    std::string filter_exp = ""; /* The filter expression */

    result = this->GetDevice(&dev, &net, &mask);
    if (result) return result;
    std::cout << "Device: " << *dev << std::endl;
    std::cout << "net: " << net << ", mask: " << mask << std::endl;

    result = this->PrepareSniffer(&handle, &filter_exp, dev, &net, &mask);
    if (result) return result;

    // Grab packages
    //result = this->ProcessNext(handle);
    //result = this->Process(handle, 10);
    result = this->Process(handle, 0);
    
    pcap_close(handle);

    return result;
}

int YPcap0Service::GetDevice(std::string **dev, bpf_u_int32 *net, bpf_u_int32 *mask) const {
    char *result, errbuf[PCAP_ERRBUF_SIZE];

//    if (dev == NULL) {
    if (!*dev) {
        result = pcap_lookupdev(errbuf);
        if (result == NULL) {
            std::cerr << " Couldn't find default device: " << errbuf << std::endl;
            return 2;
        }
        *dev = new std::string(result);
    }

    if (pcap_lookupnet((*dev)->c_str(), net , mask, errbuf) == -1) {
        std::cout << "Cannot get netmask for dvice " << **dev << ": " << errbuf << std::endl;
        *net = 0;
        *mask = 0;
    }

    return(0);
}

int YPcap0Service::PrepareSniffer(pcap_t **handle, std::string *filter_exp, std::string *dev, bpf_u_int32 *net, bpf_u_int32 *mask) const {
    char errbuf[PCAP_ERRBUF_SIZE];
    int dlt;
    struct bpf_program fp;              /* The compiled filter expression */

    *handle = pcap_open_live(dev->c_str(), BUFSIZ, 1, 1000, errbuf);
    if (!*handle) {
        std::cout << "Couldn't open device " << *dev << ": " << errbuf;
        return 2;
    }
    std::cout << "Opened device " << *dev << std::endl;

    dlt = pcap_datalink(*handle);
    if (dlt != DLT_EN10MB) {
        std::cout << "Device " << *dev << " doesn't provide Ethernet headers - not supported" << std::endl;
        return 2;
    }
    std::cout << "Link-layer header type: " << dlt << std::endl;

    if (pcap_compile(*handle, &fp, (*filter_exp).c_str(), 0, *net) == -1) {
        std::cerr << "Couldn't parse filter " << *filter_exp << ": " << pcap_geterr(*handle) << std::endl;
        return 2;
    }
    std::cout << "Compiled filter " << *filter_exp << std::endl;

    if (pcap_setfilter(*handle, &fp) == -1) {
        std::cerr << "Couldn't install filter " << *filter_exp << ": " << pcap_geterr(*handle) << std::endl;
        return 2;
    }
    std::cout << "Installed filter " << *filter_exp << std::endl;

    return 0;
}

int YPcap0Service::ProcessNext(pcap_t *handle) const {
    struct pcap_pkthdr header;
    const u_char *packet;

    pcap_set_buffer_size(handle, 256 * 1024);
    pcap_set_snaplen(handle, 256);

    packet = pcap_next(handle, &header);
    std::cout << "Jacked a packet with length of " << header.len << std::endl;

    return 0;
}

int YPcap0Service::Process(pcap_t *handle, int cnt) const {
    u_char *user = NULL;

    std::cout << "Loop Count: " << cnt << std::endl;

    int result = pcap_loop(handle, cnt, &YPcap0Service::HandlePacket, user);
    //int result = pcap_dispatch(handle, cnt, &YPcap0Service::HandlePacket, user);

    switch (result) {
	    case 0:
            std::cout << "Completed." << std::endl;
            break;
        case PCAP_ERROR_BREAK:
            std::cerr << "Although no packets have been processed, terminating the sniffer due to a request." << std::endl;
            break;
        case PCAP_ERROR_NOT_ACTIVATED:
            std::cerr << "The capture handle has not been activated." << std::endl;
            break;
        case PCAP_ERROR:
            std::cerr << "Something wrong..." << std::endl;
            break;
        default:
            if (result)
                std::cout << "Processed " << result << " packets." << std::endl;
            else
                std::cerr << "Encountered an unexpected result. (" << result << ")" << std::endl;
            break;
    }
    return 0;
}

void YPcap0Service::HandlePacket(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes) {
    std::cout << "=====================================" << std::endl;
    std::cout << "Packet Length: " << h->len << std::endl;

    struct ether_header *eth_hdr = (struct ether_header*)bytes;
    u_short ether_type = ntohs(eth_hdr->ether_type);

    std::cout << "Ether Type: " << ether_type << std::endl;

    if (ether_type != ETHERTYPE_IP)
        return;

    ypcap0::IpPacket pkt;
    struct ip *ip_header = (struct ip*)(bytes + sizeof(struct ether_header));
    char* base64 = new char[256];

    std::cout << "IP Version: " << ip_header->ip_v << std::endl;

    char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);

    std::cout << "From " << src_ip << " to " << dst_ip << std::endl;

    pkt.set_srcaddress(src_ip);
    pkt.set_dstaddress(dst_ip);
    pkt.set_size(h->len);
    std::string serializedData;
    pkt.SerializeToString(&serializedData);

    //boost::beast::detail::base64::encode(base64, serializedData.c_str(), serializedData.length());
    boost::beast::detail::base64::encode(base64, &serializedData[0], serializedData.length());
    std::string strBase64(base64);

    delete[] base64;

    std::cout << "Serialized: " << strBase64 << std::endl;
    std::cout << "=====================================" << std::endl;
}


#ifndef TINS_CONFIG_H
#define TINS_CONFIG_H

/* modified from config.h.in

/* Define if the compiler supports basic C++11 syntax */
#define TINS_HAVE_CXX11 1

/* Have IEEE 802.11 support */
#define TINS_HAVE_DOT11 0

/* Have WPA2 decryption library */
#define TINS_HAVE_WPA2_DECRYPTION 0

/* Use pcap_sendpacket to send l2 packets */
#define TINS_HAVE_PACKET_SENDER_PCAP_SENDPACKET 0

/* Have TCPIP classes */
#define TINS_HAVE_TCPIP 1

/* Have TCP ACK tracking */
#define TINS_HAVE_ACK_TRACKER 0

/* Have TCP stream custom data */
#define TINS_HAVE_TCP_STREAM_CUSTOM_DATA 0

/* Have GCC builtin swap */
#define TINS_HAVE_GCC_BUILTIN_SWAP 1

/* Have WPA2Decrypter callbacks */
#define TINS_HAVE_WPA2_CALLBACKS 0

/* Have libpcap */
#define TINS_HAVE_PCAP 0

#endif // TINS_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void print_family(struct addrinfo *aip)
{
	printf(" family: ");
	switch (aip->ai_family) {
	case AF_INET:
		printf("inet");
		break;
	case AF_INET6:
		printf("inet6");
		break;
	case AF_UNIX:
		printf("unix");
		break;
	case AF_UNSPEC:
		printf("unspecified");
		break;
	default:
		printf("unknown");
	}
}

void print_type(struct addrinfo *aip)
{
	printf(" type: ");
	switch (aip->ai_socktype) {
	case SOCK_STREAM:
		printf("stream");
		break;
	case SOCK_DGRAM:
		printf("datagram");
		break;
	case SOCK_SEQPACKET:
		printf("seqpacket");
		break;
	case SOCK_RAW:
		printf("raw");
		break;
	default:
		printf("unknown (%d)", aip->ai_socktype);
	}
}

void print_protocol(struct addrinfo *aip)
{
	printf(" protocol: ");
	switch (aip->ai_protocol) {
	case 0:
		printf("default");
		break;
	case IPPROTO_TCP:
		printf("TCP");
		break;
	case IPPROTO_UDP:
		printf("UDP");
		break;
	case IPPROTO_SCTP:
		printf("SCTP");
		break;
	case IPPROTO_RAW:
		printf("raw");
		break;
	default:
		printf("unkonwn (%d)", aip->ai_protocol);
	}
}

void print_flags(struct addrinfo *aip)
{
	printf("flags:");
	if (aip->ai_flags == 0) {
		printf(" 0");
	} else {
		if (aip->ai_flags & AI_PASSIVE)
			printf(" passive");
		if (aip->ai_flags & AI_CANONNAME)
			printf(" canon");
		if (aip->ai_flags & AI_NUMERICHOST)
			printf(" numhost");
		if (aip->ai_flags & AI_NUMERICSERV)
			printf(" numserv");
		if (aip->ai_flags & AI_V4MAPPED)
			printf(" v4mapped");
		if (aip->ai_flags & AI_ALL)
			printf(" all");
	}
}

int main(int argc, char *argv[])
{
	int					err;
	struct addrinfo		hint;
	struct addrinfo		*aip, *ailist;
	struct sockaddr_in	*sinp;
	struct sockaddr_in6	*sin6p;
	char				inet_addr[INET_ADDRSTRLEN];
	char				inet6_addr[INET6_ADDRSTRLEN];
	const char			*addr;

	if (argc != 2 && argc != 3)
		errx(1, "Usage: %s (<address> | <hostname>) [service]\n", argv[0]);

	memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_CANONNAME;
	hint.ai_addr = NULL;
	hint.ai_canonname = NULL;
	hint.ai_next = NULL;
	if ((err = getaddrinfo(argv[1], argv[2], &hint, &ailist)) != 0)
		errx(1, "getaddrinfo error: %s\n", gai_strerror(err));

	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		print_flags(aip);
		print_family(aip);
		print_type(aip);
		print_protocol(aip);
		printf("\n\thost: %s", aip->ai_canonname ? aip->ai_canonname : "-");

		if (aip->ai_family == AF_INET) {
			sinp = (struct sockaddr_in *)aip->ai_addr;
			addr = inet_ntop(AF_INET, &sinp->sin_addr, inet_addr, INET_ADDRSTRLEN);
			printf(" address: %s", addr ? addr : "unknown");
			printf(" port: %d", ntohs(sinp->sin_port));
		} else {
			sin6p = (struct sockaddr_in6 *)aip->ai_addr;
			addr = inet_ntop(AF_INET6, &sin6p->sin6_addr, inet6_addr, INET6_ADDRSTRLEN);
			printf(" address: %s", addr ? addr : "unknown");
			printf(" port: %d", sin6p->sin6_port);
		}
		printf("\n");
	}
	
	return 0;
}

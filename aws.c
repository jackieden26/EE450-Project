#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define SAP 21700  // Server A port number.
#define SBP 22700  // Server B port number.
#define WUP 23700  // AWS UDP port number.
#define WTP 24700  // AWS TCP port number.
#define LIP "127.0.0.1"  // Local host IP address.



int main() {
	printf("The AWS is up and running.\n");
	char *msga = "hello I am aws sending to A";
	// Create UDP socket.
	int udpFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udpFd < 0) {
		perror("udpfd fail");
		exit(1);
	}

	struct sockaddr_in udpSock;
	memset(&udpSock, 0, sizeof(udpSock));

	udpSock.sin_family = AF_INET;
	udpSock.sin_port = htons(WUP);
	udpSock.sin_addr.s_addr = inet_addr(LIP);

	// Bind UDP socket.
	int retval = bind(udpFd, (struct sockaddr *)&udpSock, sizeof(udpSock));
	if (retval < 0) {
		perror("aws bind fail");
		exit(1);
	}

	// Send data to A.
	struct sockaddr_in sockA;
	memset(&sockA, 0, sizeof(sockA));
	sockA.sin_family = AF_INET;
	sockA.sin_port = htons(SAP);
	sockA.sin_addr.s_addr = inet_addr(LIP);

	sendto(udpFd, msga, strlen(msga), 0, (struct sockaddr *)&sockA, sizeof(sockA));

	printf("%d\n", SAP);
}

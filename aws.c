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
#define AWSUPORT 23700  // AWS UDP port number.
#define AWSTPORT 24700  // AWS TCP port number.
#define LIP "127.0.0.1"  // Local host IP address.

struct clientQuery {
  char mapid;
  int srcVertex;
  long int fileSize;
};

int main() {
	int retval;
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
	udpSock.sin_port = htons(AWSUPORT);
	udpSock.sin_addr.s_addr = inet_addr(LIP);

	// Bind UDP socket.
	retval = bind(udpFd, (struct sockaddr *)&udpSock, sizeof(udpSock));
	if (retval < 0) {
		perror("aws udp bind fail");
		exit(1);
	}

	// Send data to A.
	struct sockaddr_in sockA;
	memset(&sockA, 0, sizeof(sockA));
	sockA.sin_family = AF_INET;
	sockA.sin_port = htons(SAP);
	sockA.sin_addr.s_addr = inet_addr(LIP);

	sendto(udpFd, msga, strlen(msga), 0, (struct sockaddr *)&sockA, sizeof(sockA));

	// Create TCP socket.
	int tcpFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tcpFd < 0) {
		perror("tcpFd failed");
		exit(1);
	}

	struct sockaddr_in tcpSock;
	memset(&tcpSock, 0, sizeof(tcpSock));

	tcpSock.sin_family = AF_INET;
	tcpSock.sin_port = htons(AWSTPORT);
	tcpSock.sin_addr.s_addr = inet_addr(LIP);

	retval = bind(tcpFd, (struct sockaddr *)&tcpSock, sizeof(tcpSock));
	if (retval < 0) {
		perror("aws tcp bind fail");
		exit(1);
	}

	retval = listen(tcpFd, 5);
	if (retval < 0) {
		perror("listen fail");
	}

	// Prepare to accept cleint's communication.
	struct sockaddr_in clientSock;
	socklen_t clientSockLen;
	clientSockLen = sizeof(clientSock);
	memset(&clientSock, 0, sizeof(clientSock));
	int childSockFd = accept(tcpFd, (struct sockaddr *)&clientSock, &clientSockLen);
	if (childSockFd <= 0) {
		perror("accept failed");
		exit(1);
	}

	printf("client ip is: %s\n", inet_ntoa(clientSock.sin_addr));
	printf("client port is: %d\n", ntohs(clientSock.sin_port));

	struct clientQuery clientBuffer;
	int clientBufferLen = 100;

	retval = recv(childSockFd, &clientBuffer, sizeof(clientBuffer), 0);
	if (retval < 0) {
		perror("recv fail");
		exit(1);
	}
	printf("bumber of bytes i received is: %d\n", retval);
	printf("query size is: %d\n", sizeof(clientBuffer));
	printf("mapid is: %c\n", clientBuffer.mapid);
	printf("srcVertex is: %d\n", clientBuffer.srcVertex);
	printf("fileSize is: %ld\n", clientBuffer.fileSize);

	// printf("mapid is: %s\n", clientBuffer[1]);
	// printf("srcVertex is: %d\n", atoi(clientBuffer[2]));
	// printf("fileSize is: %ld\n", atol(clientBuffer[3]));


	close(udpFd);
	close(tcpFd);
	printf("end of aws.c main.\n");
}

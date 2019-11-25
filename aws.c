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

// Query structure received from client.
struct clientQuery {
  char mapid;
  int srcVertex;
  long int fileSize;
};

// Query structure sent to server A.
struct AQuery {
  char mapid;
  int srcVertex;
};

int main() {
	int retval;
	printf("The AWS is up and running.\n");

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

	// Prepare to accept cleint's TCP communication.
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

	struct clientQuery clientToAWSMsg;
	int clientToAWSMsgLen = 100;

	retval = recv(childSockFd, &clientToAWSMsg, sizeof(clientToAWSMsg), 0);
	if (retval < 0) {
		perror("recv fail");
		exit(1);
	}

  printf("The AWS has received map ID %c, start vertex %d and file size %ld"
      "from the client using TCP over port %d\n",\
      clientToAWSMsg.mapid, clientToAWSMsg.srcVertex, \
      clientToAWSMsg.fileSize, AWSTPORT);

	// printf("mapid is: %s\n", clientToAWSMsg[1]);
	// printf("srcVertex is: %d\n", atoi(clientToAWSMsg[2]));
	// printf("fileSize is: %ld\n", atol(clientToAWSMsg[3]));


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

  // Send query to A.
  struct sockaddr_in sockA;
  memset(&sockA, 0, sizeof(sockA));
  sockA.sin_family = AF_INET;
  sockA.sin_port = htons(SAP);
  sockA.sin_addr.s_addr = inet_addr(LIP);

  struct AQuery awsToAMsg;
  awsToAMsg.mapid = clientToAWSMsg.mapid;
  awsToAMsg.srcVertex = clientToAWSMsg.srcVertex;

  sendto(udpFd, &awsToAMsg, sizeof(awsToAMsg), 0,
    (struct sockaddr *)&sockA, sizeof(sockA));

  printf("The AWS has sent map ID and starting vertex to server A using "
        "UDP over port %d\n", AWSUPORT);



	close(udpFd);
	close(tcpFd);
	printf("end of aws.c main.\n");
}

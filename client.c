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

#define AWSTPORT 24700  // AWS TCP port number.
#define LIP "127.0.0.1"  // Local host IP address.

struct clientQuery {
  char mapid;
  int srcVertex;
  long int fileSize;
};

int main(int argc, char const *argv[]) {
  int retval;
  if (argc != 4) {
    printf("wrong number of arguments are typed\n");
    exit(1);
  }

  printf("The client is up and running.\n");

  if (strlen(argv[1]) != 1) {
    printf("map id is more than one letter.\n");
  }

  // Extract input arguments.
  struct clientQuery query;
  query.mapid = argv[1][0];
  query.srcVertex = atoi(argv[2]);
  query.fileSize = atol(argv[3]);

  // Establish TCP connection with AWS.
  // First to create client's own socket.
  int mySockFd;
  mySockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (mySockFd < 0) {
    perror("socket creation failed");
    exit(1);
  }

  struct sockaddr_in mySock;
  memset(&mySock, 0, sizeof(mySock));
  // mySock.sin_family = AF_INET;
  // mySock.sin_port = 0;
  // mySock.sin_addr.s_addr = inet_addr(LIP);

  // Then connect it to AWS server.
  struct sockaddr_in AWSock;
  memset(&AWSock, 0, sizeof(AWSock));
  AWSock.sin_family = AF_INET;
  AWSock.sin_port = htons(AWSTPORT);
  AWSock.sin_addr.s_addr = inet_addr(LIP);

  retval = connect(mySockFd, (struct sockaddr *)&AWSock, sizeof(AWSock));
  if (retval < 0) {
    perror("connection failed");
    exit(1);
  }

  struct sockaddr_in mySock2;
  memset(&mySock2, 0, sizeof(mySock2));
  socklen_t mySock2Len = sizeof(mySock2);
  retval = getsockname(mySockFd, (struct sockaddr *)&mySock2, &mySock2Len);
  if (retval < 0) {
    perror("getsockname failed");
    exit(1);
  }

  retval = send(mySockFd, &query, sizeof(query), 0);
  if (retval < 0) {
    perror("send fail");
    exit(1);
  }

  printf("The client has sent query to AWS using TCP: start vertex "
    "%d; map %c; file size %ld.\n", query.srcVertex, query.mapid, query.fileSize);

  // printf("my ip is: %s\n", inet_ntoa(mySock2.sin_addr));
  // printf("my port is: %d\n", ntohs(mySock2.sin_port));

  close(mySockFd);
  return 0;
}

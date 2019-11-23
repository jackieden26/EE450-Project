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


#define SAP 21700
#define SBP 22700
#define AUP 23700
#define ATP 24700
#define LIP "127.0.0.1"

#define MAXLINE 1024

int main() {
  int sockfd;
  char buffer[MAXLINE];
  char *hello = "Hello from server";
  struct sockaddr_in mySock;

  // Creating socket file descriptor
  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sockfd < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&mySock, 0, sizeof(mySock));

  mySock.sin_family = AF_INET;
  mySock.sin_addr.s_addr = inet_addr(LIP);
  mySock.sin_port = htons(SAP);

  // Bind the socket with the server address
  int retval = bind(sockfd, (const struct sockaddr *)&mySock, sizeof(mySock));
  if (retval < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in WSock;
  memset(&WSock, 0, sizeof(WSock));

  printf("The server A is up and running using UDP on port %d\n", SAP);

  int WSockLen, n;
  n = recvfrom(sockfd, (char *)buffer, MAXLINE,
              MSG_WAITALL, ( struct sockaddr *) &WSock,
              &WSockLen);
  buffer[n] = '\0';
  printf("Client : %s\n", buffer);
  sendto(sockfd, (const char *)hello, strlen(hello),
      MSG_CONFIRM, (const struct sockaddr *) &WSock,
          WSockLen);
  printf("Hello message sent.\n");

  return 0;
}

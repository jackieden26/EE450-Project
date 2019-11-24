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

#define WTP 24700  // AWS TCP port number.
#define LIP "127.0.0.1"  // Local host IP address.

int main(int argc, char const *argv[]) {
  if (argc != 4) {
    printf("wrong number of arguments are typed\n");
    exit(1);
  }

  printf("The client is up and running.\n");

  if (strlen(argv[1]) != 1) {
    printf("map id is more than one letter.\n");
  }

  // Extract input arguments.
  char mapid = argv[1][0];
  int srcVertex = atoi(argv[2]);
  long int fileSize = atoi(argv[3]);



  return 0;
}

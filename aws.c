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

int main() {
	printf("%d\n", SAP);
}

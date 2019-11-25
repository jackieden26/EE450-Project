#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define SAP 21700
#define SBP 22700
#define AWSUPORT 23700
#define AWSTPORT 24700
#define LIP "127.0.0.1"

#define MAXLINE 1024

struct cityMap {
  char mapid;
  double propSpeed;
  double tranSpeed;
  int labelArray[10];  // Stores the map from index 0-9 to label index.
  int vertexNum;  // Number of valid labels in labelArray.
  int edgeNum;  // Number of edges.
  int adjMatrix[10][10]; // Graph's adjacency matrix.
};

struct AQuery {
  char mapid;
  int srcVertex;
};

bool ifVertexInLabelArray(int labelArray[], int label, int vertexNum) {
  for (size_t i = 0; i < vertexNum; i++) {
    if (labelArray[i] == label) {
      return true;
    }
  }
  return false;
}

int smaller(int a, int b) {
  if (a < b) {
    return a;
  }
  return b;
}

/*
 * Reference: https://stackoverflow.com/questions/33968157/how-to-read-
 * characters-from-a-file-until-a-space-and-store-that-as-one-string-c
 */
void splitLineToArray(char *line, int array[3]) {
  int arrayIndex = 0;
  char *end = line;
  while(*end) {
    if (arrayIndex > 3) {
      perror("splitLineToArray goes wrong");
      exit(1);
    }
    array[arrayIndex] = strtol(line, &end, 10);
    arrayIndex += 1;
    while (*end == ' ') {
        end += 1;
    }
    line = end;
  }
}

int mapConstruction(struct cityMap* cityMaps) {
  int retval;
  FILE *mapFile = fopen("map.txt", "r");
  if (mapFile == NULL) {
    perror("open map.txt fail");
    exit(1);
  }
  char *line = NULL;
  int linelen = 0;
  int mapCount = -1;
  while (getline(&line, &linelen, mapFile) != -1) {
    line[strlen(line) - 2] = '\0';
    if (isalpha(line[0])) {
      mapCount += 1;
      cityMaps[mapCount].mapid = line[0];
      getline(&line, &linelen, mapFile);

      line[strlen(line) - 2] = '\0';
      cityMaps[mapCount].propSpeed = atof(line);

      getline(&line, &linelen, mapFile);
      line[strlen(line) - 2] = '\0';
      cityMaps[mapCount].tranSpeed = atof(line);
    }

    else{
      int info[3];
      splitLineToArray(line, info);
      int label1 = info[0];
      int label2 = info[1];
      int distance = info[2];
      int vertex1, vertex2;

      if ( !ifVertexInLabelArray(cityMaps[mapCount].labelArray, label1,
          cityMaps[mapCount].vertexNum)) {
        cityMaps[mapCount].labelArray[(cityMaps[mapCount].vertexNum)] = label1;
        cityMaps[mapCount].vertexNum += 1;
      }
      if ( !ifVertexInLabelArray(cityMaps[mapCount].labelArray, label2,
          cityMaps[mapCount].vertexNum)) {
        cityMaps[mapCount].labelArray[cityMaps[mapCount].vertexNum] = label2;
        cityMaps[mapCount].vertexNum += 1;
      }

      for (size_t i = 0; i < cityMaps[mapCount].vertexNum; i++) {
        if (cityMaps[mapCount].labelArray[i] == label1) {
          vertex1 = i;
        }
        if (cityMaps[mapCount].labelArray[i] == label2) {
          vertex2 = i;
        }
      }

      cityMaps[mapCount].adjMatrix[vertex1][vertex2] = distance;
      cityMaps[mapCount].adjMatrix[vertex2][vertex1] = distance;
      cityMaps[mapCount].edgeNum += 1;
    }
  }

  free(line);
  return mapCount + 1;
}

void mapConstructionPrint(int mapNum, struct cityMap* cityMaps) {
  // Print the on screen message for map construction.
  printf("The Server A has constructed a list of %d maps: \n", mapNum);
  printf("------------------------------\n");
  printf("Map ID    Num Vertices    Num Edges\n");
  for (size_t i = 0; i < mapNum; i++) {
    printf("%c        %d              %d\n", cityMaps[i].mapid,
      cityMaps[i].vertexNum, cityMaps[i].edgeNum);
  }
  printf("------------------------------\n");
}

int getSmallestIndex(int array[], int arrayLen) {
  int smallest = -1;
  int smallestIndex = -1;
  for (size_t i = 0; i < arrayLen; i++) {
    int e = array[i];
    if (e != -1 && (smallest == -1 || e < smallest)) {
      smallest = e;
      smallestIndex = i;
    }
  }
  return smallestIndex;
}

void pathFinding(int srcLabel, struct cityMap* city) {
  int srcVertex = -1;
  for (size_t i = 0; i < city->vertexNum; i++) {
    if (srcLabel == city->labelArray[i]) {
      srcVertex = i;
    }
  }
  if (srcVertex == -1) {
    perror("path finding locating srcVertex fail");
    exit(1);
  }

  int finishedNodesCount = 0;
  int finishedNodes[city->vertexNum]; // 0 is not finished, 1 is finished.

  int currentLen[city->vertexNum];

  for (size_t i = 0; i < city->vertexNum; i++) {
    finishedNodes[i] = 0;
    currentLen[i] = -1;
  }

  currentLen[srcVertex] = 0;
  while (finishedNodesCount != city->vertexNum) {
    // Find smallest unfinished node and length.
    int smallestNode;
    int smallestLen;
    bool initialized = false;
    for (size_t i = 0; i < city->vertexNum; i++) {
      if ( !finishedNodes[i]) {
        if (currentLen[i] != -1) {
          if (!initialized) {
            smallestLen = currentLen[i];
            smallestNode = i;
            initialized = true;
          }
          else {
            if (currentLen[i] < smallestLen) {
              smallestLen = currentLen[i];
              smallestNode = i;
            }
          }
        }
      }
    }
    finishedNodes[smallestNode] = 1;
    finishedNodesCount += 1;
    currentLen[smallestNode] = smallestLen;

    // Find adjacent nodes and distance, and update their currentLen.
    for (size_t i = 0; i < city->vertexNum; i++) {
      // i is the adjacent node.
      int distance = city->adjMatrix[smallestNode][i];
      if (distance != -1) {
        // If node i is unvisited.
        if (currentLen[i] == -1) {
          currentLen[i] = smallestLen + distance;
        }
        // If node i is visited before.
        else {
          int len = currentLen[i];
          currentLen[i] = smaller((smallestLen + distance), len);
        }
      }
    }
  }
  for (size_t i = 0; i < city->vertexNum; i++) {
    printf("shortest path for node %d is %d\n", i, currentLen[i]);
  }

}

// Reference: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
int main() {
  int sockFd;
  struct sockaddr_in mySock;

  // Creating socket file descriptor.
  sockFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sockFd < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&mySock, 0, sizeof(mySock));

  mySock.sin_family = AF_INET;
  mySock.sin_addr.s_addr = inet_addr(LIP);
  mySock.sin_port = htons(SAP);

  // Bind the socket with the server address.
  int retval = bind(sockFd, (const struct sockaddr *)&mySock, sizeof(mySock));
  if (retval < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in WSock;
  memset(&WSock, 0, sizeof(WSock));

  printf("The server A is up and running using UDP on port %d\n", SAP);

  struct cityMap cityMaps[100];
  for (size_t i = 0; i < 100; i++) {
    cityMaps[i].vertexNum = 0;
    cityMaps[i].edgeNum = 0;
    for (size_t j = 0; j < 10; j++) {
      cityMaps[i].labelArray[j] = -1;
      for (size_t k = 0; k < 10; k++) {
        cityMaps[i].adjMatrix[j][k] = -1;
      }
    }
  }
  int mapNum = mapConstruction(cityMaps);
  mapConstructionPrint(mapNum, cityMaps);

  // for (size_t i = 0; i < mapNum; i++) {
  //   printf("propSpeed is: %f\n", cityMaps[i].propSpeed);
  //   printf("tranSpeed is: %f\n", cityMaps[i].tranSpeed);
  //   printf("vertexNum is: %d\n", cityMaps[i].vertexNum);
  //   for (size_t j = 0; j < 10; j++) {
  //     printf("labelArray element is: %d\n", cityMaps[i].labelArray[j]);
  //   }
  //   printf("adjacency matrix: \n");
  //   for (size_t j = 0; j < 10; j++) {
  //     for (size_t k = 0; k < 10; k++) {
  //       printf("%d ", cityMaps[i].adjMatrix[j][k]);
  //     }
  //     printf("\n");
  //   }
  // }

  // Receive from AWS using UDP.
  // int WSockLen, n;
  // struct AQuery awsToAMsg;
  // n = recvfrom(sockFd, &awsToAMsg, sizeof(awsToAMsg),
  //             MSG_WAITALL, (struct sockaddr *) &WSock,
  //             &WSockLen);
  // printf("The Server A has received input for finding shortest paths: starting "
  //         "vertex %d of map %c\n", awsToAMsg.srcVertex, awsToAMsg.mapid);


  // sendto(sockFd, (const char *)hello, strlen(hello),
  //     MSG_CONFIRM, (const struct sockaddr *) &WSock,
  //         WSockLen);

  struct cityMap* cityPtr;

  for (size_t i = 0; i < mapNum; i++) {
    if (cityMaps[i].mapid == 'B') {
      pathFinding(8, &(cityMaps[i]));
      break;
    }
  }

  close(sockFd);
  return 0;
}

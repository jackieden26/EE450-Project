# EE 550 Socket Programming Project

## Author Info
Full Name: Yu Deng
Student ID: 3837426700

## Project File Overview
This is a socket programming project written in c. In this project, two TCP
sockets and three UDP sockets are made for host message exchange. The
Dijkstra's algorithm is implemented to find the shortest path given a map
and a source vertex.

### `client.c`
This file mainly creates a TCP socket with randomly assigned port number. Once
the client's socket address, aws's socket address, and the socket's file
descriptor is passed into `connect()`, client waits till the aws server
response. After aws server accept its connection, client sends an object of
type `struct clientQuery` containing necessary information, and aws will know
because it has a same struct definition.
Once aws finishes processing data, client will use the same socket to receive.
The client will need to receive multiple times, each time with data's type and
size that aws has agreed to send.

### `serverA.c`
This file creates a UDP socket, and use `recvfrom()` to receive aws' message
with specifed aws' socket address as one argument. This file opens the
`maps.txt` and extract data. It includes an array of `struct cityMap` that
stores each city's map info. Then, after receiving the mapid and source vertex,
it uses Dijkstra's algorithm to compute the shortest path. At, it sends the
needed info back to aws.

### `serverB.c`
Similar as `serverA.c`, this file also uses UDP socket to message with aws. This
server only needs to do the computation of time by dividing length with prop
speed, and filesize with data rate.

### `aws.c`
This server acts like a manager. It creates one TCP socket to communicate with
client, and two UDP to connect to backend server to exchange data. It has an
infinite loop right before `accept()`, so it is always ready to accept. In my
program, the backlog is set to 15.

## Message Exchange Format
The following are the objects type for message exchange. I will explain
why it looks a little bit complicated in the end.

### From client to aws
A `struct clientQuery` object is sent. It contains a mapid, a source vertex and
a filesize.

### From aws to server A
A `struct AQuery` object is sent. It contains the mapid and the source vertex.

### From server A to aws
Four objects are sent.
1. An integer, `arrayLen`, representing the number of destination
nodes it has.
2. An array, `destLenArray[arrayLen]`, with each element
being a pair of source vertex and the minumum length to reach.
3. A double number `propSpeed` representing the propogation speed in that city.
4. A double number `tranSpeed` representing the transmission speed.

### From aws to server B
Two objects are sent.
1. A `BQuery` object, containing propogation, transmission speed, number of
vertex, and file size.
2. The `destLenArray[arrayLen]` array same as above.

### From server B to aws.
Three objects are sent.
1. An array of transmission time.
2. An array of propogation time.
3. An array of delay.

### From aws to client
Five objects are sent.
1. The `arrayLen` integer same as above.
2. The `destLenArray[arrayLen]` array same as above.
3. The array of transmission time same as above.
4. The array of propogation time same as above.
5. The array of delay same as above.

The reason in the last several cases I didn't use one struct to include all the
information is that, I first thought that, the array stored in a struct is only
a pointer to that array, and it make no sense if I send that pointer address.
However it is I almost finished this project that I found that, if I define the
array size when I declare it, which is what I did, then all the elements of that
array will occupy memory in the struct, instead of only a pointer. By noticing
this late that I make the message exchange less cleaner.

## Idiosyncrasy
1. The user input has to be valid, meaning the source node exists, map id
matches
strictly (upper and lower are different), and file size not too big to overflow
float type number. Otherwise my program won't work as expected.
2. `map.txt` has to be valid and strictly follow the style given in instruction.
3. At the beginning of writing the code, I used `perror()` to check every
syscall.
However I found the code too long and too unreadable. So most of my syscall does
not have error check. If somehow one syscall fails, for example, UDP message is
not delivered, my program won't notice nor terminate, which is not good
practice.
4. Other than those above, as long as input is formatted well, my program
should be fine.

## Reference
1. In `serverA.c`, my `splitLineToArray()` is referred to
https://stackoverflow.com/questions/33968157/how-to-read-characters-from-a-file-
until-a-space-and-store-that-as-one-string-c
2. In `serverA.c`, I used https://en.wikipedia.org/wiki/Insertion_sort to do an
insertion sort.
3. In `serverA.c`, I read
https://www.geeksforgeeks.org/udp-server-client-implementation-c/
to help to write socket creation and connection.
Reference 1 and 3 is not directly copied into my program, but they do
inspire me to write my own.
Reference 2, the insertion sort, is heavily used.

## Things I can do better.
1. Use c++. It has so many good data structure like min-heap so I don't need to
write
the sort myself. I can also overload the operator so I don't need to declare
a lot of index matching arrays.
2. Put some repeated codes into functions. I didn't do so because in c, if I
want to pass in an array, I need to pass its length too. So I saved myself some
time not writing those bothering functions if I don't need to.
3. Do error check. I just didn't have that much time doing it.
4. Use signal handler to close the open socket in servers. Same reason, no time.

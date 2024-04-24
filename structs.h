#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memset
#include <unistd.h> // For close
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024
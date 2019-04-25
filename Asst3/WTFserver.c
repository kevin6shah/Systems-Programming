#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

void* readd(void* socket) {
  int client_socket = *(int*) socket;
  int i;
  while(1) {
    char temp;
    char buffer[250];
    i = 0;
    while (read(client_socket, &temp, 1) > 0 && temp != '$') {
      buffer[i] = temp;
      i++;
    }
    buffer[i] = '\0';
    if (strcmp(buffer, "bye") == 0) {
      printf("Client disconnected from the server!\n\n");
      break;
    }
    printf("%s\n", buffer);
  }
  close(client_socket);
  return NULL;
}

int main(int argc, char** argv) {

  if (argc != 2) {
    printf("Usage: %s <Host>\n", argv[0]);
    return 0;
  }

  // Create the socket
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    printf("Issue trying to create socket\n");
    return 0;
  }

  int enable = 1;
  if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    printf("setsockopt(SO_REUSEADDR) failed\n");

  // Make the address
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(atoi(argv[1]));
  address.sin_addr.s_addr = INADDR_ANY;

  // BIND
  if (bind(server_socket, (struct sockaddr*) &address, sizeof(address)) < 0) {
    printf("Issue trying to bind the socket\n");
    return 0;
  }

  // LISTEN
  listen(server_socket, 5);

  // ACCEPT
  while(1) {
    pthread_t thread;
    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket != -1) {
      printf("Established connection with a client!\n");
      pthread_create(&thread, NULL, readd, (void*) &client_socket);
    }
  }

  close(server_socket);
  return 0;
}

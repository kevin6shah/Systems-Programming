#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>

int client_socket;

void handle() {
  printf("\n\nClient was interrupted... killing process now!\n");
  write(client_socket, "bye$", strlen("bye$"));
  kill(getpid(), SIGTERM);
}

void configure(char* ip, char* host) {
  int fd;
  if ((fd = open("./.configure", O_RDONLY)) >= 0) {
    // Already Configured
    close(fd);
    printf("IP Address and Host are already configured!\n");
    return;
  } else {
    // Not configured
    fd = open("./.configure", O_WRONLY | O_CREAT, 0700);
    if (fd < 0) {
      printf("Could not create a config file!\n");
      return;
    }
    write(fd, ip, strlen(ip));
    write(fd, "\t", 1);
    write(fd, host, strlen(host));
    printf(".configure file was created sucessfully!\n");
  }
}

int connect_client() {
  int fd = open("./.configure", O_RDONLY);
  if (fd < 0) {
    printf("The client is not configured... Please try configuring first!\n");
    return 0;
  }
  // Gets the IP and Host from the Config file
  char ip[250], host[250];
  char temp;
  int i = 0, dot = 0, valid = 1;
  while (read(fd, &temp, 1) > 0 && temp != '\t') {
    if (temp == '.') dot = 1;
    if (isalpha(temp)) valid = 0;
    ip[i] = temp;
    i++;
  }
  ip[i] = '\0';
  if (dot == 0 || valid == 0 || ip == NULL) {
    printf(".configure file is invalid/corrupted!\n");
    return 0;
  }

  bzero(host, 250);
  int n = read(fd, host, 250);
  if (n < 0) {
    printf("Host is Incorrect!\n");
    return 0;
  }
  valid = 1;
  for (i = 0; i < strlen(ip); i++) {
    if (isalpha(ip[i])) {
      valid = 0;
      break;
    }
  }
  if (host == NULL || valid == 0) {
    printf("Host is Incorrect!\n");
    return 0;
  }

  struct hostent *server = gethostbyname(ip);
  if (server < 0) {
    printf("Error: Given IP Address is invalid\n");
    return 0;
  }

  // Create the Socket
  client_socket = socket(AF_INET, SOCK_STREAM, 0);

  // Initialize the address
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(atoi(host));
  bcopy((char *) server->h_addr, (char*) &address.sin_addr.s_addr, server->h_length);

  signal(SIGINT,handle);
  // Connect to the Socket
  do {
    int connection_status = connect(client_socket, (struct sockaddr*) &address, sizeof(address));
    if (connection_status < 0) {
      printf("Could not connect to the server!\nTrying again...\n");
      sleep(3);
    } else {
      printf("\nEstablished connection with the server!\n\n");
      break;
    }
  } while (1);
  return 1;
}

int main(int argc, char** argv) {

  if (argc != 3 && argc != 4) {
    printf("Usage:\t%s configure <IP Address> <Port>\n", argv[0]);
    printf("\t%s checkout <Project Name>\n", argv[0]);
    printf("\t%s update <Project Name>\n", argv[0]);
    printf("\t%s upgrade <Project Name>\n", argv[0]);
    printf("\t%s commit <Project Name>\n", argv[0]);
    printf("\t%s push <Project Name>\n", argv[0]);
    printf("\t%s create <Project Name>\n", argv[0]);
    printf("\t%s destroy <Project Name>\n", argv[0]);
    printf("\t%s add <Project Name> <Filename>\n", argv[0]);
    printf("\t%s remove <Project Name> <Filename>\n", argv[0]);
    printf("\t%s currentversion <Project Name>\n", argv[0]);
    printf("\t%s history <Project Name>\n", argv[0]);
    printf("\t%s rollback <Project Name> <Version>\n", argv[0]);
    return 0;
  }

  if (strcmp(argv[1], "configure") == 0) {
    configure(argv[2], argv[3]);
    return 0;
  }

  signal(SIGINT,handle);

  if (!connect_client()) return 0;

  // Recieve Data
  int n;
  while (1) {
    char str[250];
    fgets(str, 250, stdin);
    if (strcmp(str, "bye\n") == 0) {
      strcat(str, "$");
      write(client_socket, "bye", strlen("bye"));
      break;
    }
    strcat(str, "$");
    write(client_socket, str, strlen(str));
  }

  close(client_socket);

  return 0;
}

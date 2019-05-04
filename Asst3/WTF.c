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

int exists(char* path, char* project_name) {
  DIR *directory = opendir(path);
  if (directory == NULL) {
    printf("Could not open directory\n");
    return;
  }
  struct dirent *data;
  while ((data = readdir(directory)) != NULL) {
    if (data->d_type == 4 && strcmp(data->d_name, ".") != 0 && strcmp(data->d_name, "..") != 0) {
      if (strcmp(data->d_name, project_name) == 0) return 1;
    }
  }
  closedir(directory);
  return 0;
}

int recieve() {
  char token[255];
  char c;
  int ready = 0, i = 0, size = 0, fd;
  while (1) {
    int n = read(client_socket, &c, 1);
    if (n <= 0) break;
    token[i] = c;
    if (i > 5 && ready == 0 && token[i] == 'N' && token[i-1] == 'E' &&
    token[i-2] == 'K' && token[i-3] == 'O' && token[i-4] == 'T' && token[i-5] == '$') {
      token[i-5] = '\0';
      ready = 1;
    }
    i++;
    if (ready == 1) {
      // ON SUCCESS SERVER SENDS $***$
      if (strcmp(token, "$***$") == 0) break;

      // ON FAILURE SERVER SENDS $***$
      if (strcmp(token, "$FFF$") == 0) return 0;

      // Token is ready
      if (size == 1) {
        // Size of A File -> We know
        size = atoi(token);
        if (size == 0) {
          printf("An error occured with size\n");
          return 0;
        }
        char *buffer = malloc(size+1);
        char tmp;
        int index = 0;
        do {
          int n = read(client_socket, &tmp, 1);
          if (n < 0) {
            printf("An error occured with read\n");
            return 0;
          }
          buffer[index] = tmp;
          index++;
        } while(index < size);

        char temp[6];
        int n = read(client_socket, temp, 6);
        if (n < 0) {
          printf("An error occured with read\n");
          return 0;
        }
        n = write(fd, buffer, size);
        if (n < 0) {
          printf("An error occured with write\n");
          return 0;
        }
        size = 0;
      } else if (token[0] == '#') {
        // Directory
        int n = mkdir(token+1, 0700);
        if (n < 0) {
          printf("Error trying to make '%s' directory\n", token+1);
          return 0;
        }
      } else if (token[0] == '*') {
        // File
        fd = open(token+1, O_WRONLY | O_CREAT, 0700);
        if (fd < 0) {
          printf("Error trying to make '%s' file\n", token+1);
          return 0;
        }
        size = 1;
      } else {
        printf("Structural Damage\n");
        return 0;
      }
      bzero(token, 255);
      i = 0;
      ready = 0;
    }
  }
  return 1;
}

int checkout(char* project_name) {
  if (exists("./", project_name) || (!connect_client())) {
    return 0;
  }
  write(client_socket, "checkout:", strlen("checkout:"));
  write(client_socket, project_name, strlen(project_name));
  write(client_socket, "$TOKEN", strlen("$TOKEN"));
  if (!recieve()) {
    printf("There occured an error checking out...\n");
    return 0;
  }
  printf("Checkout was successful...\n");
  return 1;
}

int create(char* project_name) {
  if (exists("./", project_name) || (!connect_client())) {
    return 0;
  }
  write(client_socket, "create:", strlen("create:"));
  write(client_socket, project_name, strlen(project_name));
  write(client_socket, "$TOKEN", strlen("$TOKEN"));
  if (!recieve()) {
    printf("There occured an error creating the project...\n");
    return 0;
  }
  printf("Create was successful...\n");
  return 1;
}

int update(char* project_name) {
  if (!connect_client()) {
    return 0;
  }
  write(client_socket, "update:", strlen("update:"));
  write(client_socket, project_name, strlen(project_name));
  write(client_socket, "$TOKEN", strlen("$TOKEN"));
  if (!recieve()) {
    printf("There occured an error recieving the .Manifest from the server...\n");
    return 0;
  }
  printf("Update was successful...\n");
  return 1;
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

  signal(SIGINT,handle);

  if (strcmp(argv[1], "configure") == 0) {
    configure(argv[2], argv[3]);
    return 0;
  } else if (strcmp(argv[1], "checkout") == 0) {
    if (!checkout(argv[2])) {
      printf("Checkout failed...\n");
    }
  } else if (strcmp(argv[1], "create") == 0) {
    if (!create(argv[2])) {
      printf("Create failed...\n");
    }
  } else if (strcmp(argv[1], "update") == 0) {
    if (!update(argv[2])) {
      printf("Update failed...\n");
    }
  }

  close(client_socket);

  return 0;
}

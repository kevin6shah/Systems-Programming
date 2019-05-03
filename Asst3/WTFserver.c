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
#include <dirent.h>

int server_socket;

int send_file(int client_socket, char* server_path) {
  int fd = open(server_path, O_RDONLY);
  if (fd < 0) {
    printf("Incorrect path entered\n");
    return 0;
  }

  struct stat stats;
  stat(server_path, &stats);
  int buf_size = stats.st_size;
  char size[255];
  sprintf(size, "%d", buf_size);
  write(client_socket, size, strlen(size));
  write(client_socket, "$TOKEN", strlen("$TOKEN"));

  char *buffer = malloc(buf_size + 1);
  int n = read(fd, buffer, buf_size);
  if (n < 0) {
    printf("Error Reading\n");
    return 0;
  }
  n = write(client_socket, buffer, buf_size);
  if (n < 0) {
    printf("Error Writing\n");
    return 0;
  }

  write(client_socket, "$TOKEN", strlen("$TOKEN"));
  return 1;
}

void senddir(int client_socket, char* server_path, char* client_path) {
  DIR *directory = opendir(server_path);
  if (directory == NULL) {
    printf("Could not open directory\n");
    return;
  }
  struct dirent *data;
  while ((data = readdir(directory)) != NULL) {
    if (data->d_type == 4 && strcmp(data->d_name, ".") != 0 && strcmp(data->d_name, "..") != 0) {
      // Directory
      char temp_path[255];
      strcpy(temp_path, client_path);
      strcat(temp_path, "/");
      strcat(temp_path, data->d_name);

      char new_path[255];
      strcpy(new_path, server_path);
      strcat(new_path, "/");
      strcat(new_path, data->d_name);

      write(client_socket, "#", strlen("#"));
      write(client_socket, temp_path, strlen(temp_path));
      write(client_socket, "$TOKEN", strlen("$TOKEN"));

      senddir(client_socket, new_path, temp_path);
    } else if (data->d_name[0] != '.') {
      // File
      write(client_socket, "*", strlen("*"));
      write(client_socket, client_path, strlen(client_path));
      write(client_socket, "/", strlen("/"));
      write(client_socket, data->d_name, strlen(data->d_name));
      write(client_socket, "$TOKEN", strlen("$TOKEN"));

      char new_path[250];
      strcpy(new_path, server_path);
      strcat(new_path, "/");
      strcat(new_path, data->d_name);
      if (!send_file(client_socket, new_path)) {
        printf("Error: Could not send '%s' to the server\n", new_path);
        write(client_socket, "$FFF$$TOKEN", strlen("$FFF$$TOKEN"));
      }
    }
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

int checkout(int client_socket, char* project_name) {
  if (!exists("./", ".project_repo")) {
    mkdir(".project_repo", 0700);
    return 0;
  }
  if (!exists(".project_repo/", project_name)) {
    return 0;
  }
  // NEED TO WORK ON BELOW
  write(client_socket, "#", strlen("#"));
  write(client_socket, project_name, strlen(project_name));
  write(client_socket, "$TOKEN", strlen("$TOKEN"));
  char server_path[250] = ".project_repo/";
  strcat(server_path, project_name);
  senddir(client_socket, server_path, project_name);
  printf("Checkout succeeded...\n");
  return 1;
}

void* main_process(void* socket) {
  int client_socket = *(int*) socket;
  int i = 0, seen_colon = 0;
  char token[255], project_name[255];

  char c;
  while(read(client_socket, &c, 1) > 0) {
    if (c == ':') {
      token[i] = '\0';
      i = -1;
      seen_colon = 1;
    }
    if (seen_colon == 0) {
      token[i] = c;
    } else if (seen_colon == 1 && i >= 0) {
      project_name[i] = c;
    }
    if (seen_colon == 1 && project_name[i] == 'N' && project_name[i-1] == 'E' && project_name[i-2] == 'K' &&
        project_name[i-3] == 'O' && project_name[i-4] == 'T' && project_name[i-5] == '$') {
        project_name[i-5] = '\0';
        break;
    }
    i++;
  }

  if (strcmp(token, "checkout") == 0) {
    if (!checkout(client_socket, project_name)) {
      write(client_socket, "$FFF$$TOKEN", strlen("$FFF$$TOKEN"));
      printf("Checkout failed...\n");
    } else write(client_socket, "$***$$TOKEN", strlen("$***$$TOKEN"));
  }
  // Else ifs after this

  close(client_socket);
  return NULL;
}

int connect_server(char *port) {
  // Create the socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
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
  address.sin_port = htons(atoi(port));
  address.sin_addr.s_addr = INADDR_ANY;

  // BIND
  if (bind(server_socket, (struct sockaddr*) &address, sizeof(address)) < 0) {
    printf("Issue trying to bind the socket\n");
    return 0;
  }

  // LISTEN
  listen(server_socket, 50);

  pthread_t thread[60];
  int i = 0;
  // ACCEPT
  while(i < 2) {
    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket != -1) {
      printf("Established connection with a client!\n");
      pthread_create(&thread[i++], NULL, main_process, (void*) &client_socket);
    }
    if (i >= 50) {
      i = 0;
      while(i < 50) {
        pthread_join(thread[i++],NULL);
      }
      i = 0;
    }
  }
  return 1;
}

int main(int argc, char** argv) {

  if (argc != 2) {
    printf("Usage: %s <Host>\n", argv[0]);
    return 0;
  }

  if (!connect_server(argv[1])) {
    printf("Could not establish a connection with the server\n");
    return 0;
  }

  return 0;
}

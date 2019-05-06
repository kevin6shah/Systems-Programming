#include "data.h"

int server_socket;

int send_file(int client_socket, char* server_path) {
  int fd = open(server_path, O_RDONLY);
  if (fd < 0) {
    printf("Incorrect path entered\n");
    close(fd);
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
  close(fd);
  return 1;
}

int recieve(int client_socket) {
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
          close(fd);
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
  close(fd);
  return 1;
}

void senddir_helper(int client_socket, char* server_path, char* client_path) {
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

      senddir_helper(client_socket, new_path, temp_path);
    } else if (data->d_type != 4) {
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
        printf("Error: Could not send '%s' to the client\n", new_path);
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

int most_recent_version(char* path) {
  DIR *directory = opendir(path);
  if (directory == NULL) {
    printf("Could not open directory\n");
    return 0;
  }
  int recent_version = 0;
  struct dirent *data;
  while ((data = readdir(directory)) != NULL) {
    if (data->d_type == 4 && strcmp(data->d_name, ".") != 0 && strcmp(data->d_name, "..") != 0) {
      // Directory
      int temp = atoi((data->d_name) + 7);
      if (temp == 0) return 0;
      int current_version = temp;
      if (current_version > recent_version) {
        recent_version = current_version;
      }
    }
  }
  return recent_version;
}

int senddir(int client_socket, char* project_name) {
  write(client_socket, "#", strlen("#"));
  write(client_socket, project_name, strlen(project_name));
  write(client_socket, "$TOKEN", strlen("$TOKEN"));
  char server_path[255] = ".server_repo/";
  strcat(server_path, project_name);
  strcat(server_path, "/");

  int recent_version = most_recent_version(server_path);
  char rec_version[255];
  sprintf(rec_version, "%d", recent_version);
  strcat(server_path, "version");
  strcat(server_path, rec_version);
  senddir_helper(client_socket, server_path, project_name);
  return 1;
}

int checkout(int client_socket, char* project_name) {
  if (!exists("./", ".server_repo") || !exists(".server_repo/", project_name)) {
    return 0;
  }

  if (!senddir(client_socket, project_name)) return 0;
  printf("Checkout succeeded...\n");
  return 1;
}

int create(int client_socket, char* project_name) {
  char path[255] = ".server_repo/";
  if (!exists("./", ".server_repo")) {
    mkdir(".server_repo", 0700);
  } else {
    if (exists(path, project_name)) return 0;
  }
  strcat(path, project_name);
  mkdir(path, 0700);
  strcat(path, "/version0");
  mkdir(path, 0700);
  strcat(path, "/.Manifest");
  int fd = open(path, O_WRONLY | O_CREAT, 0700);
  if (fd < 0) {
    printf("Could not create the Manifest file...\n");
    close(fd);
    return 0;
  }
  write(fd, "0\n", strlen("0\n"));
  close(fd);
  if (!senddir(client_socket, project_name)) return 0;
  printf("Create was successful\n");
  return 1;
}

int update(int client_socket, char* project_name) {
  if (!exists("./", ".server_repo") || !exists(".server_repo/", project_name)) {
    return 0;
  }
  write(client_socket, "*", 1);
  write(client_socket, ".Manifest", strlen(".Manifest"));
  write(client_socket, "$TOKEN", strlen("$TOKEN"));

  char new_path[255] = ".server_repo/";
  strcat(new_path, project_name);
  strcat(new_path, "/");
  int recent_version = most_recent_version(new_path);
  char version[255];
  sprintf(version, "%d", recent_version);
  strcat(new_path, "version");
  strcat(new_path, version);
  strcat(new_path, "/.Manifest");
  if (!send_file(client_socket, new_path)) {
    printf("Error: Could not send '%s' to the server\n", new_path);
    return 0;
  }
  return 1;
}

int push(int client_socket, char* project_name) {
  if (!exists("./", ".server_repo") || !exists(".server_repo/", project_name)) {
    return 0;
  }
  char new_path[255] = ".server_repo/";
  strcat(new_path, project_name);
  strcat(new_path, "/");
  int version = most_recent_version(new_path);
  char vp[255];
  sprintf(vp, "%d", version);
  write(client_socket, vp, strlen(vp));
  write(client_socket, "$", 1);
  if (!recieve(client_socket)) return 0;
  printf("Push was successful...\n");
  return 1;
}

int commit(int client_socket, char* project_name) {
  if (!exists("./", ".server_repo") || !exists(".server_repo/", project_name)) {
    return 0;
  }
  write(client_socket, "*", 1);
  write(client_socket, ".Manifest", strlen(".Manifest"));
  write(client_socket, "$TOKEN", strlen("$TOKEN"));

  char new_path[255] = ".server_repo/";
  strcat(new_path, project_name);
  strcat(new_path, "/");
  int recent_version = most_recent_version(new_path);
  char version[255];
  sprintf(version, "%d", recent_version);
  strcat(new_path, "version");
  strcat(new_path, version);
  strcat(new_path, "/.Manifest");
  if (!send_file(client_socket, new_path)) {
    printf("Error: Could not send '%s' to the server\n", new_path);
    return 0;
  }
  write(client_socket, "$***$$TOKEN", strlen("$***$$TOKEN"));
  write(client_socket, version, strlen(version));
  write(client_socket, "$", strlen("$"));
  if (!recieve(client_socket)) {
    printf("Error: Commit failed\n");
    return 0;
  }
  printf("Commit Succeeded\n");
  return 1;
}

void* main_process(void* socket) {
  int client_socket = *(int*) socket;
  int i = 0, seen_colon = 0;
  char token[255], project_name[255], file_path[255];

  char c;
  while(read(client_socket, &c, 1) > 0) {
    if (c == ':') {
      if (seen_colon == 1) {
        project_name[i] = '\0';
        seen_colon = 2;
      } else {
        token[i] = '\0';
        seen_colon = 1;
      }
      i = -1;
    }
    if (seen_colon == 0) {
      token[i] = c;
    } else if (seen_colon == 1 && i >= 0) {
      project_name[i] = c;
    } else if (seen_colon == 2 && i >= 0) {
      file_path[i] = c;
    }
    if (seen_colon == 1 && project_name[i] == 'N' && project_name[i-1] == 'E' && project_name[i-2] == 'K' &&
        project_name[i-3] == 'O' && project_name[i-4] == 'T' && project_name[i-5] == '$') {
        project_name[i-5] = '\0';
        break;
    }
    if (seen_colon == 2 && file_path[i] == 'N' && file_path[i-1] == 'E' && file_path[i-2] == 'K' &&
        file_path[i-3] == 'O' && file_path[i-4] == 'T' && file_path[i-5] == '$') {
        file_path[i-5] = '\0';
        break;
    }
    i++;
  }

  if (strcmp(token, "checkout") == 0) {
    if (!checkout(client_socket, project_name)) {
      write(client_socket, "$FFF$$TOKEN", strlen("$FFF$$TOKEN"));
      printf("Checkout failed...\n");
    } else write(client_socket, "$***$$TOKEN", strlen("$***$$TOKEN"));
  } else if (strcmp(token, "create") == 0) {
    if (!create(client_socket, project_name)) {
      write(client_socket, "$FFF$$TOKEN", strlen("$FFF$$TOKEN"));
      printf("Create failed...\n");
    } else write(client_socket, "$***$$TOKEN", strlen("$***$$TOKEN"));
  } else if (strcmp(token, "update") == 0) {
    if (!update(client_socket, project_name)) {
      write(client_socket, "$FFF$$TOKEN", strlen("$FFF$$TOKEN"));
      printf("Update failed...\n");
    } else write(client_socket, "$***$$TOKEN", strlen("$***$$TOKEN"));
  } else if (strcmp(token, "commit") == 0) {
    if (!commit(client_socket, project_name)) {
      write(client_socket, "$FFF$$TOKEN", strlen("$FFF$$TOKEN"));
      printf("Commit failed...\n");
    } else write(client_socket, "$***$$TOKEN", strlen("$***$$TOKEN"));
  } else if (strcmp(token, "push") == 0) {
    if (!push(client_socket, project_name)) {
      write(client_socket, "$FFF$$TOKEN", strlen("$FFF$$TOKEN"));
      printf("Commit failed...\n");
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
  while(1) {
    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket != -1) {
      printf("Established connection with a client!\n");
      pthread_create(&thread[i], NULL, main_process, (void*) &client_socket);
    }
    i++;
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

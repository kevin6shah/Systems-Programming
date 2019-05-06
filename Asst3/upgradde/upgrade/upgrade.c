#include "data.h"




















//parsing types : "manifest", or, "commit", or "update"
node* parse_update_file(char* update_path){
  //char* manifest_path = ".Manifest";
  int fd = open(update_path, O_RDONLY);
  if (fd < 0) {
    printf("Can't open the manifest file\n");
    close(fd);
    return 0;
  }

  //create linked list out of all the U in update.
  node* head = malloc (sizeof(node));
  head = NULL;
  //this will store the server's .Manifest file
  //now to parse the file and actually store it inside

//read line by line
int version_manifest;
int not_end_of_file = 1; //if 1, end of file;
while(not_end_of_file){
  char c;
  int num = 1000;
  char* line_buffer = malloc(num);
  int count = 0;
  while((not_end_of_file = read(fd, &c, 1)) > 0 && c != '\n'){
    line_buffer[count] = c;
    count++;
    if (count > num - 5) {
      num*=2;
      line_buffer = realloc(line_buffer, num);
    }
  }
  line_buffer[count] = '\t';
  if (num == count) line_buffer = realloc(line_buffer, num + 2);
  line_buffer[++count] = '\0';
  if (strlen(line_buffer) < 1){
    break;
  }
  node* hashnode = malloc(sizeof(node));
  hashnode = NULL;

  //now we parse through each line.
  //reminder: .Update file structure?
  //<M/A/D> <tab> <version_num> <tab> <filename> <tab> <filepath> <tab> <hashcode>

  if (strlen(line_buffer) < 2) continue;
  int i, j = 0, tabs_seen = 0;

  char token[PATH_MAX];
  for (i = 0; i < strlen(line_buffer); i++) {
    if (line_buffer[i] == '\t') {
      token[j] = '\0';
      tabs_seen++;
      if (tabs_seen == 1) {
        hashnode->update_id = token[0];
      } else if (tabs_seen == 2) {
        hashnode->version = atoi(token);
      } else if (tabs_seen == 3) {
        hashnode->filename = malloc(strlen(token) + 1);
        strcpy(hashnode->filename, token);
      } else if(tabs_seen == 4){
        hashnode->filepath = malloc(strlen(token) + 1);
        strcpy(hashnode->filepath, token);
      } else if(tabs_seen == 5){
        hashnode->code = malloc(strlen(token) + 1);
        strcpy(hashnode->code, token);
      }
      j = 0;
      bzero(token, PATH_MAX);
      continue;
    }
    token[j] = line_buffer[i];
    j++;
  }

  //nodeInsert(hashnode, HashTable);
  head = linked_list_insert(head, hashnode);

}

return head;
}


int remuuv(char* project_name, char* file_old_path){
  int client_manifest_version;
  char path[255];
  strcpy(path, project_name);
  strcat(path, "/.Manifest");
  node **hash_client = parse_manifest(path, &client_manifest_version);

  char* file_path = malloc(strlen(project_name) + strlen(file_old_path) + 2);
  strcpy(file_path, project_name);
  strcat(file_path, "/");
  strcat(file_path, file_old_path);

  node* result;
  node* tmp = malloc(sizeof(node));
  tmp->filepath = malloc(255);
  strcpy(tmp->filepath, file_path);
  tmp->filename = malloc(255);
  strcpy(tmp->filename, get_name(file_path));
  result = search_node(tmp, hash_client);

  if (strcmp(result->filename, "does not exist") != 0) {
    //Path exists on the manifest --> Remove it
    hash_client[getkey(result->filename)] = delete(result->filepath,hash_client[getkey(result->filename)]);
  } else {
    //Path does not exist --> Error
    printf("Error: Could not remove the file at '%s'\n", file_path);
    return 0;
  }
  remove(path);
  if (make_manifest(hash_client, path, client_manifest_version)) {
    printf("Sucessfully removed '%s'\n", file_old_path);
    return 1;
  }
  return 0;
}











int upgrade(char* project_name){

  char *update_path = malloc (sizeof(1000));
  strcpy(update_path, project_name);
  strcat(update_path, ".Update");
  //check if there actually exists a .Update file
  int fd = open(".Update", O_RDONLY);
  if (fd < 0) {
    printf(".Update file does not exist dawg\n");
    close(fd);
    return 0;
  }
  close(fd);
  //Parse through .Update file, create linked list of operations
  node *head = parse_update_file(update_path);

  //traverse .Update and implement changes
  node *ptr;
  for (ptr = head; ptr != NULL; ptr = ptr->next){
    if(ptr->update_id == 'D'){
      remuuv(project_name, ptr->filepath);
    } else if(ptr->update_id == 'M'){
      //fetch ptr->filepath (specific file) from server
      //servers path : .server_repo/(most recent version)/
    }
  }




}




int main(int argc, char* argv[]){




  return 0;

}

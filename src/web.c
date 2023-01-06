#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
  int server_fd, client_fd;
  struct sockaddr_in server_address, client_address;
  socklen_t client_address_size;
  char buffer[BUFFER_SIZE];

  // Create a socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("Error creating socket");
    exit(EXIT_FAILURE);
  }

  // Set the socket options
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("Error setting socket options");
    exit(EXIT_FAILURE);
  }

  // Set the address and port for the socket
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(PORT);

  // Bind the socket to the address and port
  if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    perror("Error binding socket");
    exit(EXIT_FAILURE);
  }

  // Start listening for incoming connections
  if (listen(server_fd, 3) < 0) {
    perror("Error listening for incoming connections");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Accept an incoming connection
    client_address_size = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_size);
    if (client_fd < 0) {
      perror("Error accepting incoming connection");
      exit(EXIT_FAILURE);
    }

    // Read the request from the client
    int bytes_read = read(client_fd, buffer, BUFFER_SIZE);
    if (bytes_read < 0) {
      perror("Error reading request from client");
      exit(EXIT_FAILURE);
    }

    char request_method[BUFFER_SIZE];
    char request_path[BUFFER_SIZE];
    if (sscanf(buffer, "%s %s", request_method, request_path) != 2) {
    perror("Error parsing request");
    exit(EXIT_FAILURE);
    }

    // Get the full file path by concatenating the request path with the base directory
    char file_path[BUFFER_SIZE];
    sprintf(file_path, "/path/to/folder%s", request_path);

    // Open the file
    FILE *requested_file = fopen(file_path, "r");
    if (requested_file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
    }

    // Read the contents of the file into a buffer
    char file_buffer[BUFFER_SIZE];
    size_t file_size = fread(file_buffer, 1, BUFFER_SIZE, requested_file);
    if (file_size == 0) {
    perror("Error reading file");
    exit(EXIT_FAILURE);
    }

    // Send the file contents as the response to the client
    write(client_fd, file_buffer, file_size);

    // Close the file
    fclose(requested_file);

    // Close the client socket
    close(client_fd);
  }

  return 0;
}

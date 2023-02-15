#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
  int sock = 0, image_size;
  struct sockaddr_in serv_addr;
  char buffer[BUFFER_SIZE] = {0};
  char *filename = "the-vert.png";
  FILE *image_file;
  struct stat st;

  // Open image file
  image_file = fopen(filename, "rb");

  if (image_file == NULL) {
    perror("Failed to open file");
    return -1;
  }

  // Get image size
  if (stat(filename, &st) == 0) {
    image_size = st.st_size;
  } else {
    perror("Failed to get file size");
    return -1;
  }

  // Create socket
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation failed");
    return -1;
  }

  // Set server address and port
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    perror("Invalid address/ Address not supported");
    return -1;
  }

  // Connect to server
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Connection failed");
    return -1;
  }

  // Send image to server
  if (send(sock, &image_size, sizeof(image_size), 0) < 0) {
    perror("Send failed");
    return -1;
  }

  while (!feof(image_file)) {
    fread(buffer, sizeof(char), BUFFER_SIZE, image_file);

    if (send(sock, buffer, BUFFER_SIZE, 0) < 0) {
      perror("Send failed");
      return -1;
    }
  }

  fclose(image_file);

  // Receive response from server
  if (recv(sock, buffer, BUFFER_SIZE, 0) < 0) {
    perror("Receive failed");
    return -1;
  }

  printf("%s\n", buffer);

  return 0;
}

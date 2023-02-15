#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 5

int num_clients = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void *connection_handler(void *);

int main(int argc, char const *argv[]) {
  int server_fd, new_socket, *new_sock;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  pthread_t thread;

  // Create socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Socket creation failed");
    return -1;
  }

  // Set server address and port
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Bind socket to address and port
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Bind failed");
    return -1;
  }

  // Listen for incoming connections
  if (listen(server_fd, 3) < 0) {
    perror("Listen failed");
    return -1;
  }

  printf("Waiting for connections...\n");

while (1) {
    // Check if maximum client count has been reached
    pthread_mutex_lock(&mutex);
    if (num_clients >= MAX_CLIENTS) {
      pthread_mutex_unlock(&mutex);
      printf("Maximum number of clients reached. Waiting for a client to disconnect...\n");
      sleep(1);
      continue;
    }
    pthread_mutex_unlock(&mutex);

    // Accept incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
      perror("Accept failed");
      continue;
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    // Create new thread for connection
    new_sock = malloc(sizeof(*new_sock));
    *new_sock = new_socket;

    pthread_mutex_lock(&mutex);
    num_clients++;
    pthread_mutex_unlock(&mutex);

    if (pthread_create(&thread, NULL, connection_handler, (void *)new_sock) < 0) {
      perror("Thread creation failed");
      return -1;
    }
  }

  return 0;
}


void *connection_handler(void *socket_desc) {
  int sock = *(int *)socket_desc, image_size, received_size = 0;
  char buffer[BUFFER_SIZE] = {0};
  FILE *image_file;

  // Receive image size from client
  if (recv(sock, &image_size, sizeof(image_size), 0) < 0) {
    perror("Receive failed");
    return NULL;
  }

  printf("Received image size: %d bytes\n", image_size);

  // Open image file for writing
  image_file = fopen("img_serv/received_image.png", "wb");

  if (image_file == NULL) {
    perror("Failed to create file");
    return NULL;
  }

  // Receive image data from client
  while (received_size < image_size) {
    int bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);

    if (bytes_received < 0) {
      perror("Receive failed");
      return NULL;
    }

    fwrite(buffer, sizeof(char), bytes_received, image_file);
    received_size += bytes_received;
  }

  fclose(image_file);

  // Send response to client
  strcpy(buffer, "Image received successfully");
  if (send(sock, buffer, strlen(buffer), 0) < 0) {
    perror("Send failed");
    return NULL;
  }

  printf("Image received and saved\n");

  free(socket_desc);
  return NULL;
}
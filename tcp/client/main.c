#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  char *message;
  if (argc == 1) {
    message = "1234567_";
  } else {
    message = argv[1];
  }
  struct sockaddr_in peer;
  peer.sin_family = AF_INET;
  peer.sin_port = htons(7500);
  peer.sin_addr.s_addr = inet_addr("127.0.0.1");

  int _socket = socket(AF_INET, SOCK_STREAM, 0);
  if (_socket < 0) {
    perror("Socket error");
    return 1;
  }

  if (connect(_socket, (struct sockaddr *) &peer, sizeof(peer)) != 0) {
    perror("Unable to connect");
    return 1;
  }

  for (int i = 0; i < 8; ++i) {
    if (send(_socket, message, strlen(message), 0) <= 0) {
      perror("Unable to send");
      return 1;
    }
  }

  char buf[255];
  if (recv(_socket, buf, 255, 0) <= 0) {
    perror("Unable to recv");
  } else {
    printf("%s\n", buf);
  }

  return 0;
}
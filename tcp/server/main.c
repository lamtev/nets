#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include <nets_lib/receivenbytes.h>

typedef struct Args {
  char msg[255];
  bool *flag;
} Args;

void *listenStdinForMessage(void *args);

int main() {
  int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listeningSocket < 0) {
    perror("Socket error");
    return 1;
  }

  struct sockaddr_in localAddress;
  localAddress.sin_family = AF_INET;
  localAddress.sin_port = htons(7500);
  localAddress.sin_addr.s_addr = htonl(INADDR_ANY);

  int error = bind(listeningSocket, (struct sockaddr *) &localAddress, sizeof(localAddress));
  if (error != 0) {
    perror("Unable to bind");
    return 1;
  }

  error = listen(listeningSocket, 5);
  if (error != 0) {
    perror("Unable to listen");
    return 1;
  }

  pthread_t exitThread;
  pthread_attr_t exitThreadAttr;

  pthread_attr_init(&exitThreadAttr);

  bool exit = false;
  Args args = (Args) {"exit", &exit};

  pthread_create(&exitThread, &exitThreadAttr, listenStdinForMessage, (void *) &args);

  while (true) {
    int acceptedSocket = accept(listeningSocket, NULL, NULL);

    if (exit) {
      break;
    }

    if (acceptedSocket < 0) {
      perror("Unable to accept");
      return 1;
    }

    char buf[32];
    size_t bytesReceived = receiveNBytes(acceptedSocket, 32, buf);
    if (bytesReceived == -1) {
      perror("Unable to recv");
      return 1;
    }
    printf("received bytes = %s\n", buf);
    
    ssize_t bytesSent = send(acceptedSocket, buf, strlen(buf), 0);
    if (bytesSent == -1) {
      perror("Unable to send");
    }

//    shutdown(acceptedSocket, SHUT_RDWR);
//    close(acceptedSocket);
  }

  pthread_join(exitThread, NULL);

  shutdown(listeningSocket, SHUT_RDWR);
  close(listeningSocket);

  return 0;
}

void *listenStdinForMessage(void *args) {
  char buf[255];
  Args *_args = (Args *) args;
  
  do {
    scanf("%s", buf);
  } while (strcmp(_args->msg, buf) != 0);
  
  *_args->flag = true;
  
  pthread_exit(0);
}

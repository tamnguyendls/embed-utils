/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h> 
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 5555

void *clientThread(void * vargp)
{
    printf("Enter clientThread\n");
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char *server_addr = "127.0.0.1";
    char sendbuff[256];
    char recvbuff[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");        
        exit(EXIT_FAILURE);
    }

    // memset(&serv_addr, '0', sizeof(serv_addr));
    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_port = htons(SERVER_PORT);
    // if(inet_pton(AF_INET, server_addr, &serv_addr.sin_addr)<=0)
    // {
    //     printf("\n inet_pton error occured\n");
    //     return 1;
    // }
    
    server = gethostbyname(server_addr);
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(SERVER_PORT);

    while(1)
    {
        if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("Error : Connect Failed \n");
        }
        printf("Client is connected to server\n");
        
        printf("Client is going to sending the message: \n");
        memset(sendbuff, 0, 256); // init buffer
        fgets(sendbuff, 255, stdin); // get info from keyboard
    
        // Send data to created socket
        n = write(sockfd, sendbuff, strlen(sendbuff));
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(EXIT_FAILURE);
        }
    
        // Receive data from server.
        memset(recvbuff, 0, 256);
        n = read(sockfd, recvbuff, 255);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(EXIT_FAILURE);
        }
        
        printf("%s\n",recvbuff);
    }
    close(sockfd); // close socket
    sleep(4);
}

void *serverThread(void * vargp)
{
    printf("Enter serverThread\n");
    int sockfd;  // socket server to listen from the connection
    int newsockfd; // socket is created when server accept connection from client
    int n, len;
    char sendbuff[256]; // buffer to send data
    char recvbuff[256]; // buffer to read data
    struct sockaddr_in serv_addr, client_addr; // structure to store server/client address info
    
    // init variables
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(&client_addr, '0', sizeof(client_addr));
    memset(sendbuff, '0', 256);
    
    // init server addrress
    serv_addr.sin_family = AF_INET;             // default
    serv_addr.sin_addr.s_addr = INADDR_ANY;     // ip server
    serv_addr.sin_port = htons(SERVER_PORT);    // port number
    
    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }
    
    // bind socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        perror("ERROR on binding");
    printf("Done create and bind server socket\n");
    printf("Server is listening for connection from client\n");
    // listening
    listen(sockfd, 5); // max 5 backlogs in queue
    
    len = sizeof(client_addr); // to get info connected clients
    
    // this loop is for listening and serving to connected clients
    while(1)
    {
        printf("Server is blocking to waiting for connection at port %d\n", SERVER_PORT);
        newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t*)&len);
        if (newsockfd < 0) 
        {
            perror("ERROR on accept");
            continue;
        } 
        char s[255]; //Chua thong tin dia chi client ket noi den
        inet_ntop(client_addr.sin_family,(struct sockaddr *)&client_addr, s, sizeof s);
        printf("server: got connection from %s\n", s);

        memset(recvbuff, 0, 256);
        // read data from socket 
        printf("Server is blocking to wait for msg form clients\n\n");
        n = read(newsockfd, recvbuff, 255);    // block until data coming from clients
        if (n < 0)
            perror("ERROR reading from socket");
        
        printf("Message from client: %s\n",recvbuff);
        
        // write data via socket
        strcpy(sendbuff, "Server has got message\n");
        n = write(newsockfd,sendbuff,sizeof(sendbuff));
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(EXIT_FAILURE);
        }
        close(newsockfd);    // close client connection
        sleep(1);
    }
    
    close(sockfd);
    return 0; 
}

int main(void)
{
    pthread_t thread_id1, thread_id2; 
    int threadResult1 = 0, threadResult2 = 0;

    pthread_create(&thread_id1, NULL, serverThread, NULL); 
    pthread_create(&thread_id2, NULL, clientThread, NULL);
    pthread_join(thread_id1, (void**)&threadResult1);
    pthread_join(thread_id2, (void**)&threadResult2);

    exit(EXIT_SUCCESS);
}

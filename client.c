// udp client driver program
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define MIN(a, b) ((a)<(b)?(a):(b))
#define PORT 5000
#define MAX_RETRY_CNT 10
#define MAXLINE 1000

// Driver code
int main()
{
    char buffer[100];
    char message[100];
    int sockfd, retry_cnt;
    int wait_interval, max_wait_interval, base, multiplier, n;
    struct sockaddr_in servaddr;
    
    // clear servaddr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;
    
    // create datagram socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    // connect to server
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
    }
    
    // write the message to server
    printf("Please enter a message to server: ");
    scanf("%s", &message[0]);
    
    // Send the message to server:
    // Use exponential backoff algorithm
    retry_cnt = 0;
    max_wait_interval = 8000; //ms
    base = 500;
    multiplier = 2;
    n = 0;

    while (sendto(sockfd, &message, MAXLINE, 0, (struct sockaddr*)NULL, sizeof(servaddr)) < 0)
    {
        if (retry_cnt >= MAX_RETRY_CNT)
        {
            exit(1);
        }
        
        retry_cnt ++;
        printf("Unable to send message\n");
        
        wait_interval = MIN(max_wait_interval, base * pow(multiplier, n));
        sleep(wait_interval/1000);
        n++;
    }
    
    
    // waiting for response
    recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
    printf("Echo message: ");
    puts(buffer);

    // close the descriptor
    close(sockfd);
    
    exit(0);
    
    return 0;
}


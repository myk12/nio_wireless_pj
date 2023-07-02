#include "nio.h"

int bulk_send(int port);
int bulkly_send_data(int sock, int pid);

int main(int argc, char **argv)
{
    int p_num = 3;
    int server_port = SERVER_PORT;

    for (int i=0; i<p_num; i++)
    {
        int pid = 0;
        if ((pid = fork()) < 0)
        {
            printf("fork error");
            exit(-1);
        }else if (pid == 0)
        {
            // child
            int port = server_port + i;
            printf("PID [%d] start sending to %d\n", getpid(), port);
            bulk_send(port);
        }else
        {
            // parent
        }
    }
}

int bulk_send(int port)
{
    int ret = 0;
    int sockfd = 0;
    int pid = getpid();

    struct sockaddr_in server_addr;
    
    // 1. create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("failed to create socket");
        exit(-1);
    }

    // 2. clear memory, set add and port
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, LOCAL_TEST_SERVER_IP, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);
    
    // 3. connect to server
    ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        perror("failed to connect.");
        close(sockfd);
        exit(-2);
    }

    // 4. send data
    bulkly_send_data(sockfd, pid);

    return 0;
}


int bulkly_send_data(int sock, int pid)
{
    int ret = 0;
    char message[SEND_BUFF_LEN] = {0};
    size_t size = sizeof(message);
    
    // set message
    memset(message, 'A', size);

    while (1)
    {
        ret =  write(sock, message, size-1);
        if (ret < 0)
        {
            printf("PID [%d] Failed to send data.", pid);
            close(sock);
            exit(-3);
        }
    }

    return 0;

}

#include "nio.h"

int start_server(int port);
int statistically_read(int sock, int pid);

int
main(int argc, char **argv)
{
    int p_num = 3, listen_port = SERVER_PORT;

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
            int port = listen_port + i;
            printf("PID [%d] start listen on %d\n", getpid(), port);
            start_server(port);
        }else
        {
            // parent
        }
    }

}


int start_server(int port)
{
    int listenfd, ret, pid;
    struct sockaddr_in server_addr;

    pid = getpid();

    // 1. create socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        printf("failed to create socket.");
        exit(-1);
    }

    // 2. clear memory, set addr and port
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;   // ipv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // listen on all local IP addr
    server_addr.sin_port = htons(port);

    // 3. bind
    ret = bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret == -1)
    {
        printf("failed to bind socket.");
        close(listenfd);
        exit(-2);
    }

    ret = listen(listenfd, 16);
    if (ret == -1)
    {
        printf("failed to listen.");
        close(listenfd);
        exit(-3);
    }

    printf("PID [%d] Server listening on %d...\n", pid, port);

    for (;;)
    {
        struct sockaddr_in client_addr;
        int client_sock;
        char client_ip[64];

        socklen_t client_addr_len;
        client_addr_len = sizeof(client_addr);

        client_sock = accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock < 0)
        {
            printf("PID [%d] failed to accept\n", pid);
            close(client_sock);
            exit(-3);
        }

        printf("PID [%d] Accept client {%s, %d}\n",
                pid,
                inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip)),
                ntohs(client_addr.sin_port));

        // read data from client
        statistically_read(client_sock, pid);

    }

}

int statistically_read(int sock, int pid)
{
    double total_bytes = 0;
    time_t last_time = time(NULL);
    time_t curr_time = time(NULL);
    double elapsed_time = 0;

    while(1)
    {
        char buff[READ_BUFF_LEN]; // 1MB buffer

        int len = read(sock, buff, sizeof(buff) - 1);

        if (len <= 0)
        {
            printf("PID [%d] read error.", pid);
            close(sock);
            exit(-1);
        }

        total_bytes += len;

        curr_time = time(NULL);
        elapsed_time = difftime(curr_time, last_time);

        if (elapsed_time >= 1)
        {
            // report
            printf("PID [%d] throughput %.2f MB per %.2f sec\n", pid, total_bytes/(1024*1024), elapsed_time);

            // renew last time
            last_time = time(NULL);
            total_bytes = 0;
        }
    }

    return 0;
}
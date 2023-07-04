#include "nio.h"

int bulk_send_client(int port, int prio);
int bulk_send(int sock, int pid);
int bulk_send_and_statistic(int sock, int pid, char* log_filename);
void signal_handler(int signum);

int tos_val[3] = {8<<2, // lowest priority, CS1
                32<<2, // middle priority, CS4
                0x2E<<2};// critical, EF
char* tos_string[3] = {"low", "mid", "high"};

int main(int argc, char **argv)
{
    int p_num = 3;
    int server_port = SERVER_PORT;
    pid_t child_pid[3];

    for (int i=0; i<p_num; i++)
    {
        sleep(i+3);
        if ((child_pid[i] = fork()) < 0)
        {
            printf("fork error");
            exit(-1);
        }else if (child_pid[i] == 0)
        {
            // child
            int port = server_port + i;
            printf("PID [%d] start sending to %d\n", getpid(), port);
            bulk_send_client(port, i);
        }
    }

    // wait for three seconds
    sleep(90);

    // send signal to each child.
    for (int i=0; i<3; i++)
    {
        printf("Send signal to child process %d\n", child_pid[i]);
        kill(child_pid[3-i-1], SIGUSR1);

        //sleep 1 second
        sleep(3);
    }

    // wait for child
    for (int i=0; i<3; i++)
    {
        wait(NULL);
    }

    return 0;
}

int bulk_send_client(int port, int id)
{
    int ret = 0;
    int sockfd = 0;
    int pid = getpid();
    int prio = tos_val[id];
    char* prio_string = tos_string[id];

    struct sockaddr_in server_addr;
    
    // 1. create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("failed to create socket");
        exit(-1);
    }

	ret = setsockopt(sockfd, IPPROTO_IP, IP_TOS, &prio, sizeof(prio));
	if (ret < 0)
	{
		printf("failed to set sockopt");
		exit(-2);
	}

    // 2. clear memory, set add and port
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
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
#ifdef CLIENT_STATISTIC
    char log_filename[64] = {0};
    snprintf(log_filename, sizeof(log_filename), "log/client_pid_%d_prio_%s.dat", pid, prio_string);
    bulk_send_and_statistic(sockfd, pid, log_filename);
#else
    bulk_send(sockfd, pid);
#endif

    return 0;
}

int bulk_send(int sock, int pid)
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


int bulk_send_and_statistic(int sock, int pid, char* filename)
{
    int ret = 0;
    double gen_bytes = 0;
    double send_bytes = 0;
    char message[SEND_BUFF_LEN] = {0};
    size_t msg_size = sizeof(message);

    // time recorder
    time_t last_time = time(NULL);
    time_t curr_time = time(NULL);
    double elapsed_time = 0;
    
    // set message
    memset(message, 'A', msg_size);

    // open logfile
    FILE* file = fopen(filename, "w+");
    if (file == NULL)
    {
        printf("Failed to open log file\n");
        exit(-4);
    }

    // signal handler
    signal(SIGUSR1, signal_handler);

    while (1)
    {
        // record send bytes
        gen_bytes += msg_size;
        ret =  write(sock, message, msg_size - 1);
        if (ret < 0)
        {
            printf("PID [%d] Failed to send data.", pid);
            close(sock);
            exit(-3);
        }
        send_bytes += ret;

        // time logger
        curr_time = time(NULL);
        elapsed_time = difftime(curr_time, last_time);

        if (elapsed_time >= STATISTIC_TIME_INTERVAL_SECOND)
        {
            // report
            fprintf(file, "%.2ld %.2f %.2f\n", curr_time, gen_bytes/(1024*1024), send_bytes/(1024*1024));

            // renew last time
            last_time = time(NULL);
            gen_bytes = 0;
            send_bytes = 0;
        }

    }

    return 0;
}

void signal_handler(int signum)
{
    // process signal
    printf("Child process %d received signal %d\n", getpid(), signum);

    exit(0);
}
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9410
#define AUTH_TOKEN "OPS-2882"
#define SID "2882"
#define LOG_FILE "remoteops_IT24102882.log"

pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void write_log(const char *message)
{
    pthread_mutex_lock(&log_mutex);

    FILE *log_file = fopen(LOG_FILE, "a");

    if (log_file != NULL)
    {
        time_t now = time(NULL);
        struct tm *time_info = localtime(&now);

        char time_buffer[64];
        strftime(time_buffer, sizeof(time_buffer),
                 "%Y-%m-%d %H:%M:%S", time_info);

        fprintf(log_file, "[%s] %s\n", time_buffer, message);
        fclose(log_file);
    }

    pthread_mutex_unlock(&log_mutex);
}


void *handle_client(void *arg);
int main(void)
{
    int server_fd;
    int client_fd;
    int authenticated = 0;
    struct sockaddr_in server_addr;
     char buffer[1024];
     ssize_t bytes_received;
    FILE *uptime_file;
    double uptime_sec;
    FILE *mem_file;
    long mem_total;
    long mem_available;
    FILE *load_file;
    double cpu_load;



    printf("RemoteOps Agent starting...\n");
    printf("TCP Port: %d\n", PORT);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("socket failed");
        return 1;
    }

    printf("TCP socket created successfully.\n");
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
{
        perror("bind failed");
        close(server_fd);
        return 1;
}

     printf("Socket bound to port %d successfully.\n", PORT);
if (listen(server_fd, 10) < 0)
{
        perror("listen failed");
        close(server_fd);
        return 1;
}

    printf("Agent is listening on TCP port %d...\n", PORT);
while (1)
{
    int *client_ptr = malloc(sizeof(int));

    if (client_ptr == NULL)
    {
        perror("malloc failed");
        continue;
    }

    *client_ptr = accept(server_fd, NULL, NULL);

    if (*client_ptr < 0)
    {
        perror("accept failed");
        free(client_ptr);
        continue;
    }

    printf("Controller connected successfully.\n");

    pthread_t thread_id;

    if (pthread_create(&thread_id, NULL, handle_client, client_ptr) != 0)
    {
        perror("pthread_create failed");
        close(*client_ptr);
        free(client_ptr);
        continue;
    }

    pthread_detach(thread_id);
}

    close(server_fd); 
    return 0;
}

void *handle_client(void *arg)
{
    int client_fd = *(int *)arg;
    free(arg);
int authenticated = 0;
char buffer[1024];
ssize_t bytes_received;

FILE *uptime_file;
double uptime_sec;

FILE *mem_file;
long mem_total;
long mem_available;

FILE *load_file;
double cpu_load;
    printf("Controller thread started.\n");

while (1)
{
bytes_received = 0;

while (bytes_received < (ssize_t)sizeof(buffer) - 1)
{
    char ch;
    ssize_t n = recv(client_fd, &ch, 1, 0);

    if (n <= 0)
    {
        bytes_received = n;
        break;
    }

    buffer[bytes_received++] = ch;

    if (ch == '\n')
        break;
}
if (bytes_received <= 0) {

    printf("Controller disconnected.\n");
    break;
}

buffer[bytes_received] = '\0';

printf("Received: %s\n", buffer);
if (authenticated == 0)
{

if (strcmp(buffer, "AUTH OPS-2882\n") == 0)
{
    authenticated = 1;
    printf("Authentication successful.\n");
write_log("AUTH SUCCESS SID:2882");
    char response[] = "OK AUTHENTICATED SID:2882\n";
    send(client_fd, response, strlen(response), 0);
}
else
{
    printf("Authentication failed.\n");

    char response[] = "ERR 001 AUTH_FAILED SID:2882\n";
    send(client_fd, response, strlen(response), 0);
}
}

else
{
    if (strcmp(buffer, "SYSINFO\n") == 0)
    {
        printf("SYSINFO command received.\n");
write_log("COMMAND SYSINFO SID:2882");
        uptime_file = fopen("/proc/uptime", "r");

if (uptime_file != NULL)
{
    fscanf(uptime_file, "%lf", &uptime_sec);
    fclose(uptime_file);

    printf("System uptime: %.0f seconds\n", uptime_sec);
}
mem_file = fopen("/proc/meminfo", "r");

if (mem_file != NULL)
{
    fscanf(mem_file, "MemTotal: %ld kB\n", &mem_total);
    fscanf(mem_file, "MemFree: %*ld kB\n");
    fscanf(mem_file, "MemAvailable: %ld kB\n", &mem_available);

    fclose(mem_file);

    printf("Memory used: %ld MB\n",
           (mem_total - mem_available) / 1024);
}
load_file = fopen("/proc/loadavg", "r");

if (load_file != NULL)
{
    fscanf(load_file, "%lf", &cpu_load);
    fclose(load_file);

    printf("CPU load: %.2f\n", cpu_load);
}
char response[200];

snprintf(response, sizeof(response),
         "OK SYSINFO %.2f %ld %.0f SID:2882\n",
         cpu_load, (mem_total - mem_available) / 1024, uptime_sec);

send(client_fd, response, strlen(response), 0);    
}
else if (strcmp(buffer, "LISTPROC\n") == 0)
{
    printf("LISTPROC command received.\n");
write_log("COMMAND LISTPROC SID:2882");
    FILE *proc_file;
    char proc_buffer[2048] = "";
    char line[128];

    proc_file = popen("ps -eo pid=,comm=", "r");

    if (proc_file != NULL)
    {
        while (fgets(line, sizeof(line), proc_file) != NULL)
        {
            if (strlen(proc_buffer) + strlen(line) < sizeof(proc_buffer) - 20)
            {
                strcat(proc_buffer, line);
            }
        }

        pclose(proc_file);

        char response[2300];
        snprintf(response, sizeof(response),
                 "OK PROCS %s SID:2882\n", proc_buffer);

        send(client_fd, response, strlen(response), 0);
    }
}
else if (strncmp(buffer, "EXEC ", 5) == 0)
{
    printf("EXEC command received.\n");
write_log("COMMAND EXEC SID:2882");
    char command[50];
    char system_command[100] = "";
    char exec_output[1024] = "";
    char line[256];

    sscanf(buffer + 5, "%49s", command);

    if (strcmp(command, "DATE") == 0)
        strcpy(system_command, "date");
    else if (strcmp(command, "UPTIME") == 0)
        strcpy(system_command, "uptime");
    else if (strcmp(command, "DISKFREE") == 0)
        strcpy(system_command, "df -h /");
    else if (strcmp(command, "HOSTNAME") == 0)
        strcpy(system_command, "hostname");
    else if (strcmp(command, "WHOAMI") == 0)
        strcpy(system_command, "whoami");
    else
    {
        char response[] = "ERR 002 COMMAND_NOT_ALLOWED SID:2882\n";
        send(client_fd, response, strlen(response), 0);
        continue;
    }

    FILE *exec_file = popen(system_command, "r");

    if (exec_file != NULL)
    {
        while (fgets(line, sizeof(line), exec_file) != NULL)
        {
            if (strlen(exec_output) + strlen(line) < sizeof(exec_output) - 1)
                strcat(exec_output, line);
        }

        pclose(exec_file);

        char response[1200];
        snprintf(response, sizeof(response),
                 "OK EXEC %s SID:2882\n", exec_output);

        send(client_fd, response, strlen(response), 0);
    }
}
else if (strncmp(buffer, "PUT ", 4) == 0)
{
    char filename[100];
    long filesize;

    if (sscanf(buffer, "PUT %99s %ld", filename, &filesize) == 2)
    {
        printf("PUT command received: %s (%ld bytes)\n",
               filename, filesize);

        char filepath[256];
        snprintf(filepath, sizeof(filepath),
                 "./agentfiles/IT24102882/%s", filename);

        FILE *file = fopen(filepath, "wb");

        if (file != NULL)
        {
            long received = 0;
            char file_buffer[1024];

            while (received < filesize)
            {
                long remaining = filesize - received;
                size_t to_receive =
                    remaining < (long)sizeof(file_buffer)
                    ? (size_t)remaining
                    : sizeof(file_buffer);

                ssize_t n = recv(client_fd,
                                 file_buffer,
                                 to_receive,
                                 0);

                if (n <= 0)
                    break;

                fwrite(file_buffer, 1, n, file);
                received += n;
            }

            fclose(file);

            if (received == filesize)
            {
                char response[256];
                snprintf(response, sizeof(response),
                         "OK FILE_RECEIVED %s %ld SID:2882\n",
                         filename, filesize);

                send(client_fd, response,
                     strlen(response), 0);

                printf("File received successfully: %s\n",
                       filepath);
            }
        }
    }
}


else if (strncmp(buffer, "GET ", 4) == 0)
{
    char filename[100];

    if (sscanf(buffer, "GET %99s", filename) == 1)
    {
        printf("GET command received: %s\n", filename);

        char filepath[256];
        snprintf(filepath, sizeof(filepath),
                 "./agentfiles/IT24102882/%s", filename);

        FILE *file = fopen(filepath, "rb");

        if (file == NULL)
        {
            char response[] =
                "ERR 005 FILE_NOT_FOUND SID:2882\n";

            send(client_fd, response,
                 strlen(response), 0);
        }        else
        {
            fseek(file, 0, SEEK_END);
            long filesize = ftell(file);
            rewind(file);

            char response[256];
            snprintf(response, sizeof(response),
                     "OK FILE_SEND %s %ld SID:2882\n",
                     filename, filesize);

            send(client_fd, response,
                 strlen(response), 0);

            char file_buffer[1024];
            size_t bytes_read;

            while ((bytes_read = fread(file_buffer, 1,
                                       sizeof(file_buffer), file)) > 0)
            {
                send(client_fd, file_buffer, bytes_read, 0);
            }

            fclose(file);

            printf("File sent successfully: %s (%ld bytes)\n",
                   filepath, filesize);
        }
    }
}
else if (strncmp(buffer, "MONITOR START ", 14) == 0)
{
    int udp_port;

    if (sscanf(buffer, "MONITOR START %d", &udp_port) == 1)
    {
        printf("MONITOR START command received. UDP port: %d\n",
               udp_port);
write_log("COMMAND MONITOR START SID:2882");
        int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);

        if (udp_fd >= 0)
        {
            struct sockaddr_in udp_addr;
            memset(&udp_addr, 0, sizeof(udp_addr));

            udp_addr.sin_family = AF_INET;
            udp_addr.sin_port = htons(udp_port);
            udp_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

            char monitor_data[256];

            snprintf(monitor_data, sizeof(monitor_data),
                     "CPU_MONITOR ACTIVE SID:2882");

            sendto(udp_fd, monitor_data, strlen(monitor_data), 0,
                   (struct sockaddr *)&udp_addr,
                   sizeof(udp_addr));

            close(udp_fd);

            char response[] =
                "OK MONITOR_STARTED SID:2882\n";

            send(client_fd, response,
                 strlen(response), 0);
        }
    }
}
else if (strcmp(buffer, "MONITOR STOP\n") == 0)
{
    printf("MONITOR STOP command received.\n");
write_log("COMMAND MONITOR STOP SID:2882");
    char response[] =
        "OK MONITOR_STOPPED SID:2882\n";

    send(client_fd, response,
         strlen(response), 0);
}
else if (strncmp(buffer, "QUIT", 4) == 0)
{
    char response[] = "OK BYE SID:2882\n";
    send(client_fd, response, strlen(response), 0);

    printf("Controller requested graceful disconnect.\n");
write_log("COMMAND QUIT - GRACEFUL DISCONNECT SID:2882");
    break;
}
    else
    {
        char response[] = "ERR 003 UNKNOWN_COMMAND SID:2882\n";
        send(client_fd, response, strlen(response), 0);
    }
}
}
    close(client_fd);
    return NULL;
}

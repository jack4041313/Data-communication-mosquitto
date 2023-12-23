#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>
#include <string.h>
#include <time.h>
 
#define HOST "localhost"
#define PORT  1883
#define KEEP_ALIVE 60
#define QoS 0

bool session = true;
long int timestamp_start;
long int timestamp_end;
int latency;
struct timespec tp;

FILE *fptr;

    
void mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    
    //puts( (char *)message->payload );

    //take end timestamp after client received respone.
     if(clock_gettime(CLOCK_MONOTONIC, &tp)){
        perror("yout error message");
        exit(EXIT_FAILURE);
    }
    timestamp_end = tp.tv_sec*1000000 + tp.tv_nsec/1000;

    // timestamp_end - timestamp_start (*) This is answer
    latency = timestamp_end - timestamp_start;
    fprintf(fptr, "%d\n", latency);
    printf("%ld \n", latency);
}

void mqtt_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    int ret;
    if (!result){
        ret = mosquitto_subscribe(mosq, NULL, "B", QoS);
        if(ret < 0){
            printf("Subscription B mosquitto_loopfailed\n");
        }else{
            printf("Subscription B succeeded\n");
        }
    }else{
        printf("connect failed\n");
    }
}

int main()
{
    
    struct mosquitto *c_mosq = NULL;
    int test = 0; 
    char buff[1] = "B";
    
    // open file
    fptr = fopen("./record.txt", "w");
    if(fptr == NULL){
        printf("Open file reeor!\n");
        exit(1);
    }

    // Init
    test = mosquitto_lib_init();
    if (test < 0){
        printf("mosquitto lib int fail...");

    }

    // Create client
    c_mosq = mosquitto_new(NULL,session,NULL);
    if (c_mosq == NULL){
        printf("create client failed...\n");
    }
    
    //callback function
    mosquitto_connect_callback_set(c_mosq, mqtt_connect_callback);
    mosquitto_message_callback_set(c_mosq, mqtt_message_callback);
    
    test = mosquitto_connect(c_mosq, HOST, PORT, KEEP_ALIVE);
    if (test < 0){
        printf("connect fail");    
    }

    // Main Loop
    test = mosquitto_loop_start(c_mosq);
    if (test < 0){
        printf("mosquitto loop fail");
    }
    
    // run 600 seconds 
    long int endtime = time(NULL) + 600;
    
    while(time(NULL)<endtime){
    
        // get timestamp before public
        if(clock_gettime(CLOCK_MONOTONIC, &tp)){
            perror("yout error message");
            exit(EXIT_FAILURE);
        }
        
        timestamp_start = tp.tv_sec*1000000 + tp.tv_nsec/1000;
        
        // Publish string "B" with topic A to server
        mosquitto_publish(c_mosq, NULL, "A", 1, buff, QoS, 0);
        sleep(1);
    }
	
    // Cleanup
    fclose(fptr);
    mosquitto_loop_stop(c_mosq, true);
    mosquitto_destroy(c_mosq);
    mosquitto_lib_cleanup();

    return 0;
}

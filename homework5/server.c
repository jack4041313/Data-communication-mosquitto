#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mosquitto.h>
#include <mqtt_protocol.h>
#include "sub_client_output.h"
#include "client_shared.h"
#include "config.h"

#define HOST "localhost"
#define PORT 1883
#define KEEP_ALIVE 60
#define buffer_size 512
#define QoS 0

struct mosq_config cfg;
struct mosquitto *g_mosq = NULL;


void mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{

    struct timespec tp;
    char *new_topic;    
    char buff[buffer_size];
    long int timestamp;
    int string_len;    

    new_topic = (char *)(message->payload);
    
    // get timestamp
    if(clock_gettime(CLOCK_MONOTONIC, &tp)){
        perror("yout error message");
        exit(EXIT_FAILURE);
    }
    timestamp = tp.tv_sec*1000000 + tp.tv_nsec/1000;
    
    // merge two string and save in buff
    sprintf(buff, "Timestamp is %ld", timestamp);
    string_len = (int)strlen(buff) + 1;
    //puts(buff);
    
    mosquitto_publish(g_mosq, NULL, new_topic, string_len, buff, QoS, 0);
    memset(buff, 0, sizeof(buff));


}
 
void mqtt_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    int ret;
    if (!result){
        ret = mosquitto_subscribe(mosq, NULL, "A", QoS);
        if(ret < 0){
            printf("Subscription A failed\n");
        }else{
            printf("Subscription A succeeded\n");
        }
    }else{
        printf("connect failed\n");
    }
}


int main()
{
    
    int rc;

    rc = mosquitto_lib_init();
    if (rc < 0){
    	printf("mosquitto init fail...\n");
    }
     
    // create a client
    g_mosq = mosquitto_new(NULL, 1, NULL);
    if(g_mosq == NULL){
	printf("Create client fail...\n");
    }
   
    // Callback function
    mosquitto_connect_callback_set(g_mosq, mqtt_connect_callback);
    mosquitto_message_callback_set(g_mosq, mqtt_message_callback);
    
    // connect to broker
    rc = mosquitto_connect(g_mosq, HOST, PORT, KEEP_ALIVE);
    if (rc < 0){
        printf("connect fail");    
    }

    // Loop
    // rc = mosquitto_loop(g_mosq, -1, 1);
    rc = mosquitto_loop_forever(g_mosq, -1, 1);
    // rc = mosquitto_loop_start(g_mosq);
    if (rc < 0){
        printf("mosquitto loop fail");
    }
    
    // cleanup
    //mosquitto_disconnect(g_mosq);
    mosquitto_loop_stop(g_mosq, true);
    mosquitto_destroy(g_mosq);
    mosquitto_lib_cleanup();
    
    return 0;
}

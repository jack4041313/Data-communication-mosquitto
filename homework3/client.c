#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>
#include <string.h>
 
#define HOST "localhost"
#define PORT  1883
#define KEEP_ALIVE 60

bool session = true;

void mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
        
    puts( (char *)message->payload );

}

void mqtt_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    int ret;
    if (!result){
        ret = mosquitto_subscribe(mosq, NULL, "B", 2);
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

    int test = 0; 
    struct mosquitto *c_mosq = NULL;
    char buff[1] = "B";
    
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

    // Connect to broker
    test = mosquitto_connect(c_mosq, HOST, PORT, KEEP_ALIVE);
    if (test < 0){
        printf("connect fail");    

    }
    
    // Publish string "B" with topic A to server
    mosquitto_publish(c_mosq, NULL, "A", 1, buff, 0, 0);
    memset(buff, 0, sizeof(buff));
    
    //callback function
    mosquitto_connect_callback_set(c_mosq, mqtt_connect_callback);
    mosquitto_message_callback_set(c_mosq, mqtt_message_callback);
  
    // Loop
    test = mosquitto_loop_forever(c_mosq, -1, 1);
    if (test < 0){
        printf("mosquitto loop fail");
    }

    // Cleanup
    //mosquitto_disconnect(c_mosq);
    //mosquitto_loop_stop(c_mosq, true);
    mosquitto_destroy(c_mosq);
    mosquitto_lib_cleanup();

    return 0;
}

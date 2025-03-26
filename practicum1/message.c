/****************************************************
 * message.c
 * Implementation of functions declared in message.h
 ****************************************************/

 #include "message.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 
 Message* create_msg(int id,
                     const char* sender,
                     const char* receiver,
                     const char* content,
                     int delivered)
 {
     Message* msg = (Message*)malloc(sizeof(Message));
     if (!msg) {
         fprintf(stderr, "create_msg: Memory allocation failed.\n");
         return NULL;
     }
 
     msg->id         = id;
     msg->time_sent  = time(NULL);
     msg->delivered  = delivered;
 
     /* Safely copy strings */
     strncpy(msg->sender,   sender,   MAX_SENDER_LEN - 1);
     msg->sender[MAX_SENDER_LEN - 1] = '\0';
 
     strncpy(msg->receiver, receiver, MAX_RECEIVER_LEN - 1);
     msg->receiver[MAX_RECEIVER_LEN - 1] = '\0';
 
     strncpy(msg->content,  content,  MAX_CONTENT_LEN - 1);
     msg->content[MAX_CONTENT_LEN - 1] = '\0';
 
     return msg;
 }
 
 int store_msg(const Message* msg)
 {
     if (!msg) {
         fprintf(stderr, "store_msg: Received NULL message pointer.\n");
         return -1;
     }
 
     /* Construct filename. You should ensure "messages/" directory exists. */
     char filename[64];
     snprintf(filename, sizeof(filename), "messages/%d.msg", msg->id);
 
     FILE* fp = fopen(filename, "wb");
     if (!fp) {
         perror("store_msg: Failed to open file for writing");
         return -1;
     }
 
     /* Write entire message struct in binary */
     size_t written = fwrite(msg, sizeof(Message), 1, fp);
     fclose(fp);
 
     if (written != 1) {
         fprintf(stderr, "store_msg: Error writing file.\n");
         return -1;
     }
 
     return 0;
 }
 
 Message* retrieve_msg(int msg_id)
 {
     /* Construct filename for reading */
     char filename[64];
     snprintf(filename, sizeof(filename), "messages/%d.msg", msg_id);
 
     FILE* fp = fopen(filename, "rb");
     if (!fp) {
         /* Not found or error opening */
         return NULL;
     }
 
     Message* msg = (Message*)malloc(sizeof(Message));
     if (!msg) {
         fprintf(stderr, "retrieve_msg: Memory allocation failed.\n");
         fclose(fp);
         return NULL;
     }
 
     size_t read_count = fread(msg, sizeof(Message), 1, fp);
     fclose(fp);
 
     if (read_count != 1) {
         /* Error reading or incomplete data */
         free(msg);
         return NULL;
     }
 
     return msg;
 }
 
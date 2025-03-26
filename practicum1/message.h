/****************************************************
 * message.h
 * Header file for the Message structure and
 * related function prototypes.
 ****************************************************/

 #ifndef MESSAGE_H
 #define MESSAGE_H
 
 #include <time.h>
 
 /* Constants for string lengths (adjust as needed) */
 #define MAX_SENDER_LEN   32
 #define MAX_RECEIVER_LEN 32
 #define MAX_CONTENT_LEN  256
 
 /* Message structure */
 typedef struct {
     int     id;                          /* Unique identifier */
     time_t  time_sent;                   /* Time the message was 'sent' */
     char    sender[MAX_SENDER_LEN];      /* Sender (placeholder text) */
     char    receiver[MAX_RECEIVER_LEN];  /* Receiver (placeholder text) */
     char    content[MAX_CONTENT_LEN];    /* Message content (placeholder) */
     int     delivered;                   /* 0 or 1 indicating delivery status */
 } Message;
 
 /**
  * Creates a new Message object on the heap.
  * @param id unique integer ID
  * @param sender text for sender
  * @param receiver text for receiver
  * @param content message content
  * @param delivered 0 or 1
  * @return pointer to newly allocated Message object, or NULL on failure
  */
 Message* create_msg(int id,
                     const char* sender,
                     const char* receiver,
                     const char* content,
                     int delivered);
 
 /**
  * Stores a message on disk. By default, writes to "messages/<id>.msg" in binary.
  * @param msg pointer to Message
  * @return 0 on success, non-zero on error
  */
 int store_msg(const Message* msg);
 
 /**
  * Retrieves a message from disk by ID. Reads from "messages/<id>.msg" in binary.
  * @param msg_id the message ID
  * @return pointer to newly allocated Message, or NULL on error/not found
  */
 Message* retrieve_msg(int msg_id);
 
 #endif /* MESSAGE_H */
 
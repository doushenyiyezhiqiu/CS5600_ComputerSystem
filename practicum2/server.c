/* --------------------------------------------------------------------
 *  server.c  –  multi‑threaded remote‑file‑system server
 *               now with per‑file flock() to protect concurrent access
 * ------------------------------------------------------------------ */
 #include "server.h"

 #include <sys/file.h>   /* flock()  */
 #include <fcntl.h>      /* open()   */
 #include <sys/stat.h>   /* mkdir()  */
 
 /* ----------  global permission table (unchanged)  ----------------- */
 #define MAX_FILES 100
 
 typedef struct {
     char filename[256];
     permission_t perm;          /* READ_ONLY or READ_WRITE */
 } filemeta_t;
 
 static filemeta_t      g_files[MAX_FILES];
 static int             g_file_count = 0;
 static pthread_mutex_t g_files_lock = PTHREAD_MUTEX_INITIALIZER;
 
 /* ----------  helpers for permission table  ------------------------ */
 static permission_t get_file_permission(const char *path)
 {
     pthread_mutex_lock(&g_files_lock);
     for (int i = 0; i < g_file_count; ++i)
         if (strcmp(g_files[i].filename, path) == 0) {
             permission_t p = g_files[i].perm;
             pthread_mutex_unlock(&g_files_lock);
             return p;
         }
     pthread_mutex_unlock(&g_files_lock);
     return READ_WRITE;          /* default for unknown files */
 }
 
 static void set_file_permission(const char *path, permission_t p)
 {
     pthread_mutex_lock(&g_files_lock);
     /* if already present, leave it unchanged */
     for (int i = 0; i < g_file_count; ++i)
         if (strcmp(g_files[i].filename, path) == 0) {
             pthread_mutex_unlock(&g_files_lock);
             return;
         }
     if (g_file_count < MAX_FILES) {
         strncpy(g_files[g_file_count].filename, path,
                 sizeof(g_files[g_file_count].filename) - 1);
         g_files[g_file_count].filename[255] = '\0';
         g_files[g_file_count].perm = p;
         ++g_file_count;
     }
     pthread_mutex_unlock(&g_files_lock);
 }
 
 static permission_t parse_perm(const char *s)
 {
     if (!s)                        return READ_WRITE;
     if (strcasecmp(s, "RO") == 0)  return READ_ONLY;
     return READ_WRITE;
 }
 
 /* ====================================================================
  *  WRITE  -------------------------------------------------------------
  * ===================================================================*/
 static void handle_write(int csock,
                          char *localArg,
                          char *remotePath,
                          char *permStr)
 {
     printf("[Server] WRITE: remote='%s' perm='%s'\n",
            remotePath, permStr ? permStr : "(default RW)");
 
     /* permission logic */
     permission_t perm = get_file_permission(remotePath);
     if (perm == READ_WRITE) {                    /* first time? */
         set_file_permission(remotePath, parse_perm(permStr));
         perm = get_file_permission(remotePath);
     }
     if (perm == READ_ONLY) {
         send(csock, "ERR_FILE_IS_READ_ONLY", 22, 0);
         printf("[Server]  -> rejected (read‑only)\n");
         return;
     }
 
     /* tell client to send data */
     send(csock, "OK_READY_TO_RECEIVE", 20, 0);
 
     char full[BUF_SIZE];
     snprintf(full, sizeof(full), "%s/%s", SERVER_DATA_DIR, remotePath);
 
     /* open + exclusive lock */
     int fd = open(full, O_WRONLY | O_CREAT | O_TRUNC, 0666);
     if (fd < 0) { perror("open"); send(csock,"ERR_OPEN",8,0); return; }
     if (flock(fd, LOCK_EX) < 0) { perror("flock(EX)"); close(fd);
         send(csock,"ERR_FLOCK_FAILED",16,0); return; }
 
     /* receive exactly one chunk (demo) */
     char buf[BUF_SIZE] = {0};
     int  n = recv(csock, buf, sizeof(buf), 0);
     if (n <= 0) {
         flock(fd, LOCK_UN);
         close(fd);
         send(csock,"ERR_RECV_FILE_DATA",18,0);
         return;
     }
     if (write(fd, buf, n) != n) {
         perror("write");
         flock(fd, LOCK_UN); close(fd);
         send(csock,"ERR_WRITE_FAILED",16,0);
         return;
     }
 
     flock(fd, LOCK_UN);
     close(fd);
     send(csock, "WRITE_OK", 8, 0);
     printf("[Server]  -> wrote %d bytes to '%s'\n", n, full);
 }
 
 /* ====================================================================
  *  GET  ---------------------------------------------------------------
  * ===================================================================*/
 static void handle_get(int csock,
                        char *remotePath,
                        char *localArg)
 {
     printf("[Server] GET: remote='%s'\n", remotePath);
 
     char full[BUF_SIZE];
     snprintf(full, sizeof(full), "%s/%s", SERVER_DATA_DIR, remotePath);
 
     int fd = open(full, O_RDONLY);
     if (fd < 0) {
         send(csock,"ERR_FILE_NOT_FOUND",18,0);
         printf("[Server]  -> not found\n");
         return;
     }
     if (flock(fd, LOCK_SH) < 0) { perror("flock(SH)"); close(fd);
         send(csock,"ERR_FLOCK_FAILED",16,0); return; }
 
     char buf[BUF_SIZE] = {0};
     int  n = read(fd, buf, sizeof(buf));
     flock(fd, LOCK_UN);
     close(fd);
 
     send(csock,"OK_SENDING_FILE",15,0);
     if (n > 0) send(csock, buf, n, 0);
     printf("[Server]  -> sent %d bytes\n", n);
 }
 
 /* ====================================================================
  *  RM  ----------------------------------------------------------------
  * ===================================================================*/
 static void handle_rm(int csock, char *remotePath)
 {
     printf("[Server] RM: remote='%s'\n", remotePath);
 
     if (get_file_permission(remotePath) == READ_ONLY) {
         send(csock,"ERR_FILE_IS_READ_ONLY",22,0);
         printf("[Server]  -> rejected (read‑only)\n");
         return;
     }
 
     char full[BUF_SIZE];
     snprintf(full, sizeof(full), "%s/%s", SERVER_DATA_DIR, remotePath);
 
     int fd = open(full, O_WRONLY);          /* open just for lock */
     if (fd < 0) { send(csock,"ERR_REMOVE_FAILED",17,0);
                   printf("[Server]  -> file not present\n"); return; }
     if (flock(fd, LOCK_EX) < 0) { perror("flock(EX)"); close(fd);
         send(csock,"ERR_FLOCK_FAILED",16,0); return; }
 
     int rc = remove(full);
     flock(fd, LOCK_UN);
     close(fd);
 
     if (rc == 0) {
         send(csock,"RM_OK",5,0);
         printf("[Server]  -> removed\n");
     } else {
         send(csock,"ERR_REMOVE_FAILED",17,0);
         perror("remove");
     }
 }
 
 /* ====================================================================
  *  Per‑client thread
  * ===================================================================*/
 typedef struct { int s; struct sockaddr_in a; } client_t;
 
 static void *client_thread(void *arg)
 {
     client_t *c = (client_t*)arg;
     char ip[INET_ADDRSTRLEN];
     inet_ntop(AF_INET,&c->a.sin_addr,ip,sizeof(ip));
     unsigned short port = ntohs(c->a.sin_port);
     printf("[Server] Client %s:%u connected\n", ip, port);
 
     char buf[BUF_SIZE] = {0};
     int  n = recv(c->s, buf, sizeof(buf)-1, 0);
     if (n <= 0) { close(c->s); free(c); return NULL; }
     buf[n] = '\0';
     printf("[Server]  cmd='%s'\n", buf);
 
     char *cmd  = strtok(buf," \t\r\n");
     char *arg1 = strtok(NULL," \t\r\n");
     char *arg2 = strtok(NULL," \t\r\n");
     char *arg3 = strtok(NULL," \t\r\n");
 
     if (!cmd) { close(c->s); free(c); return NULL; }
 
     if (strcasecmp(cmd,"WRITE")==0 && arg1 && arg2)
         handle_write(c->s,arg1,arg2,arg3);
     else if (strcasecmp(cmd,"GET")==0 && arg1 && arg2)
         handle_get(c->s,arg1,arg2);
     else if (strcasecmp(cmd,"RM")==0 && arg1)
         handle_rm(c->s,arg1);
     else
         send(c->s,"ERR_BAD_ARGS",12,0);
 
     close(c->s);
     printf("[Server] Client %s:%u disconnected\n", ip, port);
     free(c);
     return NULL;
 }
 
 /* ====================================================================
  *  main
  * ===================================================================*/
 int main(void)
 {
     mkdir(SERVER_DATA_DIR,0777);
 
     int lsock = socket(AF_INET,SOCK_STREAM,0);
     if (lsock<0){perror("socket");return 1;}
     int opt=1; setsockopt(lsock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
 
     struct sockaddr_in addr={0};
     addr.sin_family=AF_INET; addr.sin_port=htons(PORT);
     addr.sin_addr.s_addr=INADDR_ANY;
     if (bind(lsock,(struct sockaddr*)&addr,sizeof(addr))<0){
         perror("bind"); return 1;}
     if (listen(lsock,5)<0){perror("listen");return 1;}
     printf("[Server] Listening on port %d …\n", PORT);
 
     while (1) {
         client_t *c = malloc(sizeof(client_t));
         socklen_t len = sizeof(c->a);
         c->s = accept(lsock,(struct sockaddr*)&c->a,&len);
         if (c->s < 0) { perror("accept"); free(c); continue; }
         pthread_t tid; pthread_create(&tid,NULL,client_thread,c);
         pthread_detach(tid);
     }
     return 0;
 }
 
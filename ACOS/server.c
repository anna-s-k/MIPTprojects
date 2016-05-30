#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

char* make_html(char* path, char* host, char* file);
int get_rqst(int newsockfd, char* rqst){
	puts("getting"); 	
	bzero(rqst, 3000);
	if(recv(newsockfd, rqst, 3000, 0) < 0){
		perror("");
		exit(1);
	}
	//puts(rqst);
	return 0;	
}
//in rast dir
void send_file(int newsockfd, char* dir, char* rqst){
	puts("in");
	char path[256];
	char dir2[256];
	char* file;
	char* host;
	puts(rqst);
	strtok(rqst, " ");
	perror("1");
	file = strtok(NULL, " \r\n");
	strtok(NULL, " ");
	host = strtok(NULL, " \r\n");
	puts(host);
	perror("2");
	/*if (strcmp(file, "/favicon.ico") == 0){
		send(newsockfd, "HTTP/1.1 200 OK\n\n", 17, 0);
		send(newsockfd, "a", 1, 0);
		return;
	}*/
	strcpy(path, dir);
	strcpy(dir2, dir);
	perror("3");
	//strncat(path, "/", 1);
	strncat(path, file, 256);
	strncat(
	perror("3");
	puts("path: ");
	puts(path);
	
	int fd;
	//if not dir
	
	if (opendir(path) == NULL){
		puts("file");
		fd = open(path, O_RDONLY);
		if (fd < 0){
			printf("No such file or dir\n");
			exit(1);
		}
		else{
			send(newsockfd, "HTTP/1.1 200 OK\n\n", 17, 0);
			int bytes_read;
			char buf[1024];
			while ( (bytes_read=read(fd, buf, 1024))>0 )
				write (newsockfd, buf, bytes_read);	
		}
	}
	//if dir exists!!!!
	else{
		
		
		strncat(path, "index.html", 256);
		puts("psht2:");
		puts(path);
		fd = open(path, O_RDONLY);
		if (fd  < 0){
			send(newsockfd, "HTTP/1.1 200 OK\n\n", 17, 0);
			char* index = make_html(dir2, host, file);
			//char* index = "<html><head></head><body><a href=http://127.0.0.1:8082/wc.c>wc.c</a><br>find.c</a></body></html>";
			write (newsockfd, index, strlen(index));	
		}
		
	}
	shutdown (newsockfd, SHUT_RDWR);
	close(newsockfd);
	
}

char* make_html(char* path, char* host, char* file){
	char html[3000] = "<html><body>";
	bzero(html, 3000);
	puts("html:");
	printf("dsadad\n");
	puts(path);
	char href[100];
	strcpy(href, "<a href=http://");
	strcat(href, host);
	strcat(href, path);
	strcat(href, file);
	struct dirent *dp;
	DIR* dirp = opendir(path);
	while((dp = readdir(dirp)) != NULL){
		strcat(html,href);
		strcat(html,dp->d_name);
		strcat(html, ">");
		strcat(html, path);
		strcat(html, dp->d_name);
		strcat(html, "</a><br>");
	}
	strcat(html, "</body></html>");
	puts(html);
	return html;
}



int main(int argc, char* argv[]){ 
	puts("azaza");
	if (argc < 2){
		printf("No port");
		exit(1);
	}
	
	int port = atoi(argv[1]);
	char* dir = (argc < 3) ? "." : argv[2];
	
	
	struct sockaddr_in serv_addr, clnt_addr;
	
	//socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		perror("Socket");
		exit(1);
	}
	
	//connection socket with local 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);//to network byte order
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (const struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ){
		perror("Bind");
		exit(1);
		//exit(1);
	}
	
	//listen
	if(listen(sockfd, 3) < 0){
		perror("Listen");
		exit(1);
	}
	int i = 0;
	int client[10];
	while(1){
		//accept get next client
		int clnt_addr_len = sizeof(clnt_addr);
		client[i] = accept(sockfd, (struct sockaddr *) &clnt_addr, (socklen_t *) &clnt_addr_len);
		
		perror("Accept: ");
		if (client[i] < 0){
			perror("Accept");
			exit(1);
		}
		char rqst[3000];
		bzero(rqst, 3000);
		printf("waiting for rqst %d\n", i);
		
		int son = fork();
		if (son == 0){
			get_rqst(client[i], rqst);
			
			send_file(client[i], dir, rqst);
			
			exit(0);
		}
		wait(0);
		//client[i] = -1;	
		i++;
	}
	while(1);
	shutdown (sockfd, SHUT_RDWR);
	close(sockfd);
	
		
	return 0;
}

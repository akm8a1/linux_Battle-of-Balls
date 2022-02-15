#include "common.h"
#include "struct.h"
int connect_server(char* address,char* port){
	int tcp_socket;
	struct sockaddr_in addr;
	if ((tcp_socket=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("cannot connect");
		exit(1);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = inet_addr(address);
	if(connect(tcp_socket, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in))==-1){
		perror("cannot connect");
		exit(1);
	}
	return tcp_socket;
}
int sendPackage(int fd,struct Package* package){
	int size = sizeof(struct Package);
	char* buffer = (char*)malloc(size);
	memcpy(buffer,package,size);
	int pos = 0 ;
	int len = 0 ;
	while (pos<size){
		len = send(fd,buffer+pos,1024,0);
		if (len<=0) {
			perror("ERROR");
			break;
		}
		pos+=len;
	}
	free(buffer);	
	return 0;
}
struct Package* receivePackage(int fd,struct Package* package) {
//	struct Package* package = (struct Package*)malloc(sizeof(struct Package));
	int size = sizeof(struct Package);
	char* buffer = (char*)malloc(size);
	int pos = 0 ;
	int len;
	while (pos<size) {
		len = recv(fd,buffer+pos,1024,0);
		if (len<0) {
			perror("cannot receive data");
			exit(1);
		}
		pos+=len;
	}
	memcpy(package,buffer,size);
	free(buffer);
	return package;
}

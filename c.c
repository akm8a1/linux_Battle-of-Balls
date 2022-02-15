#include "talk.h"
#include <ncurses.h>
#include <locale.h>
#define MAP_SIZE 32
void printMap(char map[MAP_SIZE][MAP_SIZE]);
void updateMap(char source[32][32],char des[32][32]);
int main(int ac, char *av[]){
	int tcp_socket;
	int client_id; //这个客户端的id
	int client_status;
	int game_status;
	char map[32][32];
	char title[256];
	tcp_socket = connect_server(av[1],av[2]);
	if (tcp_socket == -1) {
		perror("未能连接服务器\n");
		exit(1);
	}
	setlocale(LC_ALL, ""); 
	initscr();
	crmode();
	noecho();
	clear();
	printf("已成功连接至服务器\n");
	struct Package* initial_package = (struct Package*)malloc(sizeof(struct Package));
	receivePackage(tcp_socket,initial_package);
	client_id = initial_package->client_id;
	client_status = initial_package->client_status;
	game_status = initial_package->game_status;
	strcpy(title,initial_package->title);
	updateMap(initial_package->map,map);
	free(initial_package);
	while (game_status) {
		printMap(map);
	//	refresh();
		//char c = getch();
		if (client_status) {
			struct Package* sp = (struct Package*)malloc(sizeof(struct Package));
			char c = getch();
			if (c=='q') {
				strcpy(sp->title,"quit game");
				sp->client_id = client_id; 
				sp->client_status = 0;
				sendPackage(tcp_socket,sp);
			} else if (c=='w') {
				sp->client_id = client_id;
				sp->client_status = 1;
				strcpy(sp->title,"up");
				sendPackage(tcp_socket,sp);
			} else if (c=='a') {
				sp->client_id = client_id;
				sp->client_status = 1;
				strcpy(sp->title,"left");
				sendPackage(tcp_socket,sp);
			} else if (c=='d') {
				sp->client_id = client_id;
				sp->client_status = 1;
				strcpy(sp->title,"right");
				sendPackage(tcp_socket,sp);
			} else if (c=='x') {
				sp->client_id = client_id;
				sp->client_status = 1;
				strcpy(sp->title,"down");
				sendPackage(tcp_socket,sp);
			} else {
				continue;
			}
			free(sp);
		} else {
			struct Package* sp = (struct Package*)malloc(sizeof(struct Package));
			sp->client_id = client_id;
			sp->client_status = client_status;
			strcpy(sp->title,"watch");
			sendPackage(tcp_socket,sp);
			free(sp);
			move(0,0);
			printw("您已进入观战");
		//	sleep(1);
		}
		struct Package* rp = (struct Package*)malloc(sizeof(struct Package));
		receivePackage(tcp_socket,rp);		
		client_status = rp->client_status;
		game_status = rp->game_status;
		strcpy(title,rp->title);
		refresh();
		updateMap(rp->map,map);
		free(rp);
	}
	close(tcp_socket);
	if(client_status==1) {
		move(0,0);
		printw("您已赢得本局游戏,按任意键退出\n");
	} else {
		move(0,0);
                printw("您输了,按任意键退出\n");
	}
	getch();
	endwin();
	return 0;
}
void printMap(char map[MAP_SIZE][MAP_SIZE]){
	for (int i=0;i<MAP_SIZE;i++){
		for (int j=0;j<MAP_SIZE;j++) {
			move(i,j);
			addch(map[i][j]);
		}
	}
}
void updateMap(char source[32][32],char des[32][32]){
	for (int i=0;i<32;i++) {
		for (int j=0;j<32;j++) {
			des[i][j] = source[i][j];			
		}
	}

}

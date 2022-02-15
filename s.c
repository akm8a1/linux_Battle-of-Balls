#include "talk.h"
#include "utils.c"
int main(int ac, char *av[]){
	int tcp_socket;
	struct sockaddr_in addr;
	//共享内存
	int shmid = shmget(99,sizeof(struct Game),0777|IPC_CREAT);
	void* pos = NULL;
	struct Game* game = NULL;
	if (-1==shmid) {
		perror("无法获取共享内存");
		exit(1);
	}
	pos = shmat(shmid,NULL,0);
	game = (struct Game*)pos;
	//信号量
	sem_t *mutex;
	if ((mutex=sem_open("game",O_CREAT,0644,1))==SEM_FAILED){
		perror("unable to create semaphore");
		sem_unlink("game");
		exit(1);
	}
	//初始化游戏
	game->status = 1; 
	game->size = 0 ;
	initializeMap(game->map);//初始化地图
	tcp_socket  =  socket(AF_INET,  SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(av[1]));
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(tcp_socket, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in))==-1){
		perror("cannot bind");
		exit(1);
	}   
	listen(tcp_socket, 1);
	int game_status=1; 
	while(game_status){
		int fd=accept(tcp_socket, NULL, NULL);	
		int rv_fork=fork();
		if(0==rv_fork) {
			//具体的这个玩家的进程
			//把这个玩家的信息告诉他
			//	添加一个玩家
		//	int game_status = 1;
			int client_status = 1;
			struct Rect rect;
			rect.id = game->size;
			//初始化这个玩家
			initializeRectLocation(&rect,game->map,game);
			game->rects[game->size] = rect;
			game->size++; //玩家数+1
			struct Package* p = (struct Package*)malloc(sizeof(struct Package));
			p->client_id = rect.id; 
			p->client_status = 1; 
			p->game_status = 1; 
			strcpy(p->title,"initial_client");
			refreshMap(game);
			for (int i=0;i<32;i++) {
				for (int j=0;j<32;j++) {
					p->map[i][j] = game->map[i][j];
				}
			}
			sendPackage(fd,p);
			free(p);
			while (game_status) {
				//只要这个游戏还没有结束,就一直和客户端交互
				struct Package* package = (struct Package*)malloc(sizeof(struct Package));
				receivePackage(fd,package);
				printf("%s\n",package->title);
				int client_id = package->client_id; 	
				client_status = package->client_status;
				sem_wait(mutex);	
				if (strcmp(package->title,"quit game")==0){
					blankMap(game);
					quitGame(package->client_id,game);
					client_status = 0;
				} else if (strcmp(package->title,"up")==0){
					blankMap(game);
					up(&game->rects[package->client_id],1);
				} else if (strcmp(package->title,"down")==0){
					blankMap(game);
					down(&game->rects[package->client_id],1);
				} else if (strcmp(package->title,"left")==0){
					blankMap(game);
					left(&game->rects[package->client_id],1);
				} else if (strcmp(package->title,"right")==0){
					blankMap(game);
					right(&game->rects[package->client_id],1);
				}
				free(package);
				package = (struct Package*)malloc(sizeof(struct Package));
				/*现在所有的行动都完成了，如果玩家吃到了奖励点，需要去覆盖，完结吃了别的玩家也需要去覆盖*/
			//	printf(" %c \n",game->map[22][7]);
				getPoints(&game->rects[client_id],game->map); 
				client_status = eat(game,client_id);
				refreshMap(game);
				sem_post(mutex);
				strcpy(package->title,"response");
				for (int i=0;i<32;i++) {
					for (int j=0;j<32;j++) {
						package->map[i][j] = game->map[i][j];
					}
				}
				printf(" %c \n",package->map[22][7]);
				if (game->size<=1) {
					game_status = 0;
				}
				package->client_status = client_status;
				package->game_status = game_status;
				sendPackage(fd,package);
				free(package);
			}
			//游戏结束
			close(fd);
			shmctl(shmid,IPC_RMID,0);
			exit(0);
		}else{
			close(fd);  
		}
	}
	return 0;
}

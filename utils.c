#include <stdio.h>
#include <stdlib.h>
#include "struct.h"
#define BOTTOM 32
#define RIGHT 32

int refreshMap(struct Game* game){
	for (int i=0;i<game->size;i++){
		struct Rect rect = game->rects[i];
		int startX = rect.startX;
		int startY = rect.startY;
		int length = rect.length;
		int i;
		int j;
		if (rect.status==0) {
			continue;			
		}
		else {
			for (i=startX;i<startX+length;i++) {
				game->map[i][startY] = '*';
			}
			i--;
			for (j=startY;j<startY+length;j++) {
				game->map[i][j] = '*';
			}
			j--;
			for (;i>startX;i--) {
				game->map[i][j] = '*';
			}
			for (;j>startY;j--) {
				game->map[i][j] = '*';
			}
		}
	} 
	return 0; 
}

void blankMap(struct Game* game){

	for(int i=0;i<game->size;i++) {
		struct Rect rect = game->rects[i];
		int startX = rect.startX;
		int startY = rect.startY;
		int length = rect.length;
		int i;
		int j;
		for (i=startX;i<startX+length;i++) {
			game->map[i][startY] = ' ';
		}
		i--;
		for (j=startY;j<startY+length;j++) {
			game->map[i][j] = ' ';
		}
		j--;
		for (;i>startX;i--) {
			game->map[i][j] = ' ';
		}
		for (;j>startY;j--) {
			game->map[i][j] = ' ';
		}
	} 
}
void up (struct Rect* rect,int distance) {
	if (rect->startX-distance>0) {
		rect->startX = rect->startX-distance;	
	} else {
		rect->startX = 1 ; 	
	}
}
void down(struct Rect* rect,int distance) {
	int length = rect->length;
	if (rect->startX+distance+length<=BOTTOM) {
		rect->startX = rect->startX+distance;
	}else {
		rect->startX = BOTTOM-distance-length+1;
	}
} 
void left(struct Rect* rect,int distance){
	if (rect->startY-distance>0) {
		rect->startY = rect->startY-distance;
	} else {
		rect->startY = 1 ;
	}
}
void right(struct Rect* rect,int distance){
	int length = rect->length;
	if (rect->startY+distance+length<=RIGHT) {
		rect->startY = rect->startY+distance;
	} else {
		rect->startY = RIGHT-distance-length+1;
	}
}
void quitGame(int client_id,struct Game* game){
	struct Rect rect = game->rects[client_id]; 
	int startX = rect.startX;
	int startY = rect.startY;
	int length = rect.length;
	int i;
	int j;
	for (i=startX;i<startX+length;i++) {
		game->map[i][startY] = ' ';
	}
	i--;
	for (j=startY;j<startY+length;j++) {
		game->map[i][j] = ' ';
	}
	j--;
	for (;i>startX;i--) {
		game->map[i][j] = ' ';
	}
	for (;j>startY;j--) {
		game->map[i][j] = ' ';
	}
	printf("%d %d\n",i,j);
	game->rects[client_id].status = 0;
	game->size--;
}
//在地图中放置奖励
void initializeMap(char map[BOTTOM][RIGHT]){
	int m;
	int n;	
	for (m=0;m<BOTTOM;m++) {
		map[m][0] = '|';
	}
	m--;
	for (n=0;n<RIGHT;n++) {
		map[m][n] = '-';
	}
	n--;
	for (;m>0;m--) {
		map[m][n] = '|';
	}
	for (;n>=0;n--) {
		map[m][n] = '-';
	}	
	for (int i=1;i<BOTTOM-1;i++) {
		for (int j=1;j<RIGHT-1;j++) {
			int randomInt = rand()%100;
			if (randomInt<1) {
				map[i][j] = '$';
			} else {
				map[i][j] = ' ';
			}
		}
	}		
}
//吃掉范围内的所有建立点，并获得奖励

void getPoints(struct Rect* rect,char map[BOTTOM][RIGHT]){
	if (rect->status==0) return;
	int startX = rect->startX;
	int startY = rect->startY;
	int length = rect->length;
	for (int i=startX;i<startX+length;i++) {
		for (int j=startY;j<startY+length;j++) {
			if (map[i][j] == '$') {
				rect->length++;
				map[i][j] = ' ';			
			}
		}
	}
}
//与其他玩家的交互
int eat(struct Game* game,int client_id){
	struct Rect client = game->rects[client_id];
	if (client.status==0) return 0;
	for (int i=0;i<game->size;i++) {
		struct Rect rect = game->rects[i];
		if (rect.id==client_id || rect.status==0) {
			continue;
		} else {
			int up = rect.startX;
			int down = rect.startX+rect.length;
			int left = rect.startY;
			int right = rect.startY+rect.length;
			for (int m=client.startX;m<client.startX+client.length;m++) {
				for (int n=client.startY;n<client.startY+client.length;n++) {
					if ((m>=up)&&(m<=down)&&(n>=left)&&(n<=right)) {
						//有了接触
						if (client.length>=rect.length){
							printf("big\n");
							game->rects[client_id].length = client.length+rect.length;
							game->rects[i].status = 0;
							game->size--;
							return 1;
						}else{
							quitGame(client_id,game);
							printf("little\n");
							game->size--;
							return 0;
						}
					}
				}
			}
		}
	}
	return 1;
}

//初始化矩形的位置

void initializeRectLocation(struct Rect* rect,char map[BOTTOM][RIGHT],struct Game* game){
	/*首先初始点不能与奖励点重合，其次不能和其他玩家的初始点重合*/
	int flag = 1;
	while (flag) {
		int x = rand()%(BOTTOM-2)+1;
		int y = rand()%(RIGHT-2)+1;
		if (game->size==0) {
			if (map[x][y]!='$') {
				rect->status = 1;
				rect->startX = x;
				rect->startY = y;
				rect->length = 1;
				break;
			}

		}
		for (int i=0;i<game->size;i++) {
			if ((x!=game->rects[i].startX || y!=game->rects[i].startY)&&map[x][y]!='$') {
				rect->status = 1;
				rect->startX = x;
				rect->startY = y;
				rect->length = 1;
				flag = 0;
				break;
			}
		}                
	}
} 

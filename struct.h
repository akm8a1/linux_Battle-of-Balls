#ifndef struct_h
#define struct_h
struct Rect{
	int id; //玩家的ID
	int status; //玩家的状态 1存活  0死亡
	int startX; //左上角行坐标
	int startY; //左上角列坐标
	int length; //正方形的边长
};
struct Game{
	int status; //游戏的状态 0游戏还未开始 1游戏开始 2游戏结束
	int size; //玩家数目(当前元素个数)
	struct Rect rects[100]; //所有的玩家
	char map[32][32]; //游戏地图
};
struct Request{
	int id;
	char content[1024];
};
struct Package{
	int client_id; //客户端id
	int game_status; //游戏状态
	int client_status; //这个玩家的状态
	char title[256]; //消息的描述
	char map[32][32]; //游戏地图
};
#endif

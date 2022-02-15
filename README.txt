1.文件结构
	c.c 为客户端代码
	s.c 为服务器端代码
	common.h中引入了所使用到的头文件
	struct.h中是自定义的数据结构体
	talk.h中定义了有关于网络编程的工具函数，包括连接建立、收发消息等
	utils.c中定义的是有关于对游戏数据进行操作的工具函数，例如初始化地图、输赢判定等
2.编译命令
客户端编译命令
cc c.c -o c -std=c99 -lncursesw -D_XOPEN_SOURCE=600
服务器端编译命令
cc s.c -o s -std=c99 -lpthread -D_XOPEN_SOURCE=600
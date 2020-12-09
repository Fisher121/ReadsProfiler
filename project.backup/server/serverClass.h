#pragma once
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <sys/wait.h>
#include "sqlite3.h"
#include <csignal>
#include "../structs.h"


static void handler(int sig)
{
        pid_t pid;
        while(1)
        {
                pid=waitpid(-1,NULL, WNOHANG);
                if(pid == 0)
                        return;
                else
                        if(pid == -1)
                                return;
        }
}

class Server{
private:
	int PORT;
	struct sockaddr_in server;
	struct sockaddr_in from;
	int sd,pid,res;
	sqlite3* db;
	char msg[100];
	char rasp[1024];
	int client;
public:
	Server(){ PORT = 2024;}
	~Server();
	int init();
	int serveClients();
	void loop();
	bool login(char* username, char* password);
	template<typename T>int Write(int client, T msg);
	char* Read(int client);
	int signIn(char* username, char* password, char *email);
	int sendBook(char* bookID);
	void getBooks(std::vector<book> &bookVector,search src);
	template<typename T>int writeVector(int client, std::vector<T> Vector,int size);
};

#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <stdarg.h>
#include <map>
#include "../structs.h"
#include <iostream>
#include <vector>
#include <string>

extern errno;

class client{
	private:
	
		int sd;
		struct sockaddr_in server;
		char username[20];
		char *ipv4;
		int port;
		bool state = true;
	public:
		client(){port = 2024;ipv4 = "127.0.0.1";}
		~client();
		int Connect();
		int Write(char* msg);
		template<typename T>int Read(T &message);
		void login(char* username, char* password);
		int init();
		void loop();
		void getCmd();
		void Close();
		void makeCmd(char *cmd,int sz, ...);
		void signIn(char* username, char* password, char* email);
		void getBookDetails(long int bookID, book *Book);
		void getBook(char* bookID,std::vector<str> &Vector);
		void searchBooks(search crit);
		template<typename T>int readVector(std::vector<T> &Vector);

};




































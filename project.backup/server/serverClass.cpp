#include "serverClass.h"
extern int errno;

int Server::init()
{
bzero(rasp,1024);
res = sqlite3_open("projectDatabase.db", &db);
if(res != SQLITE_OK)
{
      perror("[server]Eroare la deschidere baza de date.\n");
      printf("%s\n",sqlite3_errmsg(db));
      sqlite3_close(db);
      return errno;
}


	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
{
        perror("[server]Eroare la socket().\n");
        return errno;
}

bzero(&server, sizeof(server));
bzero(&from, sizeof(from));

server.sin_family = AF_INET;
server.sin_addr.s_addr = htonl(INADDR_ANY);
server.sin_port = htons(PORT);

if(bind(sd,( struct sockaddr*)&server,sizeof(struct sockaddr)) == -1)
{
        perror("[server]Eroare la bind.\n");
        return errno;
}

if(listen(sd,5) == -1)
{
        perror("[server]Eroare la listen.\n");
        return errno;
}
return 1;
}

int Server::serveClients()
{
        socklen_t length = sizeof(from);
        int size;

	signal(SIGCHLD,handler);
	printf("[server]Asteptam la portul %d...\n",PORT);
        fflush(stdout);

	client = accept(sd, (struct sockaddr*) &from, &length);
        if(client < 0 )
        {
                perror ("[server]Eroare la accept.\n");
		close(client);
		return errno;
        }


        if((pid = fork()) < 0)
        {
                perror("[server]Eroare la fork().\n");
                close(client);
        }

	if(pid == 0)
        {
		strcpy(msg,Read(client));
		if(msg!=NULL)
                	printf("[server]Mesajul a fost receptionat: %s\n",msg);
		else
		{
			close(client);
			exit(1);
		}
		switch (msg[0]) {
			case '1':
				{
				char* pch;
				char username[20],password[20];	
				int raspuns;
				pch = strtok(msg,". ");
				pch = strtok(NULL,". ");
				strcpy(username,pch);
				pch = strtok(NULL,". ");
				strcpy(password,pch);
				if(login(username,password))
					raspuns = 1;
				else
					raspuns = 0;
				printf("[server]Trimitem mesajul inapoi...%d\n",raspuns);
               			this->Write<int>(client,raspuns);
				}
				break;
			case '2':
				{
				char *pch;
				char username[20],password[20],email[320];
				int raspuns;
				pch = strtok(msg,". ");
				pch = strtok(NULL,". ");
				strcpy(username,pch);
				pch = strtok(NULL,". ");
				strcpy(password,pch);
				pch = strtok(NULL,". ");
				strcpy(email,pch);
				if(signIn(username,password,email))
				{
					raspuns = 1;
				}
				else
				{
					raspuns = 0;
				}
				printf("[server]Trimitem mesajul inapoi...%d\n",raspuns);
        		        this->Write<int>(client,raspuns);
	
				}
				break;
			case '3':
				{
				char* pch;
				search src;
				std::vector<book> bookVector;
				pch = strtok(msg,". ");
                                pch = strtok(NULL,". ");
				strcpy(src.titlu,pch);
				pch = strtok(NULL,". ");
                                strcpy(src.autor,pch);
				pch = strtok(NULL,". ");
                                strcpy(src.gen,pch);
				pch = strtok(NULL,". ");
                                strcpy(src.an,pch);
				pch = strtok(NULL,". ");
                                strcpy(src.ISBN,pch);
				pch = strtok(NULL,". ");
                                strcpy(src.rating,pch);
				this->getBooks(bookVector,src);
				this->writeVector(client,bookVector,sizeof(book));
				printf("Am trimis structura!\n");
				}
				break;
			case '4':
				this->sendBook("1");
					break;

			default:
				strcpy(rasp,"unknown_command");
				this->Write<char[1024]>(client,rasp);
				break;

		}
		close(client);
                exit(1);
        }
        else
        {
                close(client);
		return 1;
        }
}

void Server::loop()
{
	int cond = 1;
	while(cond == 1)
	{
		cond = this->serveClients();
	}
}

bool Server::login(char* username, char* password)
{
	/*
	char* sqlAsk = "SELECT * FROM users WHERE username ='";
	strcat(sqlAsk,username);
	strcat(sqlAsk,"' and pasword='");
	strcat(sqlAsk,password);
	strcat(sqlAsk,"';");
	*/
	return true;
}
int Server::signIn(char* username, char* password, char*  email)
{
	return 1;
}
template<typename T>int Server::Write(int client, T msg)
{
	printf("Writing...\n");
	int size = sizeof(T);
	if(write(client,&size, sizeof(int)) <= 0)
        {
             	perror("[server]Eroare la write() catre client(1).\n");
		return errno;
        }
	if(write(client, &msg,size) <= 0)
	{
            	perror("[server]Eroare la write() catre client(2).\n");
		return errno;
        }
	return true;
}

char* Server::Read(int client)
{
	int index;
	char* message;
	if(read(client,&index,sizeof(int)) <0)
	{
		perror("[server]Eroare la read() de la client(1).\n");
		return NULL;
	}
	if(read(client,message,index) < 0)
	{
		perror("[server]Eroare la read() de la client(2).\n");
		return NULL;
	}
	return message;

}

void Server::getBooks(std::vector<book> &bookVector,search src)
{
	book Book;
	//here we search for the books according to src
	strcpy(Book.gen,"drama-comedie");
	strcpy(Book.subgen,"actiune-SF-romance");
	Book.IDAutor = 111;
	strcpy(Book.titlu,"Info UAIC");
	Book.an = 2020;
	Book.ISBN = 1234567890111;
	Book.rating = 2.5;
	Book.ID = 21;
	bookVector.push_back(Book);
}
template<typename T>int Server::writeVector(int client,std::vector<T> Vector,int size)
{
	int sz = Vector.size();
        if(write(client,&sz, sizeof(int)) <= 0)
        {
                perror("[server]Eroare la write() vector catre client(1).\n");
                return errno;
        }
	if(write(client,&size, sizeof(int)) <= 0)
        {
                perror("[server]Eroare la write() vector catre client(2).\n");
                return errno;
        }
        for(int i = 0;i<=Vector.size()-1;i++)
	{
        	if(write(client,&Vector[i],size) <= 0)
        	{
                	perror("[server]Eroare la write() vector catre client(2).\n");
                	return errno;
       		 }
	}
        return 1;

}

int Server::sendBook(char* bookID)
{
	int fd,cpy = 0;
	char path[20];
	str buffer;
	std::vector<str> toSend;
	bzero(path,20);
	strcpy(path,"books/");
	strcat(path,bookID);
	if((fd = open(path, O_RDONLY)) <0)
	{
		printf("[server]Eroare la deschidere fisier!\n");
			return errno;
	}
	bzero(buffer.buffer,4096);
	int x = 1;
	while(read(fd,buffer.buffer,sizeof(buffer)-1) > 0)
	{
		toSend.push_back(buffer);
		bzero(buffer.buffer,4096);
	}
	this->writeVector<str>(client,toSend,sizeof(str));
	close(fd);
	return 1;
}






















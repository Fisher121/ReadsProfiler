#include "client.h"

int client::init()
{
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ipv4);
	server.sin_port = htons(port);
	return 1;
}

int client::Connect()
{
	if((sd=socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
        	perror("[client]Eroare la socket.\n");
       		 return errno;
	}

	if(connect(sd, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1)
	{
        	perror("[client]Eroare la connect().\n");
                return errno;
	}
	else
		return 1;
}

int client::Write(char* msg)
{
	int size = strlen(msg)+1;
	this->Connect();
	if(write(sd,&size,sizeof(int)) <= 0)
	{
       	 	perror("[client]Eroare la write() spre server(1).\n");
		this->Close();
       		return errno;
	}
		if(write(sd,msg,size) <= 0)
        {
		this->Close();
                perror("[client]Eroare la write() spre server(2).\n");
                return errno;
        }
	return 1;
}

template<typename T>int client::Read(T &message)
{
	int index;
	if(read(sd,&index,sizeof(index)) < 0)
	{
		this->Close();
       		perror("[client]Error on read() from server(1).\n");
        	return errno;
	}
	if(read(sd,&message,index) < 0)	
	{
		this->Close();
        	perror("[client]Error on read() from server(2).\n");
        	return errno;
	}
	this->Close();
	return 1;
}
void client::getCmd()
{
	int cmd;
	char username[20],password[20],email[320];
	printf("1.login\n");
	printf("2.sign in\n");
	printf("3.Cautati cartea dupa nume\n");
	printf("4.Cititi carte\n");
	printf("9.Quit\n");
	scanf("%d",&cmd);
	switch (cmd){
		case 1:
			printf("username: ");
			scanf("%s",username);
			printf("password: ");
			scanf("%s",password);
			this->login(username,password);
			break;
		case 2:
                        printf("username: ");
                        scanf("%s",username);
                        printf("password: ");
                        scanf("%s",password);
			printf("email: ");
			scanf("%s",email);
                        this->signIn(username,password,email);
			break;
		case 3:
			{
			search crit;
			bzero(crit.an,5);
			bzero(crit.ISBN,14);
			bzero(crit.rating,3);
			bzero(crit.autor,200);
			bzero(crit.titlu,200);
			bzero(crit.gen,200);
			printf("Introduceti 0 daca doriti sa sariti vre-un pas!\n");
			printf("Nume carte: ");
			std::cin>>crit.titlu;
			printf("Autor : ");
			std::cin>>crit.autor;
			printf("Genuri(separate prin '-')");
			std::cin>>crit.gen;
			printf("ISBN: ");
			std::cin>>crit.ISBN;
			printf("an aparitie: ");
			std::cin>>crit.an;
			printf("rating: ");
			std::cin>>crit.rating;
			this->searchBooks(crit);
			break;
			}
		case 4:
			{
			std::vector<str> Vector;
			char bookID[10];
			printf("bookID:");
			std::cin>>bookID;
			this->getBook(bookID,Vector);
			for(int i=0;i<=Vector.size()-1;i++)
			{
				printf("%s",Vector[i].buffer);
			}
			}
			break;
			
		case 9:
			state = false;
			break;

	}
}

void client::loop()
{
	while(state)
	{
		this->getCmd();
	}
}

void client::getBook(char* bookID, std::vector<str> &Vector)
{
	char cmd[50];
	this->makeCmd(cmd,2,'4',bookID);
	this->Write(cmd);
	this->readVector<str>(Vector);
	std::cout<<"am ajuns aici";
}
void client::login(char *username, char *password)
{
	char toSend[50];
	int msg;
	this->makeCmd(toSend,3,'1',username,password);
	this->Write(toSend);
	this->Read<int>(msg);

	if(msg == 1)
	{
		printf("V-ati logat cu succes!\n");
		strcpy(this->username,username);
	}
	else
	{
		printf("Username sau parola necunoscuta!\n");
	}
}

void client::signIn(char* username,char* password, char* email)
{
	char toSend[370];
	int msg;
	this->makeCmd(toSend,4,'2',username,password,email);

	this->Write(toSend);
	this->Read<int>(msg);

	if(msg == 1)
	{
		printf("Cont creat cu succes!\n");
	}
	else
	{
		printf("Contul nu a putut fi creat!\n");
	}

}
void client::Close()
{
	close(sd);
}

void client::makeCmd(char* cmd, int sz, ...)
{
	char c[100];
	va_list args;
	va_start(args,sz);
	bzero(c,100);
	c[0] = va_arg(args,int);
	c[1] = '.';
	for(int i = 2 ;i<=sz;i++)
	{
		strcat(c,va_arg(args,char*));
		c[strlen(c)] = '.';
	}
	va_end(args);
	strcpy(cmd,c);

}

template<typename T>int client::readVector(std::vector<T> &Vector)
{
	int size;
	int sz;
        if(read(sd,&size, sizeof(int)) <= 0)
        {
                perror("[server]Eroare la read() vector de la client(1).\n");
                return errno;
        }
	if(read(sd,&sz, sizeof(int)) <= 0)
        {
                perror("[server]Eroare la read() vector  de la client(2).\n");
                return errno;
        }
	std::cout<<"aici\n";
	T temp;
        for(int i = 0;i<=size-1;i++)
                if(read(sd,&temp,sz) <= 0)
                {
                        perror("[server]Eroare la read() vector de la client(3).\n");
                        return errno;
                 }
		else
		{
			Vector.push_back(temp);
		}
	this->Close();
	printf("am citit vectorul!\n");
        return 1;

}

void client::searchBooks(search crit)
{
	char cmd[200];
	this->makeCmd(cmd,7,'3', crit.titlu,crit.autor,crit.gen,crit.an,crit.ISBN,crit.rating);
 	this->Write(cmd);
       	std::vector<book> rasp;
       	this->readVector<book>(rasp);
	std::cout<<"Cartile gasite sunt: \n";
	for(auto c : rasp)
		std::cout<<c.titlu<<std::endl;
      	

}

//void client::getBookDetails(long int bookID, book *Book)
//{
	
//}	

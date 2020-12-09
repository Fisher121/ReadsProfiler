struct book
{
	char gen[200];
	char subgen[200];
	int IDAutor;
	char titlu[200];
	int an;
	long int ISBN;
	float rating;
	int ID;
};
struct autor
{
	int ID;
	int gen[20];
	int subgen[20];
	char nume[200];
};
struct search
{
	char gen[200];
	char autor[200];
	char titlu[200];
	char an[5];
	char ISBN[14];
	char rating[4];

};
struct str
{
	char buffer[4096];
};

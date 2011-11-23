#pragma warning(disable: 4996)

#include <winsock2.h>

class CHttp
{
public:
	CHttp();
	~CHttp();

private:
	WSADATA wsaData;
	SOCKET sock;
	int bInit;
	char *hostname;

public:
	int connect(char * trget);
	void close();
	int get(char *trget, char *filename);
	int IsInit(){return bInit;};
};

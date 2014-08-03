#include "stdafx.h"
#include "rssgp.h"
#include "http.h"

#include <sys/stat.h>

CHttp::CHttp()
{
	int err;

	bInit = 0;

	err = WSAStartup(MAKEWORD(2,0), &wsaData);
	if(err != 0){
		switch (err) {
		case WSASYSNOTREADY:
			printf("WSASYSNOTREADY\n");
			break;
		case WSAVERNOTSUPPORTED:
			printf("WSAVERNOTSUPPORTED\n");
			break;
		case WSAEINPROGRESS:
			printf("WSAEINPROGRESS\n");
			break;
		case WSAEPROCLIM:
			printf("WSAEPROCLIM\n");
			break;
		case WSAEFAULT:
			printf("WSAEFAULT\n");
			break;
		}
		WSACleanup();
		return;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET){
		printf("error : %d\n", WSAGetLastError());
		return;
	}

	bInit = -1;
}

CHttp::~CHttp()
{
	WSACleanup();
}

int CHttp::connect(char * trget)
{
	struct hostent *host;
	unsigned int **addrptr;
	struct sockaddr_in server;

	hostname = trget;
	host = gethostbyname(hostname);
	if (host == NULL) {
		if (WSAGetLastError() == WSAHOST_NOT_FOUND) {
			printf("host not found : %s\n", trget);
		}
		return 1;
	}

	addrptr = (unsigned int **)host->h_addr_list;

	server.sin_family = AF_INET;
	server.sin_port = htons(80);
	server.sin_addr.S_un.S_addr = *(*addrptr);

	return ::connect(sock, (struct sockaddr *)&server, sizeof(server));
}

void CHttp::close()
{
	if(!this->IsInit())
		return;

	closesocket(sock);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET){
		printf("error : %d\n", WSAGetLastError());
		return;
	}
}

int CHttp::get(char *trget, char *filename)
{
	char req[256] = "";
	char buf[65536];
	char tmp[256] = {0};
	int n, i, j, bHead = 1, wsp = 0, sc = 0;
	FILE *fp;

	if(!this->IsInit())
		return 1;
	if(trget == NULL || filename == NULL)
		return 2;
	if(strlen(trget) == 0 || strlen(filename) == 0)
		return 3;

	strcpy(req, "GET ");
	strcat(req, trget);
	strcat(req, " HTTP/1.0\r\n");
	//strcat(req, "User-Agent: rssgp(win32)\r\n");
	strcat(req, "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n");
	strcat(req, "Host: ");strcat(req, hostname);strcat(req, "\r\n");
	strcat(req, "\r\n");

	n = send(sock, req, (int)strlen(req), 0);
	if(n < 0){
		printf("send : %d\n", WSAGetLastError());
		return 1;
	}

	memset(buf, 0, sizeof(buf));
	strcpy(buf, filename);
	for (int i = strlen(buf); i >= 0; i--) {
		if (buf[i] == '\\') {
			break;
		}
		buf[i] = '\0';
	}

	struct _stat st = {0};
	_stat(buf, &st);
	if ((st.st_mode & _S_IFDIR) == 0) {
		CreateDirectory(buf, NULL);
	}

	memset(buf, 0, sizeof(buf));
	fp = fopen(filename, "wb");

	while(n > 0){
		n = recv(sock, buf, sizeof(buf), 0);
		if(n < 0){
			printf("recv : %d\n", WSAGetLastError());
			return 1;
		}
		// ���X�|���X�̏���
		switch(bHead)
		{
		case 1:
			// HTTP-Version
			for(i = 0;i < n;i++){
				if(buf[i] != ' '){
					tmp[i] = buf[i];
				}
				else
				{
					tmp[i] = '\0';
					i++;
					bHead++;
					break;
				}
			}
		case 2:
			// Status-Code
			for(j = 0;i < n;i++, j++){
				if(buf[i] != ' '){
					tmp[j] = buf[i];
				}
				else
				{
					tmp[j] = '\0';
					i++;
					sc = strtol(tmp, NULL, 10);
					bHead++;
					break;
				}
			}
		case 3:
			// Reason-Phrase
			for(i = 0;i < n;i++){
				if(tmp[0] == '\r' && tmp[1] == '\n'
					&& tmp[2] == '\r' && buf[i] == '\n'){
					wsp = i + 1;
					bHead = 0;
					break;
				}
				memcpy(tmp, &tmp[1], 2);
				tmp[2] = buf[i];
			}
			break;
		default:
			wsp = 0;
			break;
		}
		// �{�f�B���̕ۑ�
		fwrite(&buf[wsp], 1, n - wsp, fp);
	}
	fclose(fp);

	return 0;
}

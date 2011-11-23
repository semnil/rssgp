#pragma warning(disable: 4996)

class CDocList
{
	typedef struct _ITEMLIST{
		char	title[128];	// �^�C�g��
		char	text[256];	// �e�L�X�g
		char	url[128];	// �����NURL
	}ITEMLIST;

	typedef struct _DOCLIST{
		char		urltxt[512];	// �h�L�������g��URL
		struct{
			char	pcol[32];		// �v���g�R��
			char	server[128];	// �T�[�o
			char	path[224];		// �p�X
		}url;
		char		filepath[256];	// �h�L�������g��ۑ�����ۂ̃t�@�C���p�X
		ITEMLIST	*items;			// ���݂̃A�C�e�����X�g�ւ̃|�C���^
	}DOCLIST;

public:
	CDocList();
	CDocList(int n);
	~CDocList();
	char *geturl(int n);
	char *getserver(int n);
	char *getpath(int n);
	char *getfpath(int n);
	int add(char *url, char *filepath);
	int del(int n);

private:
	int nItem;
	struct DOCLIST *dlist;
};

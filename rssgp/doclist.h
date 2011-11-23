#pragma warning(disable: 4996)

class CDocList
{
	typedef struct _ITEMLIST{
		char	title[128];	// タイトル
		char	text[256];	// テキスト
		char	url[128];	// リンクURL
	}ITEMLIST;

	typedef struct _DOCLIST{
		char		urltxt[512];	// ドキュメントのURL
		struct{
			char	pcol[32];		// プロトコル
			char	server[128];	// サーバ
			char	path[224];		// パス
		}url;
		char		filepath[256];	// ドキュメントを保存する際のファイルパス
		ITEMLIST	*items;			// 現在のアイテムリストへのポインタ
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

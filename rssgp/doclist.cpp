#include "stdafx.h"
#include "rssgp.h"
#include "doclist.h"

CDocList::CDocList()
{
	nItem = 128;
	dlist = (DOCLIST *)calloc(sizeof(DOCLIST) * nItem, 1);
}

CDocList::CDocList(int n)
{
	nItem = n;
	dlist = (DOCLIST *)calloc(sizeof(DOCLIST) * nItem, 1);
}

CDocList::~CDocList()
{
	int i;

	for(i = 0;i < nItem;i++){
		if(dlist[i].items != NULL){
			free(dlist[i].items);
		}
	}

	free(dlist);
}

char *CDocList::geturl(int n)
{
	if(n <= nItem)
		return dlist[n-1].urltxt;
	else
		return NULL;
}

char *CDocList::getserver(int n)
{
	if(n <= nItem)
		return dlist[n-1].url.server;
	else
		return NULL;
}

char *CDocList::getpath(int n)
{
	if(n <= nItem)
		return dlist[n-1].url.path;
	else
		return NULL;
}

char *CDocList::getfpath(int n)
{
	if(n <= nItem)
		return dlist[n-1].filepath;
	else
		return NULL;
}

int CDocList::add(char *url, char *filepath)
{
	int i, j, n;

	// あきを探す
	for(i = 0;i < nItem;i++){
		if(strcmp(dlist[i].filepath, "") == 0){
			n = i;
			break;
		}
	}
	if(i == nItem)
		return -1;

	// URLをコピー
	strcpy(dlist[n].urltxt, url);

	// プロトコル
	for(i = 0, j = 0;j < (int)strlen(url);i++, j++){
		if(url[i] == ':'){
			break;
		}
		dlist[n].url.pcol[i] = url[i];
	}
	j += 3;
	// サーバ
	for(i = 0;j < (int)strlen(url);i++, j++){
		if(url[j] == '/')
			break;
		dlist[n].url.server[i] = url[j];
	}
	// パス
	for(i = 0;j < (int)strlen(url);i++, j++){
		dlist[n].url.path[i] = url[j];
	}

	// ファイルパス
	for(i = 0;i < (int)strlen(filepath);i++){
		dlist[n].filepath[i] = filepath[i];
	}

	return 0;
}

int CDocList::del(int n)
{
	if(n <= nItem)
		return -1;

	if(dlist[n-1].items != NULL)
		free(dlist[n-1].items);

	memset(&dlist[n-1], 0, sizeof(DOCLIST));

	return 0;
}

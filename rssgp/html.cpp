#include "stdafx.h"
#include "rssgp.h"
#include "html.h"

extern HWND hMainWnd;

struct _spconvt *Html::spconvt = NULL;
int Html::spconv_max = 0;

Html::Html()
{
}

Html::Html(char *str)
{
	FILE *fp;
	int i;
	char buf[1024];
	char str1[15], str2[4], str3[14];

	if((fp = fopen(str, "rt")) == NULL)
		return;

	// 行数を数える
	for(i = 0;;i++){
		if(fgets(buf, sizeof(buf), fp) == NULL)
			break;
		if(buf[0] == '#' || buf[0] == '\0')
			i--;
	}
	fseek(fp, 0, SEEK_SET);

	// 設定記憶エリアを確保
	spconv_max = i;
	spconvt = (struct _spconvt*)calloc(i, sizeof(struct _spconvt));

	// 設定読み込み
	for(i = 0;;i++){
		if(fgets(buf, sizeof(buf), fp) != NULL){
			if(buf[0] == '#' || buf[0] == '\0'){
				i--;
				continue;
			}
			str1[0] = str2[0] = str3[0] = '\0';
			sscanf(buf, "%s%s%s", str1, str2, str3);
			if(strlen(str1) < sizeof(spconvt[i].name))
				strcpy(spconvt[i].name, str1);
			if(strtoul(str2, NULL, 0) <= 255)
				spconvt[i].glen = (unsigned char)strtol(str2, NULL, 0);
			if(strlen(str3) < sizeof(spconvt[i].pdata))
				strcpy(spconvt[i].pdata, str3);
			if(spconvt[i].pdata[0] == '\0'){
				spconvt[i].pdata[0] = ' ';
				spconvt[i].pdata[1] = '\0';
			}
		}else{
			break;
		}
	}

	fclose(fp);
}

Html::~Html()
{
	if(spconvt != NULL)
		free(spconvt);
}

void Html::decode(char* filepath)
{
	char str[256], buf, tmp1 = 0, tmp2[16] = {0}, wcstr[2];
	int i, rtc;
	FILE *nfp, *ofp;

	// 旧ファイルの退避
	strcpy(str, filepath);
	strcat(str, ".bef");
	rtc = MoveFileEx(filepath, str, MOVEFILE_REPLACE_EXISTING);

	if((nfp = fopen(filepath, "wb")) != 0){
		if((ofp = fopen(str, "rb")) != 0){
			while(fread(&buf, 1, 1, ofp) > 0){
				if(tmp1 == '&'){
					memset(tmp2, 0, 6);
					for(i = 0;buf != ';';i++){
						tmp2[i] = buf;
						fread(&buf, 1, 1, ofp);
						if(i + 1 >= sizeof(tmp2) - 1){
							tmp2[sizeof(tmp2)-1] = 0;
							fprintf(nfp, "&%s", tmp2);
							fputc(buf, nfp);
							tmp2[0] = 0;
							break;
						}
					}
					if(tmp2[0] == '#'){
						if(tmp2[1] != 'x'){		// 16進数のコード
							memcpy(tmp2, &tmp2[1], sizeof(tmp2) - 1);
						}else{					// 10進数のコード
							tmp2[0] = '0';
						}
						*((WORD*)wcstr) = (WORD)strtol(tmp2, NULL, 0);
						memset(tmp2, 0, 6);
						WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wcstr, 1, (LPSTR)tmp2, 3, NULL, NULL);
						fwrite(tmp2, 1, strlen(tmp2), nfp);
					}else if(strlen(tmp2) > 0){
						// 救済処理
						for(i = 0;i < (int)strlen(tmp2);i++){
							if(tmp2[i] == '&'){
								fwrite(tmp2, 1, i, nfp);
								memcpy(tmp2, &tmp2[i+1], strlen(tmp2) - i);
								i = -1;
							}
						}
						// 特殊文字の変換処理
						for(i = 0;i < spconv_max;i++){
							if(spconvt[i].glen == 0){
								if(strcmp(tmp2, spconvt[i].name) == 0){
									fputs(spconvt[i].pdata, nfp);
									break;
								}
							}else{
								if(strncmp(tmp2, spconvt[i].name, spconvt[i].glen) == 0){
									fputs(spconvt[i].pdata, nfp);
									break;
								}
							}
						}
						if(i == spconv_max){
							fprintf(nfp, "&%s;", tmp2);
							MessageBox(hMainWnd, tmp2, "rssgp", MB_OK);
						}
					}
				}else if(buf != '&'){
					fwrite(&buf, 1, 1, nfp);
				}
				tmp1 = buf;
			}
			fclose(ofp);
		}
		fclose(nfp);
	}

	DeleteFile(str);
}

// rssgp.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "rssgp.h"
#include "http.h"
#include "doclist.h"
#include "html.h"

#define MAX_LOADSTRING 100

// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��
CHttp *http;
Html *html;
CDocList *pDocList;								// �h�L�������g���X�g
HWND hMainWnd;

struct _geturls{
	char	url[256];
	char	fpath[256];
};
struct _geturls *geturls = NULL;
int get_url_max;

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void GetDocument(int i);
void CmpDocument(int i);
void CnvCharCode(char *filepath);
void ReadSettings(char *str);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �����ɃR�[�h��}�����Ă��������B
	MSG msg;
	HACCEL hAccelTable;

	// �O���[�o������������������Ă��܂��B
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_RSSGP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// �A�v���P�[�V�����̏����������s���܂�:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RSSGP));

	// ���C�� ���b�Z�[�W ���[�v:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
//  �R�����g:
//
//    ���̊֐�����юg�����́A'RegisterClassEx' �֐����ǉ����ꂽ
//    Windows 95 ���O�� Win32 �V�X�e���ƌ݊�������ꍇ�ɂ̂ݕK�v�ł��B
//    �A�v���P�[�V�������A�֘A�t����ꂽ
//    �������`���̏������A�C�R�����擾�ł���悤�ɂ���ɂ́A
//    ���̊֐����Ăяo���Ă��������B
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RSSGP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_RSSGP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 150/*CW_USEDEFAULT*/, 80/*0*/, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   hMainWnd = hWnd;

   return TRUE;
}

//
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:  ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- ���~���b�Z�[�W��\�����Ė߂�
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent, i;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		html = new Html("rssgp.scv");
		ReadSettings("rssgp.stg");
		if(get_url_max <= 0){
			PostMessage(hWnd, WM_QUIT, NULL, NULL);
			break;
		}
		http = new CHttp();
		pDocList = new CDocList();
		for(i = 0;i < get_url_max;i++){
			pDocList->add(geturls[i].url, geturls[i].fpath);
		}
		for(i = 1;i <= get_url_max;i++)CmpDocument(i);
		SetTimer(hWnd, IDT_DOC_REQEST, 5 * 60000, NULL);
		break;
	case WM_TIMER:
		KillTimer(hWnd, IDT_DOC_REQEST);
		for(i = 1;i <= get_url_max;i++)CmpDocument(i);
		SetTimer(hWnd, IDT_DOC_REQEST, 5 * 60000, NULL);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_RENEW:
			for(i = 1;i <= get_url_max;i++)CmpDocument(i);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �`��R�[�h�������ɒǉ����Ă�������...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		if(geturls != NULL)
			free(geturls);
		delete pDocList;
		delete http;
		delete html;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���ł��B
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void GetDocument(int i)
{
	if(http->connect(pDocList->getserver(i)) == 0){
		http->get(pDocList->getpath(i), pDocList->getfpath(i));
		http->close();
	}
}

void CmpDocument(int i)
{
	char str[256], rtc, tmp1, tmp2;
	FILE *nfp, *ofp;

	// ���t�@�C���̑ޔ�
	strcpy(str, pDocList->getfpath(i));
	strcat(str, ".old");
	rtc = MoveFileEx(pDocList->getfpath(i), str, MOVEFILE_REPLACE_EXISTING);
	// �V�t�@�C���̎擾
	GetDocument(i);
	html->decode(pDocList->getfpath(i));
	if(!rtc)	// �V�K
		return;

	// �t�@�C���̔�r
	if((nfp = fopen(pDocList->getfpath(i), "rb")) != 0){
		if((ofp = fopen(str, "rb")) != 0){
			str[0] = 0;
			while(fread(&tmp1, 1, 1, nfp) > 0){
				if(fread(&tmp2, 1, 1, ofp) <= 0)
					break;
				if(memcmp(&tmp1, &tmp2, 1) != 0){
					if(tmp1 >= '0' && tmp1 <= '9'
						&& tmp2 >= '0' && tmp2 <= '9')
						continue;
					strcat(str, pDocList->geturl(i));
					strcat(str, "\n�ɕύX������܂����B");
					break;
				}
			}
			fclose(ofp);
		}
		fclose(nfp);
	}

	if(strlen(str) > 0){
		MessageBox(hMainWnd, str, "rssgp", MB_OK);
	}

	// ���t�@�C���̍폜
	strcpy(str, pDocList->getfpath(i));
	strcat(str, ".old");
	//DeleteFile(str);
}

void ReadSettings(char *str)
{
	FILE *fp;
	unsigned int i;
	char buf[1024];
	char str1[512], str2[512];

	if((fp = fopen(str, "rt")) == NULL)
		return;

	// �s���𐔂���
	for(i = 0;;i++){
		if(fgets(buf, sizeof(buf), fp) == NULL)
			break;
		if(buf[0] == '#' || buf[0] == '\0')
			i--;
	}
	fseek(fp, 0, SEEK_SET);

	// �ݒ�L���G���A���m��
	get_url_max = i;
	geturls = (struct _geturls*)calloc(i, sizeof(struct _geturls));

	// �ݒ�ǂݍ���
	for(i = 0;;i++){
		if(fgets(buf, sizeof(buf), fp) != NULL){
			if(buf[0] == '#' || buf[0] == '\0'){
				i--;
				continue;
			}
			sscanf(buf, "%s%s", str1, str2);
			if(strlen(str1) < sizeof(geturls[i].url))
				strcpy(geturls[i].url, str1);
			if(strlen(str2) < sizeof(geturls[i].fpath))
				strcpy(geturls[i].fpath, str2);
		}else{
			break;
		}
	}

	fclose(fp);
}

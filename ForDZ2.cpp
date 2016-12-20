#include "stdafx.h"
#include "ForDZ2.h"

#define MAX_LOADSTRING 100


HINSTANCE hInst;								
TCHAR szTitle[MAX_LOADSTRING];					
TCHAR szWindowClass[MAX_LOADSTRING];			

												
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	MyDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	
	MSG msg;
	HACCEL hAccelTable;

	
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FORDZ2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FORDZ2));

	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FORDZ2));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_FORDZ2);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; 
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
unsigned int F_Move(unsigned int Block, bool flag_move)	
{
	unsigned int S_Left, S_Right;
	if (flag_move)
	{
		S_Left = Block << 3; 
		S_Right = (Block >> (8 * 4 - 3));
	}
	else
	{
		S_Right = (Block >> 3);
		S_Left = Block << (8 * 4 - 3);
	}
	return (S_Left | S_Right);
}

unsigned int F_Gamma(unsigned int Block) 
{
	int Gamma = (rand() << (8 * 2)) | ((rand() << (8 * 2)) >> (8 * 2)); 
	return (Block ^ Gamma);
}

void crypt(char original[512], int key, char result[512], int strsize, bool flag_crypt) 
{
	int const blocksize = 4;
	int SizeLastBlock, copy_blocksize;
	unsigned int Block1, Block2;
	int blocks;

	blocks = strsize / static_cast<float>(blocksize);
	SizeLastBlock = strsize - blocks * blocksize;
	if (SizeLastBlock != 0)	blocks++;
	srand(key);
	for (int i = 0; i < blocks; i++)
	{
		if ((i != blocks - 1) || (SizeLastBlock = 0))
			copy_blocksize = blocksize;          
		else
			copy_blocksize = SizeLastBlock;
		memcpy(&Block1, original + (blocksize * i), copy_blocksize);
		if (flag_crypt)
		{
			Block2 = F_Gamma(Block1);
			Block1 = F_Move(Block2, false);
		}
		else
		{
			Block2 = F_Move(Block1, true);
			Block1 = F_Gamma(Block2);
		}
		memcpy(result + (blocksize * i), &Block1, copy_blocksize);
	}
}
char Str[512];
int len = 0;
int key; 

OPENFILENAME ofn; 
TCHAR NameFile[256] = "MyFile.shf"; 							
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CHAR: 
				  
		Str[len] = wParam;
		len++; 
		Str[len] = 0; 
		hdc = GetDC(hWnd); 
		SetTextColor(hdc, RGB(255, 0, 0));
		TextOut(hdc, 40, 40, Str, len); 
		ReleaseDC(hWnd, hdc); 
		break;
	case WM_COMMAND: 
		wmId = LOWORD(wParam); 
		wmEvent = HIWORD(wParam);		
		switch (wmId) 
		{
		case IDM_ABOUT: 
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT: 
			DestroyWindow(hWnd);
			break;
		case ID_32771: 
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = NameFile; 
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = 255;
			ofn.lpstrFilter = "Шифрованные файлы (*.shf)\0*.shf\0Все файлы\0*.*\0";
			ofn.nFilterIndex = 0;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			ofn.lpstrDefExt = "shf"; 

			if (GetOpenFileName(&ofn))
			{		
				if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, MyDialog) == IDOK)
				{
					FILE *pF1;
					char Shifr[512]; 
					pF1 = fopen(NameFile, "rb"); 
					if (pF1 == 0)
					{
						MessageBox(hWnd, "Ошибка открытия файла. Возможно файл не существует", "Сообщение об ошибке", MB_OK | MB_ICONHAND);
						return TRUE;
					}
					fread(&len, sizeof(int), 1, pF1); 
					fread(Shifr, sizeof(char), len, pF1); 
					fclose(pF1);
					crypt(Shifr, key, Str, len, false);
					Str[len] = 0;
					InvalidateRect(hWnd, 0, TRUE);
				}
				else MessageBox(hWnd, "Данные не прочитаны. Ключ не введен!!", "Сообщение", MB_OK | MB_ICONHAND);
			}
			break;
		case ID_32772: 
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = "Шифрованные файлы (*.shf)\0*.shf\0Все файлы\0*.*\0";
			ofn.lpstrFile = NameFile;
			ofn.nMaxFile = 255;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
			ofn.lpstrTitle = NULL;
			ofn.lpstrDefExt = "shf"; 
			if (GetSaveFileName(&ofn)) 
			{
				
				if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, MyDialog) == IDOK)
				{
					char Shifr[512];
					crypt(Str, key, Shifr, strlen(Str), true);

					FILE *pF1; 
					pF1 = fopen(NameFile, "wb"); 
					if (pF1 == 0)
					{
						MessageBox(hWnd, "Ошибка открытия файла.", "Сообщение об ошибке", MB_OK | MB_ICONHAND);
						return TRUE;
					}
					fwrite(&len, sizeof(int), 1, pF1); 
					fwrite(Shifr, sizeof(char), len, pF1); 
					fclose(pF1);
					MessageBox(hWnd, "Данные успешно сохранены.", "Сообщение", MB_OK | MB_ICONINFORMATION);
				}
				else MessageBox(hWnd, "Данные не сохранены. Ключ не введен!!", "Сообщение", MB_OK | MB_ICONHAND);
			}

			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT: 
		hdc = BeginPaint(hWnd, &ps);
		
		SetTextColor(hdc, RGB(255, 0, 0));
		TextOut(hdc, 40, 40, Str, len); 
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
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
INT_PTR CALLBACK MyDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			if (LOWORD(wParam) == IDOK) 
			{
				char Str[32];
				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_GETTEXT, 31, (LPARAM)Str); 
																				  
				if (sscanf(Str, "%d", &key) < 1)
				{
					MessageBox(hDlg, "Неверный формат ключа. Ключ должен быть целым числом", "Сообщение о неверном формате ключа", MB_OK | MB_ICONHAND);
					return (INT_PTR)TRUE; 
				}
			}
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


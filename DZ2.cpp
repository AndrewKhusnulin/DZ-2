#include "stdafx.h"
#include "ForDZ2.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
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

 	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FORDZ2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FORDZ2));

	// Цикл основного сообщения:
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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FORDZ2));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_FORDZ2);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

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

// Функции шифрования -------------------
unsigned int F_Move(unsigned int Block, bool flag_move)	// Функция сдвига на 3 бита
{
	unsigned int S_Left, S_Right;
	if (flag_move)
	{
		S_Left = Block << 3; // Сдвиг 4-ёх байтового блока данных на 3 бита
		S_Right = (Block >> (8 * 4 - 3));
	}
	else
	{
		S_Right = (Block >> 3);
		S_Left = Block << (8 * 4 - 3);
	}
	return (S_Left | S_Right);
}

unsigned int F_Gamma(unsigned int Block) // Функция Гаммирования
{
	int Gamma = (rand() << (8 * 2)) | ((rand() << (8 * 2)) >> (8 * 2)); // Гамма состоит из 2 блоков по 2 байта
	return (Block ^ Gamma);
}

юvoid crypt(char original[512], int key, char result[512], bool flag_crypt) //истина шифрование, ложь дешифрование
{
	int const blocksize = 4;
	int SizeLastBlock, copy_blocksize;
	unsigned int Block1, Block2;
	int blocks;
	int strsize = strlen(original);

	blocks = strsize / blocksize;
	SizeLastBlock = strsize - blocks * blocksize;
if (SizeLastBlock != 0)	blocks++;
	srand(key); // Инициализация генератора ПСЧ ключом шифра
	for (int i = 0; i < blocks; i++)
	{
		if ((i != blocks) || (SizeLastBlock = 0))
			copy_blocksize = blocksize;           // Вычисление размера текущего блока
		else
			copy_blocksize = SizeLastBlock;
		memcpy(&Block1, original + (blocksize * i), copy_blocksize); // Копируем заданное кол-во байт Original в Block1
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
// Функции шифрования -------------

char Str[512];
int len = 0;
int key; // Сюда запишем значение секретного ключа

OPENFILENAME ofn; // Структура для стандартных окон "Открыть файл" и "Сохранить"
TCHAR NameFile[256] = "MyFile.shf"; // Сюда будет записано имя файла

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND	- обработка меню приложения
//  WM_PAINT	- Закрасить главное окно
//  WM_DESTROY	- ввести сообщение о выходе и вернуться.
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	
	switch (message)
	{
	case WM_CHAR: // Сообщение ввод символа
		// Записываем символ в строку
		Str[len] = wParam;
		len++; // Длина строки увеличивается на 1
		Str[len] = 0; // Строка заканчивается символом с кодом 0
		// Вывод строки на экран
		hdc = GetDC(hWnd); // Получаем контекст устройства
		SetTextColor(hdc, RGB(0, 0, 255));
		TextOut(hdc, 40, 40, Str, len); // Вывод строки
		ReleaseDC(hWnd, hdc); // Освобождаем контекст устройства
		break;
	case WM_COMMAND: // Сообщение от выбора элемента меню
		wmId    = LOWORD(wParam); // Получаем идентификатор (номер) элемента меню
		wmEvent = HIWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId) // Обработка отдельных элементов меню
		{
		case IDM_ABOUT: // О программе 
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT: // Выход
			DestroyWindow(hWnd);
			break;
		case ID_32771: // Открыть файл
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = NameFile; // Сюда будет записано имя файла
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = 255;
			ofn.lpstrFilter = "Шифрованные файлы (*.shf)\0*.shf\0Все файлы\0*.*\0"; // Фильтр для отображения файлов
			ofn.nFilterIndex = 0;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			ofn.lpstrDefExt = "shf"; // Строка расширение по умолчанию

			if (GetOpenFileName(&ofn)) // Появляется окно "Открыть", функция возвращает истинно, если нажата кнопка Открыть
			{
				// Отображаем окно для ввода ключа
				if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, MyDialog) == IDOK)
				{
					FILE *pF1;
					char Shifr[512]; // Зашифрованный текст
					pF1 = fopen(NameFile, "rb"); // Открываем файл для чтения в двоичном режиме
					if (pF1 == 0)
					{
						MessageBox(hWnd, "Ошибка открытия файла. Возможно файл не существует", "Сообщение об ошибке", MB_OK | MB_ICONHAND);
						return TRUE;
					}
					fread(&len, sizeof(int), 1, pF1); // Читаем Размер данных в байтах
					fread(Shifr, sizeof(char), len, pF1); // Читаем данные из файл
					fclose(pF1);
					crypt(Shifr, key, Str, false);
					Str[len] = 0; // Строка заканчивается 0
					InvalidateRect(hWnd, 0, TRUE); // Перерисовка окна
				}
				else MessageBox(hWnd, "Данные не прочитаны. Ключ не введен!!", "Сообщение", MB_OK | MB_ICONHAND);
			}
			break;
		case ID_32772: // Сохранить файл
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd; // родительское окно 
			ofn.lpstrFilter = "Шифрованные файлы (*.shf)\0*.shf\0Все файлы\0*.*\0"; // Маска (фильтр) для отображаемых файлов
			ofn.lpstrFile = NameFile;  // Сюда будет записано полное имя файла
			ofn.nMaxFile = 255;
			ofn.lpstrFileTitle = NULL;//
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
			ofn.lpstrTitle = NULL;
			ofn.lpstrDefExt = "shf"; // Строка расширение по умолчанию
			if (GetSaveFileName(&ofn)) // Появляется окно "Сохранить", функция возвращает истинно, если нажата кнопка Ok
			{
				// Отображаем окно для ввода ключа
				if (DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, MyDialog) == IDOK)
				{
					char Shifr[512]; // Зашифрованный текст
					crypt(Str, key,Shifr, true);

					FILE *pF1;  // Файл для сохранения
					pF1 = fopen(NameFile, "wb"); // Для зашифрованных данных файл в двоичном режиме
					if (pF1 == 0)
					{
						MessageBox(hWnd, "Ошибка открытия файла.", "Сообщение об ошибке", MB_OK | MB_ICONHAND);
						return TRUE;
					}
					fwrite(&len, sizeof(int), 1, pF1); // Размер данных в байтах
					fwrite(Shifr, sizeof(char), len, pF1); // Записываем данные в файл
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
	case WM_PAINT: // Перерисовка окна
		hdc = BeginPaint(hWnd, &ps);
		// TODO: добавьте любой код отрисовки...
		SetTextColor(hdc, RGB(0, 0, 255));
		TextOut(hdc, 40, 40, Str, len); // Вывод строки
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

// Обработчик сообщений для окна "О программе".
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

// Обработчик сообщений для окна "Ввод секретного ключа.
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
			if (LOWORD(wParam) == IDOK) // Читаем секретный ключ из текстового поля
			{
				char Str[32];
				SendDlgItemMessage(hDlg, IDC_EDIT1, WM_GETTEXT, 31, (LPARAM)Str); // Прочитали текст из текстового поля и записали в струку Str
				// Читаем данные из строки
				if (sscanf(Str, "%d", &key) < 1) // Целое значение не прочитано из строки
				{
					MessageBox(hDlg, "Неверный формат ключа. Ключ должен быть целым числом", "Сообщение о неверном формате ключа", MB_OK | MB_ICONHAND);
					return (INT_PTR)TRUE; // Выход без закрытия диалогового окна
				}
			}
			EndDialog(hDlg, LOWORD(wParam)); // Закрытие диалогового окна
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


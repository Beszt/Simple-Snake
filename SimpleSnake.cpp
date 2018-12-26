// Niezb�dny stuff
#include <windows.h>
#include <ctime>

#include "Graphics.h"

Graphics* graphics;

// Prototyp funkckcji WindowProc do obs�ugi komunikat�w
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// G��wny uchwyt programu do os�ugi wiadomo�ci o zako�czeniu programu
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HBITMAP hBitmap = NULL;
	switch (message)
	{
		case WM_CREATE:

		break;

		case WM_DESTROY:
		{
						   PostQuitMessage(0);
						   return 0;
		}

		case WM_PAINT:
		{

		}
	break;
	}


	return DefWindowProc(hWnd, message, wParam, lParam);
}

// G��wna funkcja WinAPI
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Stworzenie objektu typu "uchwyt" okna, zostanie wype�niony potem przez funkcj�
	HWND windowhandle;
	// Struktura okien (Windows Class)
	WNDCLASSEX wc;

	// Wst�pne czyszczenie pami�ci pod nasze okno (zabezpieczenie przed �mieciami)
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// Wype�nianie informacji o naszym oknie
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "MainWindow";

	// Rejestracja klasy Okna
	RegisterClassEx(&wc);

	RECT WindowRect = { 0, 0, 800, 600 }; //Rozmiar okna

	AdjustWindowRectEx(&WindowRect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

	// Stworzenie okna, wype�nienie hWnd, by mog�o pos�u�y� potem jako uchwyt
	windowhandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MainWindow", "Simple Snake", WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, 300, 150, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, NULL, NULL, hInstance, NULL);

	if (!windowhandle) return -1; //Zabezpieczenie

	//Inicjacja direct2d
	graphics = new Graphics();

	if (!graphics->Init(windowhandle)) //Zabezpieczenie
	{
		delete graphics;
		return -1;
	}

	// Wy�wietlenie okna
	ShowWindow(windowhandle, nCmdShow);

	// Stworzenie objektu dla komunikat�w
	MSG msg;

	//Zmienne globalne
	bool NewGame = true;
	bool EndGame = false;
	bool StartMove = false;
	int iMoves = 0;
	int iSnakeBlocks = 1;
	const int BoardsSize = 30;
	const int SnakeSize = 32;
	const int AppleSize = 32;

	D2D1_RECT_F Boards = { WindowRect.left + 25, WindowRect.top + 47, WindowRect.right - 25, WindowRect.bottom - 25 }; //Dodatkowe liczby to marginesy okna w px
	D2D1_RECT_F Snake[50000]; //Tablica na historie ruch�w w�a
	for (int i = 1; i < 50000; i++)
	{
		Snake[i] = {0, 0, 0, 0};
	}
	D2D1_RECT_F SnakeNP = { 0, 0, 0, 0 };

	D2D1_RECT_F Apple = { 0, 0, 0, 0 };
	D2D1_RECT_F AppleNP = { 0, 0, 0, 0 };

	bool bAppleNP = false;
	bool SnakeFP = false;
	int x; // G�owa w�a
	int y;
	int xA; // Jab�ko
	int yA;
	int score = 0;
	char Direction = 'n';

	// Niesko�czona, G��WNA p�tla programu
	while (TRUE)
	{
		// Obs�uga komunikat�w
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Obs�uga komunikatu zako�czaj�cego prac� programu
		if (msg.message == WM_QUIT)
		{
			break;
		}


		// Tutaj w�a�ciwa cz�� p�tli (Obs�uga silnika gry)
		graphics->BeginDraw();

		if (NewGame == true)
		{
			score = 0;
			Direction = 'n';

			iMoves = 1;
			iSnakeBlocks = 1;
			bAppleNP = false;
			StartMove = false;
			SnakeFP = false;
			NewGame = false;
			EndGame = false;

			for (int i = 1; i < 50000; i++)
			{
				Snake[i] = { 0, 0, 0, 0 };
			}
			graphics->ClearScreen(255, 255, 255);
			graphics->DrawBoards(Boards, BoardsSize);
			graphics->DrawScore();

			while (!SnakeFP) // Wyznaczanie losowej pocz�tkowej pozycji w�a
			{
				srand(time(NULL));
				x = (rand() % 750 - SnakeSize) + SnakeSize;
				y = (rand() % 550 - SnakeSize) + SnakeSize;
				Snake[1] = D2D1::RectF(x, y, x + SnakeSize, y + SnakeSize);

				if (!graphics->BoardsCollision(Snake[1], SnakeSize, Boards, BoardsSize)) // Zabezpiecznie, aby przy nowej grze, Snake nie pojawi� si� na �cianie
				{
					graphics->DrawSnakeHead(Snake[1], SnakeSize, Direction);
					SnakeFP = true;
				}
			}

			while (!bAppleNP) //Wyznaczanie losowej pozycji jab�ka
			{
				xA = rand() % 800;
				yA = rand() % 600;
				Apple = D2D1::RectF(xA, yA, xA + AppleSize, yA + AppleSize);

				if (!graphics->BoardsCollision(Apple, AppleSize, Boards, BoardsSize) && !graphics->AppleCollision(Snake[1], SnakeSize, Apple, AppleSize)) // Sprawdzanie czy jab�o nie wygeneruje si� na �cianach oraz na g�owie w�a
				{
					bAppleNP = true;
					AppleNP = Apple;
					graphics->DrawApple(Apple);
				}
			}
		}

		if (score < 50 && StartMove) //Szybko�c gry
		{
			Sleep(500 - score * 10);
		}
		

		//Oczekiwanie na klawisze i zmienianie kierunku w�a

		if (GetAsyncKeyState(VK_UP) && !GetAsyncKeyState(VK_LEFT) && !GetAsyncKeyState(VK_RIGHT))
		{
			if (Direction != 'd')
			{
				Direction = 'u';
			}

			if (StartMove == false)
			{
				StartMove = true;
				iMoves += 1;
			}
			
		}

		if (GetAsyncKeyState(VK_DOWN) && !GetAsyncKeyState(VK_LEFT) && !GetAsyncKeyState(VK_RIGHT))
		{
			if (Direction != 'u')
			{
				Direction = 'd';
			}

			if (StartMove == false)
			{
				StartMove = true;
				iMoves += 1;
			}

		}

		if (GetAsyncKeyState(VK_LEFT) && !GetAsyncKeyState(VK_UP) && !GetAsyncKeyState(VK_DOWN))
		{
			if (Direction != 'r')
			{
				Direction = 'l';
			}

			if (StartMove == false)
			{
				StartMove = true;
				iMoves += 1;
			}

		}

		if (GetAsyncKeyState(VK_RIGHT) && !GetAsyncKeyState(VK_UP) && !GetAsyncKeyState(VK_DOWN))
		{
			if (Direction != 'l')
			{
				Direction = 'r';
			}
			if (StartMove == false)
			{
				StartMove = true;
				iMoves += 1;
			}
		}

		//Ustalanie kierunku w�a

		if (Direction == 'u')
		{
			y -= SnakeSize;
		}
			

		if (Direction == 'd')
		{
			y += SnakeSize;
		}

		if (Direction == 'l')
		{
			x -= SnakeSize;
		}

		if (Direction == 'r')
		{
			x += SnakeSize;
		}

		SnakeNP = D2D1::RectF(x, y, x + SnakeSize, y + SnakeSize);

		if (graphics->BoardsCollision(SnakeNP, SnakeSize, Boards, BoardsSize && !EndGame)) //Sprawdzanie czy w�� nie wpad� na �ciane
		{
			NewGame = true;
			EndGame = true;
		}

		for (int i = iMoves - iSnakeBlocks; i <= iMoves; i++) //Sprawdzanie czy w�� nie wpad� na sw�j ogon
		{
			if (graphics->BodyCollision(Snake[i], SnakeNP, SnakeSize) && StartMove && !EndGame)
			{
				NewGame = true;
				EndGame = true;
			}
		}

		if (graphics->AppleCollision(SnakeNP, SnakeSize, Apple, AppleSize) && !EndGame) //Sprawdzanie czy w�� zebra� jab�ko
		{
			bAppleNP = false;
			score += 1;
			iSnakeBlocks += 1;
			Snake[iMoves] = SnakeNP;
			graphics->UpdateScore(score);
			while (bAppleNP == false) //Generowanie nowego jab�ka
			{
				xA = rand() % 800;
				yA = rand() % 600;
				AppleNP = D2D1::RectF(xA, yA, xA + AppleSize, yA + AppleSize);
				if (!graphics->BoardsCollision(AppleNP, AppleSize, Boards, BoardsSize)) //Sprawdzanie czy jab�ko nie wygeneruje si� na �cianach
				{
					for (int i = iMoves - iSnakeBlocks; i <= iMoves; i++) //Sprawdzanie czy jab�ko nie wygeneruje si� na ciele w�a
					{
						if (graphics->BodyCollision(Snake[i], AppleNP, SnakeSize))
						{
							break;
						}
						if (i == iMoves - 1)
						{
							bAppleNP = true;
							graphics->UpdateApple(Apple, AppleNP);
							Apple = AppleNP;
						}
					}
				}
			}
		}

		if (StartMove && !EndGame)
		{
			Snake[iMoves] = SnakeNP;
			iMoves += 1;

			for (int i = iMoves - iSnakeBlocks; i <= iMoves; i++) //Czyszczenie poprzedniego w�a i rysowanie nowego (od ty�u)
			{
				if (i == iMoves-1) //Je�li ostatnia przej�cie p�tli - wtedy rysuje g�owe
				{
					graphics->DrawSnakeBackground(Snake[i - iSnakeBlocks - 1]);
					graphics->DrawSnakeHead(Snake[i], SnakeSize, Direction);
				}
				else
				{
					graphics->DrawSnakeBackground(Snake[i - iSnakeBlocks - 1]); //Reszta cia�a
					graphics->DrawSnakeBody(Snake[i]);
				}
			}
		}

		graphics->DrawApple(Apple); //Rysowanie jab�ka jeszcze raz na samym ko�cu p�tli, aby by�o wiadomo �e narysuje si� ca�e

		graphics->EndDraw();

		if (EndGame) //Obs�uga GameOver'u
		{
			graphics->BeginDraw();
			graphics->ClearScreen(0, 0, 0);
			graphics->DrawEndGame(score, WindowRect);
			graphics->EndDraw();
			while (!GetAsyncKeyState(VK_RETURN)) //Dop�ki nie naci�nie si� Enteru, gra tkwi na ekranie ko�ca gry
			{

			}
			EndGame = false;
		}
	}

	delete graphics; //Zwalnianie zasob�w

	return msg.wParam;
}
// Niezbêdny stuff
#include <windows.h>
#include <ctime>

#include "Graphics.h"

Graphics* graphics;

// Prototyp funkckcji WindowProc do obs³ugi komunikatów
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// G³ówny uchwyt programu do os³ugi wiadomoœci o zakoñczeniu programu
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

// G³ówna funkcja WinAPI
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Stworzenie objektu typu "uchwyt" okna, zostanie wype³niony potem przez funkcjê
	HWND windowhandle;
	// Struktura okien (Windows Class)
	WNDCLASSEX wc;

	// Wstêpne czyszczenie pamiêci pod nasze okno (zabezpieczenie przed œmieciami)
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// Wype³nianie informacji o naszym oknie
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

	// Stworzenie okna, wype³nienie hWnd, by mog³o pos³u¿yæ potem jako uchwyt
	windowhandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MainWindow", "Simple Snake", WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, 300, 150, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, NULL, NULL, hInstance, NULL);

	if (!windowhandle) return -1; //Zabezpieczenie

	//Inicjacja direct2d
	graphics = new Graphics();

	if (!graphics->Init(windowhandle)) //Zabezpieczenie
	{
		delete graphics;
		return -1;
	}

	// Wyœwietlenie okna
	ShowWindow(windowhandle, nCmdShow);

	// Stworzenie objektu dla komunikatów
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
	D2D1_RECT_F Snake[50000]; //Tablica na historie ruchów wê¿a
	for (int i = 1; i < 50000; i++)
	{
		Snake[i] = {0, 0, 0, 0};
	}
	D2D1_RECT_F SnakeNP = { 0, 0, 0, 0 };

	D2D1_RECT_F Apple = { 0, 0, 0, 0 };
	D2D1_RECT_F AppleNP = { 0, 0, 0, 0 };

	bool bAppleNP = false;
	bool SnakeFP = false;
	int x; // G³owa wê¿a
	int y;
	int xA; // Jab³ko
	int yA;
	int score = 0;
	char Direction = 'n';

	// Nieskoñczona, G£ÓWNA pêtla programu
	while (TRUE)
	{
		// Obs³uga komunikatów
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Obs³uga komunikatu zakoñczaj¹cego pracê programu
		if (msg.message == WM_QUIT)
		{
			break;
		}


		// Tutaj w³aœciwa czêœæ pêtli (Obs³uga silnika gry)
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

			while (!SnakeFP) // Wyznaczanie losowej pocz¹tkowej pozycji wê¿a
			{
				srand(time(NULL));
				x = (rand() % 750 - SnakeSize) + SnakeSize;
				y = (rand() % 550 - SnakeSize) + SnakeSize;
				Snake[1] = D2D1::RectF(x, y, x + SnakeSize, y + SnakeSize);

				if (!graphics->BoardsCollision(Snake[1], SnakeSize, Boards, BoardsSize)) // Zabezpiecznie, aby przy nowej grze, Snake nie pojawi³ siê na œcianie
				{
					graphics->DrawSnakeHead(Snake[1], SnakeSize, Direction);
					SnakeFP = true;
				}
			}

			while (!bAppleNP) //Wyznaczanie losowej pozycji jab³ka
			{
				xA = rand() % 800;
				yA = rand() % 600;
				Apple = D2D1::RectF(xA, yA, xA + AppleSize, yA + AppleSize);

				if (!graphics->BoardsCollision(Apple, AppleSize, Boards, BoardsSize) && !graphics->AppleCollision(Snake[1], SnakeSize, Apple, AppleSize)) // Sprawdzanie czy jab³o nie wygeneruje siê na œcianach oraz na g³owie wê¿a
				{
					bAppleNP = true;
					AppleNP = Apple;
					graphics->DrawApple(Apple);
				}
			}
		}

		if (score < 50 && StartMove) //Szybkoœc gry
		{
			Sleep(500 - score * 10);
		}
		

		//Oczekiwanie na klawisze i zmienianie kierunku wê¿a

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

		//Ustalanie kierunku wê¿a

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

		if (graphics->BoardsCollision(SnakeNP, SnakeSize, Boards, BoardsSize && !EndGame)) //Sprawdzanie czy w¹¿ nie wpad³ na œciane
		{
			NewGame = true;
			EndGame = true;
		}

		for (int i = iMoves - iSnakeBlocks; i <= iMoves; i++) //Sprawdzanie czy w¹¿ nie wpad³ na swój ogon
		{
			if (graphics->BodyCollision(Snake[i], SnakeNP, SnakeSize) && StartMove && !EndGame)
			{
				NewGame = true;
				EndGame = true;
			}
		}

		if (graphics->AppleCollision(SnakeNP, SnakeSize, Apple, AppleSize) && !EndGame) //Sprawdzanie czy w¹¿ zebra³ jab³ko
		{
			bAppleNP = false;
			score += 1;
			iSnakeBlocks += 1;
			Snake[iMoves] = SnakeNP;
			graphics->UpdateScore(score);
			while (bAppleNP == false) //Generowanie nowego jab³ka
			{
				xA = rand() % 800;
				yA = rand() % 600;
				AppleNP = D2D1::RectF(xA, yA, xA + AppleSize, yA + AppleSize);
				if (!graphics->BoardsCollision(AppleNP, AppleSize, Boards, BoardsSize)) //Sprawdzanie czy jab³ko nie wygeneruje siê na œcianach
				{
					for (int i = iMoves - iSnakeBlocks; i <= iMoves; i++) //Sprawdzanie czy jab³ko nie wygeneruje siê na ciele wê¿a
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

			for (int i = iMoves - iSnakeBlocks; i <= iMoves; i++) //Czyszczenie poprzedniego wê¿a i rysowanie nowego (od ty³u)
			{
				if (i == iMoves-1) //Jeœli ostatnia przejœcie pêtli - wtedy rysuje g³owe
				{
					graphics->DrawSnakeBackground(Snake[i - iSnakeBlocks - 1]);
					graphics->DrawSnakeHead(Snake[i], SnakeSize, Direction);
				}
				else
				{
					graphics->DrawSnakeBackground(Snake[i - iSnakeBlocks - 1]); //Reszta cia³a
					graphics->DrawSnakeBody(Snake[i]);
				}
			}
		}

		graphics->DrawApple(Apple); //Rysowanie jab³ka jeszcze raz na samym koñcu pêtli, aby by³o wiadomo ¿e narysuje siê ca³e

		graphics->EndDraw();

		if (EndGame) //Obs³uga GameOver'u
		{
			graphics->BeginDraw();
			graphics->ClearScreen(0, 0, 0);
			graphics->DrawEndGame(score, WindowRect);
			graphics->EndDraw();
			while (!GetAsyncKeyState(VK_RETURN)) //Dopóki nie naciœnie siê Enteru, gra tkwi na ekranie koñca gry
			{

			}
			EndGame = false;
		}
	}

	delete graphics; //Zwalnianie zasobów

	return msg.wParam;
}
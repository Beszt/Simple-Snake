#pragma once

#include <d2d1.h>
#include <dwrite.h>

using namespace std;

class Graphics
{
	ID2D1Factory* factory;
	IDWriteFactory* wfactory;
	IDWriteTextFormat* textFormat;
	ID2D1HwndRenderTarget* renderTarget;
	ID2D1SolidColorBrush* brush;

public:

	// Funckje inizjalizuj¹ce
	Graphics();
	~Graphics();
	bool Init(HWND windowHandle);
	void BeginDraw() { renderTarget->BeginDraw(); }
	void EndDraw() { renderTarget->EndDraw(); }



	// Funckje direct2d
	void ClearScreen(float r, float g, float b);
	void DrawCircle(float x, float y, float radius, float r, float g, float b, float a);

	void DrawBoards(D2D1_RECT_F WindowSize, int Boards);

	// Rysowanie i silnik
	void DrawSnakeHead(D2D1_RECT_F Rect, int SnakeSize, char Direction);
	void DrawSnakeBody(D2D1_RECT_F Rect);
	void DrawSnakeBackground(D2D1_RECT_F Rect);

	void DrawApple(D2D1_RECT_F Apple);
	void UpdateApple(D2D1_RECT_F Apple, D2D1_RECT_F AppleNP);
	void DrawScore();
	void UpdateScore(int score);

	void DrawEndGame(int score, RECT WindowRect);

	// Kolizje
	bool BoardsCollision(D2D1_RECT_F SnakeHead, int SnakeSize, D2D1_RECT_F Boards, int BoradsSize);
	bool BodyCollision(D2D1_RECT_F Snake, D2D1_RECT_F SnakeNP, int SnakeSize);
	bool AppleCollision(D2D1_RECT_F SnakeHead, int SnakeSize, D2D1_RECT_F Apple, int AppleSize);
};
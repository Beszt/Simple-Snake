#pragma once

#include "Graphics.h"

#include <string>


Graphics::Graphics()
{
	factory = NULL;
	renderTarget = NULL;
	brush = NULL;

}

Graphics::~Graphics()
{
	if (factory) factory->Release();
	if (renderTarget) renderTarget->Release();
	if (brush) brush->Release();
}

bool Graphics::Init(HWND windowHandle) // Inizjalizacja direct2d/narzêdzi
{
	HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	if (res != S_OK) return false;

	res = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&wfactory));
	if (res != S_OK) return false;

	res = wfactory->CreateTextFormat(
		L"Verdana",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		12,
		L"pl-pl",
		&textFormat
		);
	if (res != S_OK) return false;
	
	RECT rect;
	GetClientRect(windowHandle, &rect);

	res = factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(windowHandle, D2D1::SizeU(rect.right, rect.bottom)),
		&renderTarget);

	res = renderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &brush);
	if (res != S_OK) return false;

	return true;
}



// Funkcje direct2d
void Graphics::ClearScreen(float r, float g, float b)
{
	renderTarget->Clear(D2D1::ColorF(r, g, b));
}

void Graphics::DrawBoards(D2D1_RECT_F WindowSize, int Boards)
{ 
	brush->SetColor(D2D1::ColorF(0, 0, 0));
	renderTarget->DrawRectangle(WindowSize, brush, Boards);
}

bool Graphics::BoardsCollision(D2D1_RECT_F SnakeHead, int SnakeSize, D2D1_RECT_F Boards, int BoradsSize)
{
	if ((SnakeHead.left - (SnakeSize / 2)) <= Boards.left)
	{
		return true;
	}

	if ((SnakeHead.top - (SnakeSize / 2)) <= Boards.top)
	{
		return true;
	}
	if ((SnakeHead.right + (SnakeSize / 2)) >= Boards.right)
	{
		return true;
	}

	if ((SnakeHead.bottom + (SnakeSize / 2)) >= Boards.bottom)
	{
		return true;
	}

return false;
}

bool Graphics::BodyCollision(D2D1_RECT_F Snake, D2D1_RECT_F SnakeNP, int SnakeSize)
{
	if (SnakeNP.left < Snake.left + SnakeSize &&
		SnakeNP.left + SnakeSize > Snake.left &&
		SnakeNP.top < Snake.top + SnakeSize &&
		SnakeSize + SnakeNP.top > Snake.top)
		{
			return true;
		}
	return false;
}

bool Graphics::AppleCollision(D2D1_RECT_F SnakeHead, int SnakeSize, D2D1_RECT_F Apple, int AppleSize)
{
	if (SnakeHead.left < Apple.left + AppleSize &&
		SnakeHead.left + AppleSize > Apple.left &&
		SnakeHead.top < Apple.top + AppleSize &&
		AppleSize + SnakeHead.top > Apple.top)
		{
			return true;
		}
	return false;
}

void Graphics::DrawSnakeHead(D2D1_RECT_F Rect, int SnakeSize, char Direction) //Rysowanie g³owy wê¿a
{
	brush->SetColor(D2D1::ColorF(255, 255, 0)); //G³owa
	renderTarget->FillRectangle(Rect, brush);

	brush->SetColor(D2D1::ColorF(0, 0, 0));

	if (Direction == 'n') //Pozycja startowa "neutralna"
	{
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.left + SnakeSize / 4, Rect.top + SnakeSize / 2), SnakeSize / 6, SnakeSize / 6), brush, 2); // 1 oko
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.right - SnakeSize / 4, Rect.top + SnakeSize / 2), SnakeSize / 6, SnakeSize / 6), brush, 2); // 2 oko

		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.left + SnakeSize / 4, Rect.top + SnakeSize / 2), 1, 1), brush, 1); // 1 Ÿrenica
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.right - SnakeSize / 4, Rect.top + SnakeSize / 2), 1, 1), brush, 1); // 2 Ÿrenica
	}

	if (Direction == 'u') //G³owa skierowana ku górze
	{
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.left + SnakeSize / 4, Rect.top + SnakeSize / 4), SnakeSize / 6, SnakeSize / 6), brush, 2); // 1 oko
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.right - SnakeSize / 4, Rect.top + SnakeSize / 4), SnakeSize / 6, SnakeSize / 6), brush, 2); // 2 oko

		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.left + SnakeSize / 4, Rect.top + SnakeSize / 4), 1, 1), brush, 1); // 1 Ÿrenica
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.right - SnakeSize / 4, Rect.top + SnakeSize / 4), 1, 1), brush, 1); // 2 Ÿrenica
	}

	if (Direction == 'd') //G³owa skierowana ku do³owi
	{
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.left + SnakeSize / 4, Rect.bottom - SnakeSize / 4), SnakeSize / 6, SnakeSize / 6), brush, 2); // 1 oko
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.right - SnakeSize / 4, Rect.bottom - SnakeSize / 4), SnakeSize / 6, SnakeSize / 6), brush, 2); // 2 oko

		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.left + SnakeSize / 4, Rect.bottom - SnakeSize / 4), 1, 1), brush, 1); // 1 Ÿrenica
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.right - SnakeSize / 4, Rect.bottom - SnakeSize / 4), 1, 1), brush, 1); // 2 Ÿrenica
	}

	if (Direction == 'l') //Pozycja startowa "neutralna"
	{
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.left + SnakeSize / 4, Rect.top + SnakeSize / 4), SnakeSize / 6, SnakeSize / 6), brush, 2); // 1 oko
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.left + SnakeSize / 4, Rect.bottom - SnakeSize / 4), SnakeSize / 6, SnakeSize / 6), brush, 2); // 2 oko

		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.left + SnakeSize / 4, Rect.top + SnakeSize / 4), 1, 1), brush, 1); // 1 Ÿrenica
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.left + SnakeSize / 4, Rect.bottom - SnakeSize / 4), 1, 1), brush, 1); // 2 Ÿrenica
	}

	if (Direction == 'r') //Pozycja startowa "neutralna"
	{
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.right - SnakeSize / 4, Rect.top + SnakeSize / 4), SnakeSize / 6, SnakeSize / 6), brush, 2); // 1 oko
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.right - SnakeSize / 4, Rect.bottom - SnakeSize / 4), SnakeSize / 6, SnakeSize / 6), brush, 2); // 2 oko

		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.right - SnakeSize / 4, Rect.top + SnakeSize / 4), 1, 1), brush, 1); // 1 Ÿrenica
		renderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(Rect.right - SnakeSize / 4, Rect.bottom - SnakeSize / 4), 1, 1), brush, 1); // 2 Ÿrenica
	}
}

void Graphics::DrawSnakeBody(D2D1_RECT_F Rect)
{
	brush->SetColor(D2D1::ColorF(0, 255, 0));
	renderTarget->FillRectangle(Rect, brush);
}

void Graphics::DrawSnakeBackground(D2D1_RECT_F Rect)
{
	brush->SetColor(D2D1::ColorF(255, 255, 255));
	renderTarget->FillRectangle(Rect, brush);
}

void Graphics::DrawApple(D2D1_RECT_F Apple)
{
	brush->SetColor(D2D1::ColorF(255, 0, 0));
	renderTarget->FillRectangle(Apple, brush);
}

void Graphics::UpdateApple(D2D1_RECT_F Apple, D2D1_RECT_F AppleNP) //Ruch wê¿a
{
	brush->SetColor(D2D1::ColorF(255, 255, 255));
	renderTarget->FillRectangle(Apple, brush);
	brush->SetColor(D2D1::ColorF(255, 0, 0));
	renderTarget->FillRectangle(AppleNP, brush);
}

void Graphics::DrawScore() //Rysowanie pocz¹tkowego wyniku
{
	brush->SetColor(D2D1::ColorF(255, 0, 0));

	renderTarget->DrawText(
		L"Score: 0",
		ARRAYSIZE(L"Score: 0") - 1,
		textFormat,
		D2D1::RectF(4, 2, 100, 12),
		brush
		);
}

void Graphics::UpdateScore(int score) //Aktualizacja wyniku
{
	brush->SetColor(D2D1::ColorF(0, 0, 0)); //Czyszczenie poprzedniego wyniku
	renderTarget->FillRectangle(D2D1::RectF(0, 0, 120, 20), brush);

	// Konwersja na LPCWSTR
	wchar_t buffer[256];
	wsprintfW(buffer, L"%d", score);
	LPCWSTR intScore = buffer;

	std::wstring t1 = std::wstring(L"Score: ") + intScore;

	LPCWSTR endScore = t1.c_str();

	brush->SetColor(D2D1::ColorF(255, 0, 0)); //Czyszczenie poprzedniego wyniku
	renderTarget->DrawText(
		endScore,
		ARRAYSIZE(L"Score: ") + (int)log10(score) + 1, //Wyznaczanie w bajtach ile "z³¹czony" string, zajmuje miejsca w pamiêci
		textFormat,
		D2D1::RectF(4, 2, 100, 12),
		brush
		);
}

void Graphics::DrawEndGame(int score, RECT WindowRect)
{
	brush->SetColor(D2D1::ColorF(255, 0, 0));
	// Konwersja na LPCWSTR

	if (score > 0)
	{
		wchar_t buffer[256];
		wsprintfW(buffer, L"%d", score);
		LPCWSTR intScore = buffer;

		std::wstring t1 = std::wstring(L"End score: ") + intScore + std::wstring(L"\n\nPress enter...");

		LPCWSTR endScore = t1.c_str();

		renderTarget->DrawText(
			endScore,
			ARRAYSIZE(L"End score: ") + (int)log10(score) + ARRAYSIZE(L"\n\nPress enter..."), //Wyznaczanie w bajtach ile "z³¹czony" string, zajmuje miejsca w pamiêci
			textFormat,
			D2D1::RectF(WindowRect.right / 2 - 50, WindowRect.bottom / 2 - textFormat->GetFontSize() * 3, WindowRect.right / 2 + 50, WindowRect.bottom / 2 + textFormat->GetFontSize() * 3), // WindowRect.bottom GetFontSize() * ilosc wierszy w tekscie
			brush
			);
	}
	else
	{
		renderTarget->DrawText(
			L"End score: 0 \n\nPress enter...",
			ARRAYSIZE(L"End score: 0 \n\nPress enter..."), //Wyznaczanie w bajtach ile "z³¹czony" string, zajmuje miejsca w pamiêci
			textFormat,
			D2D1::RectF(WindowRect.right / 2 - 50, WindowRect.bottom / 2 - textFormat->GetFontSize() * 3, WindowRect.right / 2 + 50, WindowRect.bottom / 2 + textFormat->GetFontSize() * 3), // WindowRect.bottom GetFontSize() * ilosc wierszy w tekscie
			brush
			);
	}
	
}
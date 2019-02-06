#include "SDL.h"
#include "SDL_image.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <tuple>
#include <vector>


int posX = 100;
int posY = 200;
int sizeX = 500;
int sizeY = 500;

int shape_size = 164;

SDL_Rect SrcR;
SDL_Rect DestR;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* surface;
SDL_Texture* texture;
SDL_Texture* cross;
SDL_Texture* nought;

bool InitEverything();
bool InitSDL();
bool CreateWindow();
bool CreateRenderer();
void SetupRenderer();

void Render();
void RunGame();

int GetBoardPosition(SDL_Event e);
std::tuple<int, int> GetXandYPositions(int position);
void HandlePosition(int position, int player);
void DrawMove(int xpos, int ypos, int player);
void ComputerMove(int player);
std::vector<int> GetReaminingPossibilities();
int CheckWinner(int(&board)[3][3]);
int CheckRowWinner(int(&board)[3][3], int player);
int CheckColumnWinner(int(&board)[3][3], int player);
int CheckDiagonalWinner(int(&board)[3][3], int player);
int GetBestOption(std::vector<int>);

void Beep();

int board[3][3] = {
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0}
};
float elementSizeX = (float)sizeX / 3;
float elementSizeY = (float)sizeY / 3;


int main(int argc, char* args[])
{
	if (!InitEverything())
		return -1;

	RunGame();
}

void Render()
{
	// Render the changes above
	SDL_RenderPresent(renderer);
}

bool InitEverything()
{
	if (!InitSDL())
		return false;

	if (!CreateWindow())
		return false;

	if (!CreateRenderer())
		return false;

	SetupRenderer();

	return true;
}

bool InitSDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		std::cout << " Failed to initialize SDL : " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}

bool CreateWindow()
{
	window = SDL_CreateWindow("Tic tac toe", posX, posY, sizeX, sizeY, 0);

	if (window == nullptr)
	{
		std::cout << "Failed to create window : " << SDL_GetError();
		return false;
	}

	return true;
}

bool CreateRenderer()
{
	renderer = SDL_CreateRenderer(window, -1, 0);

	if (renderer == nullptr)
	{
		std::cout << "Failed to create renderer : " << SDL_GetError();
		return false;
	}

	return true;
}

void SetupRenderer()
{
	// Set size of renderer to the same as window
	SDL_RenderSetLogicalSize(renderer, sizeX, sizeY);

	// Set color of renderer to green
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

	surface = SDL_LoadBMP("board.bmp");
	if (surface == NULL)
	{
		printf("Couldn't load surface!");
	}
	else
	{
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (texture == NULL)
		{
			printf("Coudn't create texture!");
		}
		cross = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("cross.bmp"));
		if (cross == NULL)
		{
			printf("Coudn't create cross texture!");
		}
		nought = SDL_CreateTextureFromSurface(renderer, SDL_LoadBMP("nought.bmp"));
		if (texture == NULL)
		{
			printf("Coudn't create nought texture!");
		}
		SDL_FreeSurface(surface);
	}

	SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void RunGame()
{
	bool loop = true;
	int position = NULL;

	SrcR.x = 0;
	SrcR.y = 0;
	SrcR.w = shape_size;
	SrcR.h = shape_size;
	DestR.w = shape_size;
	DestR.h = shape_size;

	while (loop)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				loop = false;
			else if (e.type == SDL_MOUSEBUTTONUP and e.button.button == SDL_BUTTON_LEFT)
			{
				position = GetBoardPosition(e);
				HandlePosition(position, 1);
				ComputerMove(2);
				std::cout << position << std::endl;
				Beep();
			}
		}

		Render();

		SDL_Delay(16);
	}
}

int GetBoardPosition(SDL_Event e)
{
	int position = NULL;
	if (e.button.y < elementSizeY)
	{
		if (e.button.x <= elementSizeX) { position = 0; }
		else if (e.button.x > elementSizeX && e.button.x <= 2 * elementSizeX) { position = 1; }
		else if (e.button.x > 2 * elementSizeX) { position = 2; }
	}
	else if (e.button.y > elementSizeY && e.button.y <= 2 * elementSizeY)
	{
		if (e.button.x <= elementSizeX) { position = 3; }
		else if (e.button.x > elementSizeX && e.button.x <= 2 * elementSizeX) { position = 4; }
		else if (e.button.x > 2 * elementSizeX) { position = 5; }
	}
	else if (e.button.y > 2 * elementSizeY)
	{
		if (e.button.x <= elementSizeX) { position = 6; }
		else if (e.button.x > elementSizeX && e.button.x <= 2 * elementSizeX) { position = 7; }
		else if (e.button.x > 2 * elementSizeX) { position = 8; }
	}
	return position;
}

std::tuple<int, int> GetXandYPositions(int position) 
{
	int xpos = position % 3;
	int ypos = position / 3;
	return std::make_tuple(xpos, ypos);
}
void HandlePosition(int position, int player)
{
	int xpos, ypos;
	std::tie(xpos, ypos) = GetXandYPositions(position);

	if (board[ypos][xpos] == 0) 
	{
		board[ypos][xpos] = player;
		DrawMove(xpos, ypos, player);

		int isWinner = CheckWinner(board);
		if (isWinner == 1) { printf("Player 1 Won!"); }
		else if (isWinner == 2) { printf("Player 2 Won!"); }
	}	
}

void DrawMove(int xpos, int ypos, int player)
{
	int offset = 4;
	DestR.x = xpos*(shape_size + offset);
	DestR.y = ypos*(shape_size + offset);

	if (player == 1) 
	{
		SDL_RenderCopy(renderer, cross, &SrcR, &DestR);
	}
	if (player == 2) 
	{
		SDL_RenderCopy(renderer, nought, &SrcR, &DestR);
	}
}

void ComputerMove(int player)
{
	int xpos, ypos;

	std::vector<int> remainingPossibilities = GetReaminingPossibilities();

	
	if (remainingPossibilities.size() > 0)
	{
		//Choose best option between remaining possibilities
		//position = GetBestOption()
		int randNum = rand() % remainingPossibilities.size();
		int position = remainingPossibilities[randNum];

		std::tie(xpos, ypos) = GetXandYPositions(position);

		if (board[ypos][xpos] == 0)
		{
			board[ypos][xpos] = player;
			DrawMove(xpos, ypos, player);
		}
	}
}

std::vector<int> GetReaminingPossibilities()
{
	std::vector<int> possibilities;
	int position = NULL;
	for (size_t i = 0; i < (sizeof(*board) / sizeof(**board)); i++)
	{
		for (size_t j = 0; j < (sizeof(board) / sizeof(*board)); j++)
		{
			if (board[i][j] == 0)
			{
				position = 3*i + j;
				possibilities.push_back(position);
			}
		}
	}
	return possibilities;
}

int CheckWinner(int (&board)[3][3])
{
	int result = 0;
	for (size_t i_player = 1; i_player <= 2; i_player++)
	{
		if (CheckRowWinner(board, i_player) != 0) { result = i_player; }
		else if (CheckColumnWinner(board, i_player) != 0) { result = i_player; }
		else if (CheckDiagonalWinner(board, i_player) != 0) { result = i_player; }
	}
	return result;
}

int CheckRowWinner(int(&board)[3][3], int player)
{
	int result = 0;
	for (size_t i = 0; i < (sizeof(*board) / sizeof(**board)); i++)
	{
		for (size_t j = 0; j < (sizeof(board) / sizeof(*board)); j++)
		{
			if (board[i][1] == player)
			{
				if (board[i][0] == player && board[i][2] == player)
				{
					result = player;
				}
			}
		}
	}
	return result;
}

int CheckColumnWinner(int(&board)[3][3], int player)
{
	int result = 0;
	for (size_t i = 0; i < (sizeof(*board) / sizeof(**board)); i++)
	{
		for (size_t j = 0; j < (sizeof(board) / sizeof(*board)); j++)
		{
			if (board[1][j] == player)
			{
				if (board[0][j] == player && board[2][j] == player)
				{
					result = player;
				}
			}
		}
	}
	return result;
}

int CheckDiagonalWinner(int(&board)[3][3], int player)
{
	int result = 0;
	for (size_t i = 0; i < (sizeof(*board) / sizeof(**board)); i++)
	{
		for (size_t j = 0; j < (sizeof(board) / sizeof(*board)); j++)
		{
			if (board[1][1] == player)
			{
				if ((board[0][0] == player && board[2][2] == player) || (board[0][2] == player && board[2][0] == player))
				{
					result = player;
				}
			}
		}
	}
	return result;
}

void Beep()
{
	std::cout << "\a" << std::flush;
}


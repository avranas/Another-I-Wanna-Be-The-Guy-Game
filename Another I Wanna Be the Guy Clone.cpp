#include "stdafx.h"
#include <SDL.h>
#include <SDL_image.h>
#include "SDL_ttf.h"
#include "globals.h"
#include "main_game.h"
#include "Title Screen.h"
#include "Music and Sound.h"
#include <iostream>

//Make sure you do the following on your next project:
//Make a Movable_Object class; anything that follows the game's laws of physics inherits from this class.\
	this inherits from Displayable_Object. It has a logic() pure virtual function and a common_logic() function.\
	common_logic() does handling of the physics then calls logic()
//Every "super class" should have a common_logic() function that calls a pure virtual special_logic() function.\
//List out mistakes as you go, just like this!
//Read through the google style guide again and follow it
//Avoid global variables

void initialize_SDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
	else
	{
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
			printf("Warning: Linear texture filtering not enabled!");
		//Create window
		g_window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, g_screen_width, g_screen_height, SDL_WINDOW_INPUT_FOCUS);
		if (g_window == NULL)
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		else
		{
			//Create renderer for window
			g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
			cout << "Renderer initialized!" << endl;
			if (g_renderer == NULL)
				printf("Renderer could not be created! SDL Error: %s\n",
					SDL_GetError());
			else
			{
				//Initialize renderer color
				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
					printf("SDL_image could not initialize! SDL_image Error: %s\n",
						IMG_GetError());
				//initialize ttf
				if (TTF_Init() == -1)
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n",
						TTF_GetError());
			}
			Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
			Mix_Init(MIX_INIT_MP3);
			Mix_Volume(-1, 128);
			Mix_VolumeMusic(128);
		}
	}
}

void load_fonts()
{
	Font::Calibri_24 = TTF_OpenFont("Calibri.ttf", 24);
}

int main(int argc, char* args[])
{
	//initialize everything here
	initialize_SDL();
	load_fonts();
	//Start the timer
	g_timer.start();
	load_all_music();
	load_all_sounds();
	load_pic_map();
	load_tiles();
	MainGame* main_game = nullptr;
	Title_Screen* title_screen = nullptr;
	Introduction* introduction = new Introduction;
	//Make sure current_game_state and next_game_state are set to the same value
	GameStateType current_game_state = StateIntroduction;
	g_next_game_state = StateIntroduction;
	SDL_SetRenderDrawColor(g_renderer, g_default_red, g_default_green, g_default_blue, g_default_alpha);
	SDL_SetWindowIcon(g_window, IMG_Load("Alex.png"));
	SDL_SetWindowTitle(g_window, "Another I Wanna Be the Guy Clone");
	//Uncomment the next line to hide the cursor!
	////SDL_ShowCursor(SDL_DISABLE);
	while (g_main_loop_is_running)
	{
		//After every loop, update the Timer
		g_timer.update_deltaT();
		switch (current_game_state)
		{
		case StateIntroduction:
			introduction->handle_events();
			introduction->logic();
			introduction->render();
			break;
		case StateTitleScreen:
			title_screen->handle_events();
			title_screen->logic();
			title_screen->render();
			break;
		case StateMainGame:
			main_game->handle_events();
			main_game->logic();
			if (!g_window_is_minimized)
				main_game->render();
			main_game->finish();
			break;
		case StatePausedGame:
			break;
		default:
			break;
		}
		if (current_game_state != g_next_game_state)
		{
			switch (g_next_game_state)
			{
			case StateTitleScreen:
				delete main_game;
				main_game = nullptr;
				delete introduction;
				introduction = nullptr;
				title_screen = new Title_Screen;
				break;
			case StateMainGame:
				delete title_screen;
				title_screen = nullptr;
				main_game = new MainGame;
				break;
			case StateIntroduction:
				delete main_game;
				main_game = nullptr;
				delete title_screen;
				title_screen = nullptr;
				introduction = new Introduction;
				break;
			}
			current_game_state = g_next_game_state;
		}
	}
	return 0;
}

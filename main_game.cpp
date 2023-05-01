#include "stdafx.h"
#include <SDL.h>
#include "Music and Sound.h"
#include <iostream>
#include "globals.h"
#include "main_game.h"
#include "Doorway.h"
#include "Boss Health.h"
#include <algorithm>

using namespace std;
using namespace Game;

//This is used for testing
bool show_hazard_boundaries = true;

void put_camera_in_position()
{
	camera->set_main_position_x(static_cast<double>(Game::current_world->current_position_x() * g_screen_width));
	camera->set_main_position_y(static_cast<double>(Game::current_world->current_position_y() * g_screen_height));
}

namespace Game
{
	The_Kid* the_kid = nullptr;
	World* current_world = nullptr;
	Screen* current_screen = nullptr;
	Game_Over_Text* game_over_text = nullptr;
	//Do this better!!
	Text_Box* death_counter_text = nullptr;
	Text_Box* death_counter_shadow = nullptr;
}

void update_death_counter_image()
{
	string new_string;
	string death_counter_as_string = to_string(save_file->reset_counter());
	new_string = "Restarts : " + death_counter_as_string;
	//death_counter_as_string = to_string(save_file->reset_counter()); 
	//new_string += "  (" + death_counter_as_string + ')';
	death_counter_text->change_the_text(new_string.c_str());
	death_counter_shadow->change_the_text(new_string.c_str());
}

void MainGame::update_timer_display()
{
	string new_string = "Time : " + take_ms_return_english(save_file->time());
	timer_display->change_the_text(new_string.c_str());
	timer_display_shadow->change_the_text(new_string.c_str());
}

string get_world_file_name(const int world_number)
{
	switch (world_number)
	{
	case 0:
		return "Tutorial World.txt";
	case 1:
		return "End World.txt";
	case 2:
		return "World 2 Data.txt";
	case 3:
		return "World 1 Data.txt";
	default:
		cout << "Error occured in get_world_file_name() world_number: " << world_number << endl;
		return nullptr;
	}
}

void MainGame::go_here(int world, int screen_x, int screen_y, double kid_position_x, double kid_position_y)
{
	delete the_kid;
	the_kid = nullptr;
	the_bullets.clear();
	the_kid = new The_Kid;
	delete boss_health_bar;
	boss_health_bar = nullptr;
	boss_health_bar = new Boss_Health_Bar;
	delete current_world;
	current_world = nullptr;
	current_world = new World(get_world_file_name(world), screen_x, screen_y);
	current_world->switch_to_this_screen(screen_x, screen_y);
	delete camera;
	camera = new Camera;
	put_camera_in_position();
	camera->remove_offset();
	Mix_HaltChannel(-1);
	the_kid->set_hit_box_left(kid_position_x);
	the_kid->set_hit_box_top(kid_position_y);
}

void MainGame::load_save_data(string file_name)
{
	save_file->increment_resets_and_save();
	save_file->open_from_file();
	update_death_counter_image();
	go_here(save_file->world(),
		save_file->screen_x(),
		save_file->screen_y(),
		save_file->character_position_x(),
		save_file->character_position_y()
	);
}

MainGame::MainGame() :
	bullet_buffer_(0),
	timer_display(new Text_Box(5, 70 - 40, "0", Font::Calibri_24, Color::white, 1)),
	timer_display_shadow(new Text_Box(7, 72 - 40, "0", Font::Calibri_24, Color::black, 1)),
	time_since_last_reset_(0)
{
	if (g_debug_mode_on)
	{
		mouse_position_x_ = new Text_Box(505, 5, "0", Font::Calibri_24, Color::red, 1);
		mouse_position_y_ = new Text_Box(505, 25, "0", Font::Calibri_24, Color::red, 1);
	}
	Pic::bullet->init();
	delete game_over_text;
	delete death_counter_shadow;
	game_over_text = nullptr;
	death_counter_shadow = new Text_Box(7, 47 - 40, "0", Font::Calibri_24, Color::black, 1);
	delete death_counter_text;
	death_counter_text = new Text_Box(5, 45 - 40, "0", Font::Calibri_24, Color::white, 1);
	game_over_text = new Game_Over_Text(112, 192);
	star_hud_filled_ = new Displayable_Object(6, 105 - 40, Pic::star_hud_filled);
	//star_hud_empty_ = new Displayable_Object(6, 105 - 40, Pic::star_hud_empty);
	teleporter_hud_filled_ = new Displayable_Object(64, 104 - 40, Pic::teleporter_hud_filled);
	//teleporter_hud_empty_ = new Displayable_Object(64, 104 - 40, Pic::teleporter_hud_empty);
	//reset_counter = new Text_Box(SDL_Rect(16, 26, 11, 22), " (0)", Font::Calibri_24, SDL_Color(0xFF, 0xFF, 0xFF, 0x00));

	//When initializing the kid, make sure you load his position immediately afterwards.\
	...Actually I'm pretty sure this is the only place you have to do this. Either way, make sure\
	you do it.
	//We're going to need a default position or something. Or maybe default const save data if\
	no save file is found.
	load_save_data(save_file->file_name());
}

MainGame::~MainGame()
{
	if (mouse_position_x_ != nullptr)
	{
		delete mouse_position_x_;
		mouse_position_x_ = nullptr;
	}
	if (mouse_position_y_ != nullptr)
	{
		delete mouse_position_y_;
		mouse_position_y_ = nullptr;
	}
	delete game_over_text;
	game_over_text = nullptr;
	delete death_counter_text;
	death_counter_text = nullptr;
	delete death_counter_shadow;
	death_counter_shadow = nullptr;
	delete timer_display;
	timer_display = nullptr;
	delete timer_display_shadow;
	timer_display_shadow = nullptr;
	delete star_hud_filled_;
	//delete star_hud_empty_;
	delete teleporter_hud_filled_;
	//delete teleporter_hud_empty_;
	star_hud_filled_ = nullptr;
	//star_hud_empty_= nullptr;
	teleporter_hud_filled_ = nullptr;
	//teleporter_hud_empty_= nullptr;
	delete current_world;
	current_world = nullptr;
	delete the_kid;
	the_kid = nullptr;
	the_bullets.clear();
	the_kid = new The_Kid;
	delete boss_health_bar;
	boss_health_bar = nullptr;
	delete Game::camera;
	Game::camera = new Camera;
}

//This is bad code. Never do this again.
bool bad_code_that_forces_the_player_to_let_go_of_the_jump_key = false;
bool bad_code_that_means_shoot_is_pressed = false;
bool bad_code_that_forces_the_player_to_let_go_of_the_shoot_key = false;

void MainGame::handle_events()
{
	for (auto iter = current_screen->magic_teleport_boxes.begin(); iter != current_screen->magic_teleport_boxes.end(); iter++)
	{
		if ((*iter)->overlaps(&Game::the_kid->kid_hit_box()))
		{
			go_here((*iter)->goes_to_world(),
				(*iter)->goes_to_screen_x(),
				(*iter)->goes_to_screen_y(),
				(*iter)->goes_to_position_x(),
				(*iter)->goes_to_position_y()
			);
			break;
		}
	}
	//Poll input
	while (SDL_PollEvent(&g_event))
	{
		if (g_event.type == SDL_QUIT)
			g_main_loop_is_running = false;
		if (g_event.jbutton.type == SDL_JOYBUTTONDOWN)
		{
			the_kid->start_walking_right();
		}
		if (g_event.type == SDL_WINDOWEVENT)
		{
			switch (g_event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				SDL_Log("Window %d resized to %dx%d",
					g_event.window.windowID, g_event.window.data1,
					g_event.window.data2);
				change_screen_width(g_event.window.data1);
				change_screen_height(g_event.window.data2);
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				g_window_is_minimized = true;
				break;
			case SDL_WINDOWEVENT_RESTORED:
				g_window_is_minimized = false;
				break;
			default:
				break;
			}
		}
		if (g_event.type == SDL_KEYDOWN)
		{
			switch (g_event.key.keysym.sym)
			{
			case SDLK_F1:
				if (show_hazard_boundaries == false)
					show_hazard_boundaries = true;
				else
					show_hazard_boundaries = false;
				break;
				//case SDLK_1:
				//	current_screen->grab_a_smart_hazard(0)->grow();
				//	break;
				//case SDLK_2:
				//	current_screen->grab_a_smart_hazard(0)->shrink();
				//	break;
			case SDLK_3:
				//camera->x -= 10;
				break;
			case SDLK_4:
				//camera->x += 10;
				break;
			case SDLK_5:
				//camera->y -= 10;
				break;
			case SDLK_6:
				//camera->y += 10;
				break;
			case SDLK_F2:
				g_next_game_state = StateIntroduction;
				break;
			case SDLK_RIGHT:
				the_kid->start_walking_right();
				break;
			case SDLK_LEFT:
				the_kid->start_walking_left();
				break;
			case SDLK_DOWN:
				//the_kid->downdown();
				break;
			case SDLK_UP:
			{
				Doorway* returned_door = current_screen->search_for_available_doors();
				if (returned_door != nullptr)
				{
					go_here(
						returned_door->goes_to_world(),
						returned_door->goes_to_screen_x(),
						returned_door->goes_to_screen_y(),
						returned_door->goes_to_position_x(),
						returned_door->goes_to_position_y()
					);
				}
				break;
			}
			case SDLK_p:
				//This is an "erase-all-data-and-start-from-the-very-beginning-button" used for testing purposes.
				//reset_save();
				//reset_deaths();
				load_save_data(save_file->file_name());
				break;
			case SDLK_r:
				load_save_data(save_file->file_name());
				break;
			case SDLK_t:
				if (save_file->has_teleporter() && !the_kid->dead())
				{
					go_here(
						0,
						9,
						0,
						10780,
						208
					);
				}
				break;
			case SDLK_z:
				if (!bad_code_that_forces_the_player_to_let_go_of_the_jump_key)
				{
					bad_code_that_forces_the_player_to_let_go_of_the_jump_key = true;
					the_kid->jump();
				}
				break;
			case SDLK_x:
				if (!bad_code_that_forces_the_player_to_let_go_of_the_shoot_key)
				{
					bad_code_that_forces_the_player_to_let_go_of_the_shoot_key = true;
					the_kid->load_bullet();
				}
				break;
			default:
				break;
			}
		}
		if (g_event.type == SDL_KEYUP)
		{
			switch (g_event.key.keysym.sym)
			{
			case SDLK_d:
				the_kid->stop_walking_right();
				break;
			case SDLK_a:
				the_kid->stop_walking_left();
				break;
			case SDLK_LEFT:
				the_kid->stop_walking_left();
				break;
			case SDLK_RIGHT:
				the_kid->stop_walking_right();
				break;
			case SDLK_z:
				bad_code_that_forces_the_player_to_let_go_of_the_jump_key = false;
				the_kid->stop_jumping();
				break;
			case SDLK_x:
				bad_code_that_forces_the_player_to_let_go_of_the_shoot_key = false;
				bad_code_that_means_shoot_is_pressed = false;
				break;
			default:
				break;
			}
		}
		//I click on the mouse to move the kid to any position on the screen. This is for testing purposes.
		if (g_event.type == SDL_MOUSEBUTTONDOWN && g_debug_mode_on)
		{
			int x_pos = 0;
			int y_pos = 0;
			SDL_GetMouseState(&x_pos, &y_pos);
			the_kid->new_location_x(Game::current_world->current_position_x() * g_screen_width + ((double)x_pos * ((double)g_screen_width / (double)g_base_screen_width)));
			the_kid->new_location_y(Game::current_world->current_position_y() * g_screen_height + (double)y_pos * ((double)g_screen_height / (double)g_base_screen_height));
		}
	}
}

//I think maybe in logic, every member in MainGame should be used for something. What?
void MainGame::logic()
{
	if (g_debug_mode_on) {
		int x_pos = 0;
		int y_pos = 0;
		SDL_GetMouseState(&x_pos, &y_pos);
		string x_position = to_string(x_pos) + "      " + to_string(x_pos / 16) + "      " + to_string((x_pos / 16) * 16) + "     " + to_string(x_pos + current_world->current_position_x() * g_screen_width);
		string y_position = to_string(y_pos) + "      " + to_string(y_pos / 16) + "      " + to_string((y_pos / 16) * 16) + "     " + to_string(y_pos + current_world->current_position_y() * g_screen_height);
		mouse_position_x_->change_the_text(x_position.c_str());
		mouse_position_y_->change_the_text(y_position.c_str());
	}
	bullet_buffer_ -= g_timer.deltaT();
	//Create bullet if a bullet is loaded
	if (bullet_buffer_ <= 0 && the_kid->loaded_bullet())
	{
		Mix_PlayChannel(-1, Sound::shoot, 0);
		the_bullets.push_back(new Bullet(the_kid->bullet_entryway(), the_kid->direction()));
		the_kid->remove_bullet();
		bullet_buffer_ = 100;
	}
	//Stops the time on the end screen.
	if (current_world->world_number() != 1)
		save_file->add_time(g_timer.deltaT());
	update_timer_display();
	//Special game event handling
	camera->logic();
	background_music->logic();
	current_screen->screen_logic();
	current_world->world_logic();
	for_each(the_bullets.begin(), the_bullets.end(), bullet_world_collision);
}

void MainGame::render()
{
	SDL_RenderClear(g_renderer);
	current_world->render_background(); //Do this first for the background image
	current_screen->render_background();
	current_world->render_tile_map();
	if (!the_kid->dead())
	{
		the_kid->pick_an_animation();
		the_kid->draw_to_screen();
	}
	for_each(the_bullets.begin(), the_bullets.end(), mem_fun(&Displayable_Object::draw_to_screen));
	current_screen->render_foreground();
	current_world->render_foreground();
	//Render the collision boundaries for testing purposes. 
	//Press F1 to activate or deactivate
	if (show_hazard_boundaries && g_debug_mode_on)
	{
		SDL_SetRenderDrawColor(g_renderer, 0xFF, g_default_green, g_default_blue, g_default_alpha);
		current_screen->show_collision_boundaries();
		current_world->show_collision_boundaries();
		for_each(the_bullets.begin(), the_bullets.end(), mem_fun(&Bullet::render_bullet_box));
		//timer_display->render_outline();
		Game::the_kid->render_hit_box();
		SDL_SetRenderDrawColor(g_renderer, g_default_red, g_default_green, g_default_blue, g_default_alpha);

	}
	boss_health_bar->render();
	if (g_debug_mode_on && show_hazard_boundaries) {
		mouse_position_x_->simple_render();
		mouse_position_y_->simple_render();
	}
	if (the_kid->dead())
	{
		game_over_text->logic();
		game_over_text->draw_to_screen();
	}
	//star HUD logic
	if (save_file->has_star())
		star_hud_filled_->simple_render();
	//else
	//	star_hud_empty_->simple_render();
	//teleporter HUD logic
	if (save_file->has_teleporter())
		teleporter_hud_filled_->simple_render();
	//else
	//	teleporter_hud_empty_->simple_render();
	death_counter_shadow->simple_render();
	death_counter_text->simple_render();
	timer_display_shadow->simple_render();
	timer_display->simple_render();
	SDL_RenderPresent(g_renderer);
}

void MainGame::finish()
{
	current_screen->delete_objects_if_ready();
	current_world->delete_objects_if_ready();
	int counter = 0;
	the_bullets.erase(remove_if(
		the_bullets.begin(),
		the_bullets.end(),
		mem_fun(
			&Displayable_Object::delete_if_ready)),
		the_bullets.end()
	);
}
#include "stdafx.h"
#include "Game Event.h"
#include "The Kid.h"
#include "Boss Health.h"

void Game_Event::activate_if_ready()
{
	if (rect_rect_collision(Game::the_kid->kid_hit_box(), event_box_))
		activated_ = true;
}

const double First_Event::PAUSE_CHARACTER_TIME = 200;
const double First_Event::SPAWN_LOCATION_A_X = 258;
const double First_Event::SPAWN_LOCATION_Y = 352;
const double First_Event::SPAWN_LOCATION_B_X = 420;
const double First_Event::SPAWN_LOCATION_C_X = 574;
const double First_Event::SPAWN_LOCATION_D_X = 736;

void First_Event::logic()
{
	timer_ += g_timer.deltaT();
	switch (the_state_)
	{
	case Pause_Character_Movement:
		Game::the_kid->immobilize(500);
		if (timer_ >= PAUSE_CHARACTER_TIME)
		{
			timer_ = 0;
			the_state_ = Close_Doors;
			Game::current_screen->add_a_new_event(new Create_A_Door(7, 24, 8, 50));
			Game::current_screen->add_a_new_event(new Create_A_Door(56, 24, 8, 50));
		}
		break;
	case Close_Doors:
		if (timer_ >= 2000)
		{
			timer_ = 0;
			the_state_ = Spawn_Some_Stuff;
		}
		break;
	case Spawn_Some_Stuff:
		for (int counter = 0; counter != NUMBER_OF_ENEMIES; counter++)
		{
			if (did_it_pass(timer_ - g_timer.deltaT(), timer_, spawn_times[counter]))
			{
				Frog* new_frog = nullptr;

				switch (current_spawn_point)
				{
				case 0:
					current_spawn_point++;
					new_frog = new Frog(SPAWN_LOCATION_A_X, SPAWN_LOCATION_Y);
					break;
				case 1:
					current_spawn_point++;
					new_frog = new Frog(SPAWN_LOCATION_B_X, SPAWN_LOCATION_Y);
					break;
				case 2:
					current_spawn_point++;
					new_frog = new Frog(SPAWN_LOCATION_C_X, SPAWN_LOCATION_Y);
					break;
				case 3:
					current_spawn_point = 0;
					new_frog = new Frog(SPAWN_LOCATION_D_X, SPAWN_LOCATION_Y);
					break;
				}
				Game::current_screen->add_a_new_enemy(new_frog, false, true);
				event_enemies[counter] = new_frog;
				if (counter == NUMBER_OF_ENEMIES - 1)
				{
					timer_ = 0;
					the_state_ = Wait;
				}
			}
		}
		break;
	case Wait:
		for (int x = 0; x != NUMBER_OF_ENEMIES; ++x)
		{
			if (!Game::current_screen->all_enemies_are_dead())
				//This is only okay if nothing is done after the switch statement
				return;
		}
		timer_ = 0;
		the_state_ = Open_Doors;
		Game::current_screen->add_a_new_event(new Close_A_Door(7, 24, 8, 50, '_'));
		Game::current_screen->add_a_new_event(new Close_A_Door(56, 24, 8, 50, '_'));
		break;
	case Open_Doors:
		if (timer_ >= 2000)
		{
			timer_ = 0;
			done_ = true;
		}
		break;
	}
}

void Yellow_Switch_Event::logic()
{
	timer_ += g_timer.deltaT();
	if (did_it_pass(timer_ - g_timer.deltaT(), timer_, time[current_position_]))
	{
		//I will also need to fix this.
		Game::current_world->the_tile_map()->destroy_a_tile(counter_x[current_position_], counter_y[current_position_]);
		Game::current_world->the_tile_map()->create_a_tile(counter_x[current_position_], counter_y[current_position_], 'Y');
		Mix_PlayChannel(-1, Sound::disappearing_block, 0);
		current_position_++;
	}
}

void Barry_Spawn_Event::logic()
{
	Game::background_music->play_this_track_now(Music::boss_music_part_one);
	set_next_track(Music::boss_music_part_two);
	Barry* new_barry = new Barry;
	Game::boss_health_bar->point_to_this_enemy(new_barry);
	Game::current_screen->add_a_new_enemy(new_barry, false, true);
	done_ = true;
}

const double Left_Spring_Pressed_In_Barry_Fight::SPEED = 160;

void Left_Spring_Pressed_In_Barry_Fight::logic()
{
	Game::current_screen->grab_a_smart_hazard(0)->move_x(SPEED * g_timer.deltaT_in_seconds());
	Game::current_screen->grab_a_smart_hazard(1)->move_x(-SPEED * g_timer.deltaT_in_seconds());
	Game::current_screen->grab_a_smart_hazard(2)->move_x(-SPEED * g_timer.deltaT_in_seconds());
	Game::current_screen->grab_a_smart_hazard(3)->move_x(SPEED * g_timer.deltaT_in_seconds());
	timer_ += g_timer.deltaT();
	if (timer_ >= 100)
	{
		timer_ = 0;
		activated_ = false;
		Game::current_screen->grab_a_smart_hazard(0)->new_location_x(32);
		Game::current_screen->grab_a_smart_hazard(1)->new_location_x(64);
		Game::current_screen->grab_a_smart_hazard(2)->new_location_x(912);
		Game::current_screen->grab_a_smart_hazard(3)->new_location_x(976);
	}
}

const double Right_Spring_Pressed_In_Barry_Fight::SPEED = 160;

void Right_Spring_Pressed_In_Barry_Fight::logic()
{
	Game::current_screen->grab_a_smart_hazard(0)->move_x(-SPEED * g_timer.deltaT_in_seconds());
	Game::current_screen->grab_a_smart_hazard(1)->move_x(SPEED * g_timer.deltaT_in_seconds());
	Game::current_screen->grab_a_smart_hazard(2)->move_x(SPEED * g_timer.deltaT_in_seconds());
	Game::current_screen->grab_a_smart_hazard(3)->move_x(-SPEED * g_timer.deltaT_in_seconds());
	timer_ += g_timer.deltaT();
	if (timer_ >= 100)
	{
		timer_ = 0;
		activated_ = false;
		Game::current_screen->grab_a_smart_hazard(0)->new_location_x(16);
		Game::current_screen->grab_a_smart_hazard(1)->new_location_x(80);
		Game::current_screen->grab_a_smart_hazard(2)->new_location_x(928);
		Game::current_screen->grab_a_smart_hazard(3)->new_location_x(960);
	}
}

void Barry_Dead_Event::logic()
{
	timer_ += g_timer.deltaT();
	if (did_it_pass(timer_ - g_timer.deltaT(), timer_, 1))
	{
		for (int x = 1; x < Game::current_world->the_tile_map()->number_of_tiles_x(); ++x)
		{
			//I need to change these values
			Game::current_world->the_tile_map()->destroy_a_tile(x, 34);
			Game::current_world->the_tile_map()->destroy_a_tile(x, 35);
			Game::current_world->the_tile_map()->create_a_tile(x, 35, 'L');
		}
	}
	else if (did_it_pass(timer_ - g_timer.deltaT(), timer_, 200))
	{
		//I need to change these values
		for (int x = 1; x < Game::current_world->the_tile_map()->number_of_tiles_x(); ++x)
		{
			//I need to change these values
			Game::current_world->the_tile_map()->destroy_a_tile(x, 35);
			//Game::current_screen->the_smart_hazards[4]->prepare_to_delete();
		}
	}
}

const double Moon_Spawner::SPAWN_FREQUENCY = 750;
Moon_Spawner::Moon_Spawner() :
	Game_Event(192, 16, 1000, 400),
	timer_(0)
{}

void Moon_Spawner::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= SPAWN_FREQUENCY)
	{
		timer_ -= SPAWN_FREQUENCY;
		Game::current_screen->add_a_new_smart_hazard(new Moon(900, 208, "Moon_Instructions_2.txt"), false, true);
	}
}

void Big_Explosion_Event::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= frequency_)
	{
		timer_ -= frequency_;
		double entry_x = rand() % static_cast<int>(box_.w) + static_cast<int>(box_.x);
		double entry_y = rand() % static_cast<int>(box_.h) + static_cast<int>(box_.y);
		Game::current_screen->add_a_new_smart_object(new Explosion(entry_x, entry_y), false, true);
		number_of_explosions_left_--;
	}
	if (number_of_explosions_left_ == 0)
		done_ = true;
}

Destroy_Bridge_Event::Destroy_Bridge_Event(bool left) :
	Game_Event(0, 0, 0, 0),
	timer_(0),
	position_(0),
	frequency_(30),
	left_(left)
{
	if (left_)
		position_ = 5;
	else
		position_ = 54;
	activated_ = true;
}

void Destroy_Bridge_Event::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= frequency_)
	{
		timer_ -= frequency_;
		if (left_)
			position_++;
		else
			position_--;
		if (left_ && position_ == 53)
			done_ = true;
		if (!left_ && position_ == 6)
			done_ = true;
		Game::current_world->the_tile_map()->destroy_a_tile(position_, 9);
	}
}

void Create_A_Door::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= frequency_)
	{
		timer_ -= frequency_;
		Mix_PlayChannel(-1, Sound::door_open, 0);
		Game::current_world->the_tile_map()->create_a_tile(spot_x_, spot_y_, 'd');
		spot_y_++;
		how_many_--;
		if (how_many_ == 0)
			done_ = true;
	}
}

void Close_A_Door::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= frequency_)
	{
		how_many_--;
		if (how_many_ == 0)
		{
			done_ = true;
			return;
		}
		timer_ -= frequency_;
		Mix_PlayChannel(-1, Sound::door_open, 0);
		Game::current_world->the_tile_map()->destroy_a_tile(spot_x_, spot_y_ + how_many_);
		Game::current_world->the_tile_map()->create_a_tile(spot_x_, spot_y_ + how_many_, tile_id_);
	}
}

const double Frog_Event::PAUSE_CHARACTER_TIME = 200;
const double Frog_Event::SPAWN_LOCATION_A_X = 258;
const double Frog_Event::SPAWN_LOCATION_Y = 52;
const double Frog_Event::SPAWN_LOCATION_B_X = 420;
const double Frog_Event::SPAWN_LOCATION_C_X = 574;
const double Frog_Event::SPAWN_LOCATION_D_X = 736;

//Copied from First_Event::logic()
void Frog_Event::logic()
{
	timer_ += g_timer.deltaT();
	switch (the_state_)
	{
	case Pause_Character_Movement:
		Game::the_kid->immobilize(500);
		if (timer_ >= PAUSE_CHARACTER_TIME)
		{
			timer_ = 0;
			the_state_ = Close_Doors;
			Game::current_screen->add_a_new_event(new Create_A_Door(5, 1, 8, 50));
			Game::current_screen->add_a_new_event(new Create_A_Door(54, 1, 8, 50));
		}
		break;
	case Close_Doors:
		if (timer_ >= 2000)
		{
			timer_ = 0;
			the_state_ = Spawn_Some_Stuff;
		}
		break;
	case Spawn_Some_Stuff:
		for (int counter = 0; counter != NUMBER_OF_ENEMIES; counter++)
		{
			if (did_it_pass(timer_ - g_timer.deltaT(), timer_, spawn_times[counter]))
			{
				Frog* new_frog = nullptr;

				switch (rand() % 4)
				{
				case 0:
					current_spawn_point++;
					new_frog = new Frog(SPAWN_LOCATION_A_X, SPAWN_LOCATION_Y);
					break;
				case 1:
					current_spawn_point++;
					new_frog = new Frog(SPAWN_LOCATION_B_X, SPAWN_LOCATION_Y);
					break;
				case 2:
					current_spawn_point++;
					new_frog = new Frog(SPAWN_LOCATION_C_X, SPAWN_LOCATION_Y);
					break;
				case 3:
					current_spawn_point = 0;
					new_frog = new Frog(SPAWN_LOCATION_D_X, SPAWN_LOCATION_Y);
					break;
				}
				Game::current_screen->add_a_new_enemy(new_frog, false, true);
				event_enemies[counter] = new_frog;
				if (counter == NUMBER_OF_ENEMIES - 1)
				{
					timer_ = 0;
					the_state_ = Wait;
				}
			}
		}
		break;
	case Wait:
		for (int x = 0; x != NUMBER_OF_ENEMIES; ++x)
		{
			if (!Game::current_screen->all_enemies_are_dead())
				//This is only okay if nothing is done after the switch statement
				return;
		}
		timer_ = 0;
		the_state_ = Open_Doors;
		Game::current_screen->add_a_new_event(new Close_A_Door(5, 0, 9, 50, '!'));
		Game::current_screen->add_a_new_event(new Close_A_Door(54, 0, 9, 50, '!'));
		break;
	case Open_Doors:
		if (timer_ >= 2000)
		{
			timer_ = 0;
			done_ = true;
		}
		break;
	}
}

void Falling_Spikes_Screen_1::logic()
{
	timer_ += g_timer.deltaT();
	if (timer_ >= entry_frequency_)
	{
		timer_ -= entry_frequency_;
		Game::current_screen->add_a_new_smart_hazard(new Falling_Spike(entry_pos_x_[rand() % 4], -16), true, true);
	}
}

void Moving_Spikes_Screen::logic()
{
	for (int x = 0; x != 2; x++)
	{
		entry_timer_[x] += g_timer.deltaT();
		if (entry_timer_[x] >= entry_frequency_[x])
		{
			entry_timer_[x] -= entry_frequency_[x];
			Game::current_screen->add_a_new_smart_hazard(new Moving_Spike(entry_pos_x_[x], entry_pos_y_[x], new_velocity[x]), false, true);
		}
	}
}

void Troll_Spike_Platform::logic()
{
	if (rect_rect_collision(activation_box_, Game::the_kid->kid_hit_box()))
	{
		for (int counter = 0; counter != 3; counter++)
			Game::current_screen->grab_a_dumb_hazard(counter)->new_location_y(332);
		for (int counter = 0; counter != 6; counter++)
			Game::current_screen->extra_walls[counter]->new_location_y(364);
	}
	else
	{
		for (int counter = 0; counter != 3; counter++)
			Game::current_screen->grab_a_dumb_hazard(counter)->new_location_y(352);
		for (int counter = 0; counter != 6; counter++)
			Game::current_screen->extra_walls[counter]->new_location_y(384);
	}
}

void Falling_Spikes_Screen_2::logic()
{
	Game::camera->set_rand_offset(5);
	if (Game::the_kid->hit_box_left() >= 7056)
	{
		Game::camera->remove_offset();
		done_ = true;
	}
	for (int x = 0; x != 5; x++)
	{
		entry_timer_[x] += g_timer.deltaT();
		if (entry_timer_[x] >= entry_frequency_[x])
		{
			entry_timer_[x] -= entry_frequency_[x];
			Game::current_screen->add_a_new_smart_hazard(new Falling_Spike(entry_pos_x_[x] - rand() % 128, -16), true, true);
		}
	}
}

End_Game_Action::End_Game_Action() :
	Game_Event(0, 0, 0, 0),
	i_won_(2.5),
	i_won_done_(false),
	fireworks_frequency_(1),
	firework_entry_right_x(g_screen_width - 60),
	firework_entry_left_x(60)
{
	Game::the_kid->push_up(900);
	activated_ = true;
	Mix_PlayChannel(-1, Sound::save, 0);
}

const double Firework::initial_power = 400;

void End_Game_Action::logic()
{
	if (!i_won_done_)
		if (i_won_.countdown_logic())
		{
			Mix_PlayChannel(-1, Sound::i_won, 0);
			i_won_done_ = true;
		}
	if (fireworks_frequency_.frequency_timer_logic())
		Game::current_screen->add_a_new_smart_object(new Firework(rand() % (firework_entry_right_x - firework_entry_left_x) + firework_entry_left_x, 544), true, true);
}

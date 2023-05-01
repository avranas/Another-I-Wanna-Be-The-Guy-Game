#include "stdafx.h"
#include "Worlds and Maps.h"
#include "The Kid.h"
#include "Music and Sound.h"
#include "Save Box.h"
#include <map>

namespace Tiles
{
	Texture* surface_w1 = nullptr;
	Texture* ground_w1 = nullptr;
	Texture* top_left_w1 = nullptr;
	Texture* top_right_w1 = nullptr;
	Texture* left_w1 = nullptr;
	Texture* right_w1 = nullptr;
	Texture* bottom_right_w1 = nullptr;
	Texture* bottom_left_w1 = nullptr;
	Texture* bottom_w1 = nullptr;
	Texture* top_right_corner_w1 = nullptr;
	Texture* top_left_corner_w1 = nullptr;
	Texture* bottom_right_corner_w1 = nullptr;
	Texture* bottom_left_corner_w1 = nullptr;
	Texture* pipe_top_left_tile = nullptr;
	Texture* pipe_top_right_tile = nullptr;
	Texture* pipe_bottom_left_tile = nullptr;
	Texture* pipe_bottom_right_tile = nullptr;
}

namespace Tile_Map
{
	map<char, Texture*> world_1_tiles;
}

map<char, Texture*>* current_tile_map = nullptr;

void load_tiles()
{
	using namespace Tiles;
	surface_w1 = new Texture("Tiles/surface_w1.png", "All Tiles.txt");
	ground_w1 = new Texture("Tiles/ground_w1.png", "All Tiles.txt");
	top_left_w1 = new Texture("Tiles/top_left_w1.png", "All Tiles.txt");
	top_right_w1 = new Texture("Tiles/top_right_w1.png", "All Tiles.txt");
	left_w1 = new Texture("Tiles/left_w1.png", "All Tiles.txt");
	right_w1 = new Texture("Tiles/right_w1.png", "All Tiles.txt");
	bottom_right_w1 = new Texture("Tiles/bottom_right_w1.png", "All Tiles.txt");
	bottom_left_w1 = new Texture("Tiles/bottom_left_w1.png", "All Tiles.txt");
	bottom_w1 = new Texture("Tiles/bottom_w1.png", "All Tiles.txt");
	top_right_corner_w1 = new Texture("Tiles/top_right_corner_w1.png", "All Tiles.txt");
	top_left_corner_w1 = new Texture("Tiles/top_left_corner_w1.png", "All Tiles.txt");
	bottom_right_corner_w1 = new Texture("Tiles/bottom_right_corner_w1.png", "All Tiles.txt");
	bottom_left_corner_w1 = new Texture("Tiles/bottom_left_corner_w1.png", "All Tiles.txt");
	pipe_top_left_tile = new Texture("Tiles/pipe_top_left.png", "All Tiles.txt");
	pipe_top_right_tile = new Texture("Tiles/pipe_top_right.png", "All Tiles.txt");
	pipe_bottom_left_tile = new Texture("Tiles/pipe_bottom_left.png", "All Tiles.txt");
	pipe_bottom_right_tile = new Texture("Tiles/pipe_bottom_right.png", "All Tiles.txt");
	using namespace Tile_Map;
	world_1_tiles['w'] = surface_w1;
	world_1_tiles['s'] = ground_w1;
	world_1_tiles['q'] = top_left_w1;
	world_1_tiles['e'] = top_right_w1;
	world_1_tiles['a'] = left_w1;
	world_1_tiles['d'] = right_w1;
	world_1_tiles['c'] = bottom_right_w1;
	world_1_tiles['z'] = bottom_left_w1;
	world_1_tiles['x'] = bottom_w1;
	world_1_tiles['t'] = top_right_corner_w1;
	world_1_tiles['r'] = top_left_corner_w1;
	world_1_tiles['g'] = bottom_right_corner_w1;
	world_1_tiles['f'] = bottom_left_corner_w1;
	//Default is world 1.
	current_tile_map = &world_1_tiles;
}

Tiled_Map::Tiled_Map(int number_of_screens_x, int number_of_screens_y)
{
	the_tiles.resize(TILES_PER_SCREEN_X * number_of_screens_x, vector<Tile*>(TILES_PER_SCREEN_Y * number_of_screens_y, nullptr));
}

void Tiled_Map::create_a_tile(int x, int y, char type)
{
	double pass_x = x * G_TILE_WIDTH;
	double pass_y = y * G_TILE_HEIGHT;
	//Commented out tiles are from different worlds. If you want to make those worlds\
	work again, you'll have to change the tile code here, and the Screen .txt file 
	switch (type)
	{
	case 'q':case 'w':case 'e':case 'a':case 's':case 'd':case 'z':
	case 'x':case 'c':case 'r':case 't': case 'f': case 'g':
		the_tiles[x][y] = new Tile((*current_tile_map)[type], pass_x, pass_y, false, true);
		break;
	case '_':
		the_tiles[x][y] = new Tile(nullptr, pass_x, pass_y, true, false);
		break;
	case '0':
		the_tiles[x][y] = new Tile(nullptr, pass_x, pass_y, false, false);
		break;
	case '-':
		the_tiles[x][y] = new Tile(nullptr, pass_x, pass_y, false, true);
		break;
	case '1':
		the_tiles[x][y] = new Tile(Pic::blue_tile, pass_x, pass_y, false, true);
		break;
	case '!':
		the_tiles[x][y] = new Tile(Pic::empty_blue_tile, pass_x, pass_y, true, false);
		break;
	case 'F':
		the_tiles[x][y] = new Tile(Pic::blue_tile, pass_x, pass_y, true, false);
		break;
	case 'D':
		the_tiles[x][y] = new Tile(Pic::door_tile_edge, pass_x, pass_y, false, true);
		break;
		//case 'd':
		//	the_tiles[x][y] = new Tile(Pic::door_tile, pass_x, pass_y, false, false);
		//	break;
	case 'Y':
		the_tiles[x][y] = new Tile(Pic::empty_yellow_tile, pass_x, pass_y, true, false);
		break;
	case 'y':
		the_tiles[x][y] = new Tile(Pic::yellow_switch_tile, pass_x, pass_y, false, false);
		break;
	case 'G':
		the_tiles[x][y] = new Tile(Pic::empty_green_tile, pass_x, pass_y, true, false);
		break;
		//case 'g':
		//	the_tiles[x][y] = new Tile(Pic::green_switch_tile, pass_x, pass_y, false, false);
		//	break;
	case 'L':
		the_tiles[x][y] = new Tile(Pic::lava_top_tile, pass_x, pass_y, true, false);
		break;
		//case 'l':
		//	the_tiles[x][y] = new Tile(Pic::lava_tile, pass_x, pass_y, true, false);
		//	break;
	case 'i':
		the_tiles[x][y] = new Tile(Tiles::pipe_top_left_tile, pass_x, pass_y, true, false);
		break;
	case 'o':
		the_tiles[x][y] = new Tile(Tiles::pipe_top_right_tile, pass_x, pass_y, true, false);
		break;
	case 'k':
		the_tiles[x][y] = new Tile(Tiles::pipe_bottom_left_tile, pass_x, pass_y, true, false);
		break;
	case 'l':
		the_tiles[x][y] = new Tile(Tiles::pipe_bottom_right_tile, pass_x, pass_y, true, false);
		break;
	case 'b':
		the_tiles[x][y] = new Tile(Pic::bridge_tile, pass_x, pass_y, false, false);
		break;
	default:
		cout << "There was an error in tile placement. You need to fix this now." << endl;
		cout << "Type: " << type << endl;
		break;
	}
}

void Tiled_Map::read_a_file(int screen_x, int screen_y, string tile_file_name)
{
	cout << "Map_Data/" + tile_file_name << " loaded" << endl;
	ifstream map_data("Map_Data/" + tile_file_name);
	char map_value;
	for (int y = TILES_PER_SCREEN_Y * screen_y; y != (screen_y + 1) * TILES_PER_SCREEN_Y; ++y)
	{
		for (int x = TILES_PER_SCREEN_X * screen_x; x != (screen_x + 1) * TILES_PER_SCREEN_X; ++x)
		{
			map_data >> map_value;
			create_a_tile(x, y, map_value);
		}
	}
}

void Tiled_Map::make_an_empty_map(int screen_x, int screen_y)
{
	for (int y = TILES_PER_SCREEN_Y * screen_y; y != (screen_y + 1) * TILES_PER_SCREEN_Y; ++y)
	{
		for (int x = TILES_PER_SCREEN_X * screen_x; x != (screen_x + 1) * TILES_PER_SCREEN_X; ++x)
		{
			the_tiles[x][y] = new Tile(nullptr, (screen_x * g_screen_width) + (x * G_TILE_WIDTH), (screen_y * g_screen_height) + (y * G_TILE_HEIGHT), true, false);
		}
	}
}


Tiled_Map::~Tiled_Map()
{
	for (size_t y = 0; y != the_tiles[0].size(); ++y)
	{
		for (size_t x = 0; x != the_tiles.size(); ++x)
		{
			delete the_tiles[x][y];
			the_tiles[x][y] = nullptr;
		}
	}
}

void Screen::collision_test_with_enemies(Bullet* the_bullet)
{
	for (auto iter = the_enemies_.begin(); iter != the_enemies_.end(); ++iter)
	{
		(*iter)->bullet_collision(the_bullet);
	}
}

void Screen::screen_logic()
{
	//Maybe you should restructure events so that they all have enum states.\
	since so many events don't even need to be activated.
	for (int counter = 0; counter < the_events_.size(); counter++)
	{
		if (!the_events_[counter]->activated())
			the_events_[counter]->activate_if_ready();
		else
			the_events_[counter]->logic();
	}
	//Smart_Hazard logic
	for_each(the_smart_hazards_.begin(), the_smart_hazards_.end(), mem_fun(&Smart_Hazard::logic));
	//Collision detection with bullets
	for_each(Game::the_bullets.begin(), Game::the_bullets.end(), mem_fun(&Bullet::logic));
	for (auto b_iter = Game::the_bullets.begin(); b_iter != Game::the_bullets.end(); ++b_iter)
	{
		for (auto e_iter = the_enemies_.begin(); e_iter != the_enemies_.end(); ++e_iter)
		{
			if ((*e_iter)->bullet_collision(*b_iter))
				break;
		}
	}
	//Logic of smart objects
	for_each(the_smart_objects_.begin(), the_smart_objects_.end(), mem_fun(&Smart_Object::logic));
	//Extra tile logic
	for_each(extra_walls.begin(), extra_walls.end(), mem_fun(&Tile::logic));
	//Logic of enemies; must be after collision_detection.
	for_each(the_enemies_.begin(), the_enemies_.end(), mem_fun(&Enemy::common_logic));
	//Platform logic. Must be done before Kid logic.
	for_each(the_special_platforms.begin(), the_special_platforms.end(), mem_fun(&Special_Platform::logic));
	//Particle logic. Must be done before Kid logic.
	for_each(the_particles_.begin(), the_particles_.end(), mem_fun(&Particle::particle_logic));
	//Make particles if available
	for (auto e_iter = the_enemies_.begin(); e_iter != the_enemies_.end(); ++e_iter)
	{
		for (auto p_iter = (*e_iter)->the_parts_.begin(); p_iter != (*e_iter)->the_parts_.end(); p_iter++)
		{
			if (!(*p_iter)->particle_generator()->empty())
			{
				add_a_new_particle((*p_iter)->particle_generator()->grab_and_remove_back_particle(),
					(*p_iter)->particle_generator()->add_to_front(), false);
			}
		}
	}
	//The following is only done if the_kid is alive. If you want to add something for the game do to while the\
		kid is dead, be sure to do it before this point.
	if (Game::the_kid->dead())
	{
		return;
	}
	for (int counter = 0; counter != the_enemies_.size(); counter++)
	{
		if (the_enemies_[counter]->kid_collision())
		{
			Game::the_kid->kill_the_kid();
			return;
		}
	}
	for (int counter = 0; counter != the_smart_hazards_.size(); counter++)
	{
		if (the_smart_hazards_[counter]->test_collision_with_the_kid())
		{
			Game::the_kid->kill_the_kid();
		}
	}
	for (int counter = 0; counter != the_dumb_hazards_.size(); counter++)
	{
		if (the_dumb_hazards_[counter]->test_collision_with_the_kid()) {
			Game::the_kid->kill_the_kid();
		}
	}
	Game::the_kid->handle_movement();
}

void Screen::delete_objects_if_ready()
{
	background_objects_.erase(remove_if(
		background_objects_.begin(),
		background_objects_.end(),
		mem_fun(&Displayable_Object::ready_to_delete)),
		background_objects_.end()
	);
	foreground_objects_.erase(remove_if(
		foreground_objects_.begin(),
		foreground_objects_.end(),
		mem_fun(&Displayable_Object::ready_to_delete)),
		foreground_objects_.end()
	);
	the_events_.erase(remove_if(
		the_events_.begin(),
		the_events_.end(),
		mem_fun(&Game_Event::done)),
		the_events_.end()
	);
	the_smart_hazards_.erase(remove_if(
		the_smart_hazards_.begin(),
		the_smart_hazards_.end(),
		mem_fun(&Displayable_Object::delete_if_ready)),
		the_smart_hazards_.end()
	);
	the_dumb_hazards_.erase(remove_if(
		the_dumb_hazards_.begin(),
		the_dumb_hazards_.end(),
		mem_fun(&Displayable_Object::delete_if_ready)),
		the_dumb_hazards_.end()
	);
	the_enemies_.erase(remove_if(
		the_enemies_.begin(),
		the_enemies_.end(),
		mem_fun(&Enemy::delete_enemy_if_ready)),
		the_enemies_.end()
	);
	the_smart_objects_.erase(remove_if(
		the_smart_objects_.begin(),
		the_smart_objects_.end(),
		mem_fun(&Displayable_Object::delete_if_ready)),
		the_smart_objects_.end()
	);
	the_special_platforms.erase(remove_if(
		the_special_platforms.begin(),
		the_special_platforms.end(),
		mem_fun(&Displayable_Object::delete_if_ready)),
		the_special_platforms.end()
	);
	extra_walls.erase(remove_if(
		extra_walls.begin(),
		extra_walls.end(),
		mem_fun(&Displayable_Object::delete_if_ready)),
		extra_walls.end()
	);
	the_particles_.erase(remove_if(
		the_particles_.begin(),
		the_particles_.end(),
		mem_fun(&Displayable_Object::delete_if_ready)),
		the_particles_.end()
	);
}

void World::delete_objects_if_ready()
{
	background_objects_.erase(remove_if(
		background_objects_.begin(),
		background_objects_.end(),
		mem_fun(&Displayable_Object::ready_to_delete)),
		background_objects_.end()
	);
	foreground_objects_.erase(remove_if(
		foreground_objects_.begin(),
		foreground_objects_.end(),
		mem_fun(&Displayable_Object::ready_to_delete)),
		foreground_objects_.end()
	);
	world_smart_hazards_.erase(remove_if(
		world_smart_hazards_.begin(),
		world_smart_hazards_.end(),
		mem_fun(&Displayable_Object::delete_if_ready)),
		world_smart_hazards_.end()
	);
	world_enemies_.erase(remove_if(
		world_enemies_.begin(),
		world_enemies_.end(),
		mem_fun(&Enemy::delete_enemy_if_ready)),
		world_enemies_.end()
	);
	world_smart_objects_.erase(remove_if(
		world_smart_objects_.begin(),
		world_smart_objects_.end(),
		mem_fun(&Displayable_Object::delete_if_ready)),
		world_smart_objects_.end()
	);
	world_special_platforms_.erase(remove_if(
		world_special_platforms_.begin(),
		world_special_platforms_.end(),
		mem_fun(&Displayable_Object::delete_if_ready)),
		world_special_platforms_.end()
	);
	world_extra_walls_.erase(remove_if(
		world_extra_walls_.begin(),
		world_extra_walls_.end(),
		mem_fun(&Displayable_Object::delete_if_ready)),
		world_extra_walls_.end()
	);
}


void Tile::dislodge()
{
	Game::current_screen->add_a_new_smart_object(new Spinning_Tile(this), false, true);
	Game::current_world->the_tile_map()->destroy_a_tile(static_cast<int>(location_.x / G_TILE_WIDTH), static_cast<int>(location_.y / G_TILE_HEIGHT));
}

void Tiled_Map::dislodge_tiles_in_box(Rect* in_here)
{
	int start_x = static_cast<int>(in_here->x / G_TILE_WIDTH);
	int start_y = static_cast<int>(in_here->y / G_TILE_HEIGHT);
	int end_x = static_cast<int>(in_here->x_right() / G_TILE_WIDTH);
	int end_y = static_cast<int>(in_here->y_bottom() / G_TILE_HEIGHT);
	if (start_x < 0)
	{
		start_x = 0;
	}
	if (start_y < 0)
	{
		start_y = 0;
	}
	if (start_x >= static_cast<int>(the_tiles.size()))
	{
		start_x = static_cast<int>(the_tiles.size());
	}
	if (start_y >= static_cast<int>(the_tiles[0].size()))
	{
		start_y = static_cast<int>(the_tiles[0].size());
	}
	if (end_x >= static_cast<int>(the_tiles.size()))
	{
		end_x = static_cast<int>(the_tiles.size());
	}
	if (end_x < 0)
	{
		end_x = 0;
	}
	if (end_y >= static_cast<int>(the_tiles[0].size()))
	{
		end_y = static_cast<int>(the_tiles[0].size());
	}
	if (end_y < 0)
	{
		end_y = 0;
	}
	bool collision_found = false;
	for (int x = start_x; x < end_x; ++x)
	{
		for (int y = start_y; y < end_y; ++y)
		{
			if (!the_tiles[x][y]->passable())
			{
				the_tiles[x][y]->dislodge();
				collision_found = true;
			}
		}
	}
	if (collision_found)
	{
		Mix_PlayChannel(-1, Sound::thunderbolt, 0);
	}
}

void Screen::show_collision_boundaries()
{
	for_each(the_enemies_.begin(), the_enemies_.end(), mem_fun(&Enemy::draw_collision_boundaries));
	for_each(the_smart_hazards_.begin(), the_smart_hazards_.end(), mem_fun(&Smart_Hazard::draw_collision_data));
	for_each(the_dumb_hazards_.begin(), the_dumb_hazards_.end(), mem_fun(&Hazard::draw_collision_data));
}

void World::show_collision_boundaries()
{
	for_each(world_enemies_.begin(), world_enemies_.end(), mem_fun(&Enemy::draw_collision_boundaries));
	for_each(world_smart_hazards_.begin(), world_smart_hazards_.end(), mem_fun(&Smart_Hazard::draw_collision_data));
}

Doorway* Screen::search_for_available_doors()
{
	for (auto doorway_iter = the_doorways_.begin(); doorway_iter != the_doorways_.end(); ++doorway_iter)
	{
		if ((*doorway_iter)->are_we_lined_up(Game::the_kid->kid_hit_box()))
			return(*doorway_iter);
	}
	return nullptr;
}

void Screen::initialize_event(int event_code)
{
	switch (event_code)
	{
	case 1:
		the_events_.push_back(new First_Event);
		break;
	case 2:
		the_events_.push_back(new Barry_Spawn_Event);
		break;
	case 3:
		the_events_.push_back(new Left_Spring_Pressed_In_Barry_Fight);
		break;
	case 4:
		the_events_.push_back(new Right_Spring_Pressed_In_Barry_Fight);
		break;
	case 5:
		the_events_.push_back(new Moon_Spawner);
		break;
	case 6:
		the_events_.push_back(new Frog_Event);
		break;
	case 7:
		the_events_.push_back(new Falling_Spikes_Screen_1);
		break;
	case 8:
		the_events_.push_back(new Moving_Spikes_Screen);
		break;
	case 9:
		the_events_.push_back(new Troll_Spike_Platform);
		break;
	case 10:
		the_events_.push_back(new Falling_Spikes_Screen_2);
		break;
	case 11:
		the_events_.push_back(new End_Game_Action);
		break;
	default:
		cout << "There was a problem initializing events" << endl;
	}
}

Screen::Screen(int world_position_x, int world_position_y, string object_file_name) :
	world_position_x_(world_position_x),
	world_position_y_(world_position_y)
{
	//Type "9001 0 0" into the .txt file to flip this to false;
	bool adding_to_background = true;
	ifstream object_file("Map_Data/" + object_file_name);
	//This function loops through the file until there is nothing left
	int object_value = 0;
	double object_position_x = 0;
	double object_position_y = 0;
	//Only screen objects can be initialized here.
	while (!object_file.eof())
	{
		object_file >> object_value;
		object_file >> object_position_x;
		//In the .txt file, put the obje
		object_position_x += (world_position_x_ * g_screen_width);
		object_file >> object_position_y;
		object_position_y += (world_position_y_ * g_screen_height);
		switch (object_value)
		{
		case 9001:
			adding_to_background = false;
			break;
		case 0:
			// I need this here in case the file is empty.
			break;
		case 1:
		{
			double spike_angle = 0;
			object_file >> spike_angle;
			add_a_new_dumb_hazard(new Spike(object_position_x, object_position_y, spike_angle), adding_to_background, true);
			break;
		}
		case 3:
		{
			double distance = 0;
			double speed = 0;
			double radius = 0;
			object_file >> distance;
			object_file >> speed;
			object_file >> radius;
			for (int counter = 0; counter <= distance / (radius / 2); ++counter)
				add_a_new_smart_object(new Chain(
					object_position_x,
					object_position_y,
					distance - (counter * (radius / 2)),
					speed,
					radius / 4
				), adding_to_background, true);
			//Because the radius of the chain is always a quarter of the size of the saw. Unless I decide to change this.
			add_a_new_smart_hazard(new Spinning_Saw(object_position_x - 40, object_position_y - 40, distance, speed, radius)
				, adding_to_background, true);
			break;
		}
		case 4:
		{
			double distance = 0;
			double speed = 0;
			double radius = 0;
			object_file >> distance;
			object_file >> speed;
			object_file >> radius;
			for (int counter = 0; counter <= distance / (radius / 2); ++counter)
			{
				add_a_new_smart_object(new Pendulum_Chain(
					object_position_x,
					object_position_y,
					distance - (counter * (radius / 2)),
					speed,
					radius / 4), adding_to_background, true);
			}
			add_a_new_smart_hazard(new Pendulum(object_position_x - 40, object_position_y - 40, distance, speed, radius),
				adding_to_background, true);
			break;
		}
		case 555:
			//A save box is added for all difficulties
			add_a_new_platform(new Save_Switch(object_position_x, object_position_y, false), adding_to_background, true);
			break;
		case 55:
			//A save box is only added for normal and easy modes
			if (save_file->difficulty() != 2)
				add_a_new_platform(new Save_Switch(object_position_x, object_position_y, false), adding_to_background, true);
			break;
		case 5:
			//A save box is only added for easy mode
			if (save_file->difficulty() == 0)
				add_a_new_platform(new Save_Switch(object_position_x, object_position_y, true), adding_to_background, true);
			break;
		case 6:
		{
			string file_name;
			double width = 0;
			double height = 0;
			object_file >> width;
			object_file >> height;
			object_file >> file_name;
			add_a_new_platform(new Moving_Platform(object_position_x, object_position_y, width, height, file_name),
				adding_to_background, true);
			break;
		}
		case 7:
		{
			string file_name;
			double width = 0;
			double height = 0;
			object_file >> width;
			object_file >> height;
			add_a_new_platform(new Falling_Platform(object_position_x, object_position_y, width, height),
				adding_to_background, true);
			break;
		}
		case 8:
		{
			double spike_angle = 0;
			object_file >> spike_angle;
			Rect activation_box;
			object_file >> activation_box.x;
			object_file >> activation_box.y;
			object_file >> activation_box.w;
			object_file >> activation_box.h;
			activation_box.x += (world_position_x * g_screen_width);
			activation_box.y += (world_position_y * g_screen_height);
			double shaking_time;
			object_file >> shaking_time;
			double moving_angle;
			object_file >> moving_angle;
			double velocity;
			object_file >> velocity;
			add_a_new_smart_hazard(new Troll_Spike(object_position_x, object_position_y, spike_angle, activation_box, shaking_time, moving_angle, velocity),
				adding_to_background, true);
			break;
		}
		case 9:
		{
			string file_name;
			object_file >> file_name;
			add_a_new_doorway(new Doorway(object_position_x, object_position_y, file_name),
				adding_to_background, true);
			break;
		}
		case 10:
			add_a_new_smart_object(new Gate(object_position_x, object_position_y), adding_to_background, true);
			break;
		case 11:
			add_a_new_enemy(new Carock(static_cast<int>(object_position_x), object_position_y), adding_to_background, true);
			break;
		case 12:
			add_a_new_dumb_object(new Displayable_Object(object_position_x, object_position_y, Pic::doorway_open), adding_to_background, true);
			break;
		case 13:
			add_a_new_smart_object(new Axe(object_position_x, object_position_y), adding_to_background, true);
			break;
		case 14:
		{
			double jump_power = 0;
			double jump_frequency = 0;
			double jump_height = 0;
			double timer_delay = 0;
			object_file >> jump_power;
			object_file >> jump_frequency;
			object_file >> jump_height;
			object_file >> timer_delay;
			add_a_new_platform(new Leaping_Platform(object_position_x, object_position_y, jump_power, jump_frequency, jump_height, timer_delay),
				adding_to_background, true);
			break;
		}
		case 15:
			add_a_new_dumb_object(new Displayable_Object(object_position_x, object_position_y, Pic::hole_in_the_wall),
				adding_to_background, true);
			break;
		case 16:
		{
			double jump_power = 0;
			double distance;
			double speed;
			bool explodes = false;
			object_file >> jump_power;
			object_file >> distance;
			object_file >> speed;
			object_file >> explodes;
			add_a_new_platform(new Spring(object_position_x, object_position_y, jump_power, distance, speed, explodes),
				adding_to_background, true);
			break;
		}
		case 17:
			add_a_new_dumb_object(new Displayable_Object(object_position_x, object_position_y, Pic::controller_warning),
				adding_to_background, true);
			break;
		case 18: //This text box only shows up in Easy mode
		{
			int width;
			int height;
			object_file >> width;
			object_file >> height;
			string new_text;
			getline(object_file, new_text);
			if (save_file->difficulty() != 0)
				break;
			//the_text_boxes.push_back(new Text_Box(object_position_x, object_position_y, height, width, new_text, Font::Calibri,
			//	SDL_Color(0xFF, 0xFF, 0xFF, 0x00)));
			break;
		}
		case 19: //This text box only shows up in Normal mode
		{
			int width;
			int height;
			object_file >> width;
			object_file >> height;
			string new_text;
			getline(object_file, new_text);
			if (save_file->difficulty() != 1)
				break;
			//the_text_boxes.push_back(new Text_Box(object_position_x, object_position_y, height, width, new_text, Font::Calibri,
			//	SDL_Color(0xFF, 0xFF, 0xFF, 0x00)));
			break;
		}
		case 20: //This text box only shows up on Hard mode
		{
			int width;
			int height;
			object_file >> width;
			object_file >> height;
			string new_text;
			getline(object_file, new_text);
			if (save_file->difficulty() != 2)
				break;
			//the_text_boxes.push_back(new Text_Box(object_position_x, object_position_y, height, width, new_text, Font::Calibri,
			//	SDL_Color(0xFF, 0xFF, 0xFF, 0x00)));
			break;
		}
		case 21:
		{
			double max_positions;
			double current_position;
			object_file >> max_positions;
			object_file >> current_position;
			add_a_new_platform(new Trolley(object_position_x, object_position_y, max_positions, current_position),
				adding_to_background, true);
			break;
		}
		case 22:
		{
			double speed;
			double size;
			object_file >> speed;
			object_file >> size;
			add_a_new_smart_hazard(new Windmill(object_position_x, object_position_y, speed, size),
				adding_to_background, true);
			break;
		}
		case 23:
			add_a_new_smart_object(new Link(object_position_x, object_position_y), adding_to_background, true);
			break;
		case 24:
		{
			int frequency;
			int speed;
			object_file >> frequency;
			object_file >> speed;
			add_a_new_enemy(new Pounder(object_position_x, object_position_y, frequency, speed), adding_to_background, true);
			break;
		}
		case 25:
		{
			int type;
			object_file >> type;
			switch (type)
			{
			case 1:
				add_a_new_platform(new One_Way_Platform(object_position_x, object_position_y, 176, 16, Pic::one_way_platform_1),
					adding_to_background, true);
				break;
			default:
				cout << "Error: No platform will be made." << endl;
				break;
			}
			break;
		}
		case 26:
		{
			Rect activation_box;
			object_file >> activation_box.x;
			object_file >> activation_box.y;
			object_file >> activation_box.w;
			object_file >> activation_box.h;
			add_a_new_smart_hazard(new Spike_Wall(object_position_x, object_position_y, activation_box), adding_to_background, true);
			break;
		}
		case 27:
			add_a_new_smart_hazard(new Missile(object_position_x, object_position_y), adding_to_background, true);
			break;
		case 28:
		{
			bool flip = false;
			object_file >> flip;
			add_a_new_smart_object(new Metroid_Door(object_position_x, object_position_y, bool_to_SDL_RendererFlip(flip)), adding_to_background, true);
			//The door back will always add to the foreground whether you want it to or not.
			if (!flip)
			{
				add_a_new_dumb_object(new Displayable_Object(object_position_x + 8, object_position_y, Pic::metroid_door_back), false, true);
			}
			else
			{
				add_a_new_dumb_object(new Displayable_Object(object_position_x - 48, object_position_y, Pic::metroid_door_back), false, true);
			}
			break;
		}
		case 29:
		{
			string file_name;
			double width = 0;
			double height = 0;
			object_file >> width;
			object_file >> height;
			object_file >> file_name;
			add_a_new_platform(new Moving_Platform_2(object_position_x, object_position_y, width, height, file_name),
				adding_to_background, true);
			break;
		}
		//30 uses the pic_map
		case 30:
		{
			int pic_id;
			//Width and height need to be the same as the pic being used.
			double width;
			double height;
			object_file >> pic_id;
			object_file >> width;
			object_file >> height;
			add_a_new_extra_wall(new Tile(pic_map[pic_id], object_position_x, object_position_y, width, height, false, true),
				adding_to_background, true);
			break;
		}
		//31 uses the tile_map map for the current_world
		case 31:
		{
			char pic_id;
			object_file >> pic_id;
			add_a_new_extra_wall(new Tile((*current_tile_map)[pic_id], object_position_x, object_position_y, G_TILE_WIDTH, G_TILE_HEIGHT, false, true),
				adding_to_background, true);
			break;
		}
		case 32:
		{
			double angle;
			double distance;
			double frequency;
			double init_percent_thru_cycle;
			object_file >> angle;
			object_file >> distance;
			object_file >> frequency;
			object_file >> init_percent_thru_cycle;
			add_a_new_smart_hazard(new Poking_Spike(object_position_x, object_position_y, angle, distance, frequency, init_percent_thru_cycle),
				adding_to_background, true);
			break;
		}
		case 33:
		{
			bool spike_on_top;
			double distance;
			double frequency;
			double init_percent_thru_cycle;
			object_file >> spike_on_top;
			object_file >> distance;
			object_file >> frequency;
			object_file >> init_percent_thru_cycle;
			double angle;
			if (!spike_on_top)
			{
				angle = 0;
				add_a_new_smart_hazard(new Poking_Spike(object_position_x, object_position_y - 32, angle, distance, frequency, init_percent_thru_cycle), adding_to_background, true);
			}
			else
			{
				angle = 180;
				add_a_new_smart_hazard(new Poking_Spike(object_position_x - 1, object_position_y + 334, angle, distance, frequency, init_percent_thru_cycle), adding_to_background, true);
			}
			add_a_new_extra_wall((new Spike_Pillar_Base(object_position_x, object_position_y, distance, frequency, init_percent_thru_cycle)), adding_to_background, true);
			break;
		}
		//34 is the background. 1 = pic map id, 2 = x on file, 3 = y on file , 4 = width, 5 = height,
		//This will (hopefully) create a Cool_Background!
		case 3434:
			for (int x = 0; x <= g_screen_width + 64; x += 64)
			{
				for (int y = 0; y <= g_screen_height + 64; y += 31)
				{
					add_a_new_smart_object(new Cool_Background_Tile(x + object_position_x, y + object_position_y, Pic::title_background, -64, 586), true, false);
				}
			}
		case 3535:
			add_a_new_smart_object(new Cool_Background_Tile(object_position_x, object_position_y, Pic::title_background, 336, 521), true, false);
			break;
		case 35:
			add_a_new_dumb_object(new Displayable_Object(object_position_x, object_position_y, Pic::pipe), false, true);
			break;
		case 36:
		{
			double width = 0;
			double height = 0;
			int world = 0;
			int screen_x = 0;
			int screen_y = 0;
			int position_x = 0;
			int position_y = 0;
			object_file >> width;
			object_file >> height;
			object_file >> world;
			object_file >> screen_x;
			object_file >> screen_y;
			object_file >> position_x;
			object_file >> position_y;
			Rect new_rect = Rect(object_position_x, object_position_y, width, height);
			magic_teleport_boxes.push_back(new Magic_Teleport_Box(new_rect, world, screen_x, screen_y,
				position_x, position_y));
			break;
		}
		case 37:
			add_a_new_dumb_object(new Displayable_Object(object_position_x, object_position_y, Pic::big_pipe), false, true);
			break;
		case 38:
			add_a_new_dumb_object(new Displayable_Object(object_position_x, object_position_y, Pic::reverse_big_pipe), false, true);
			break;
		case 39:
			add_a_new_smart_object(new Alex(object_position_x, object_position_y), adding_to_background, false);
			break;
		case 1000:
		{
			int event_code = 0;
			object_file >> event_code;
			initialize_event(event_code);
			//the_events.push_back(new First_Event(object_position_x, object_position_y));
			break;
		}
		case 1001:
			add_a_new_platform(new Yellow_Switch(object_position_x, object_position_y), adding_to_background, true);
			break;
		case 1002:
			add_a_new_dumb_hazard(new Lava(object_position_x, object_position_y), adding_to_background, true);
			break;
		case 1003:
			add_a_new_platform(new Green_Switch(object_position_x, object_position_y), adding_to_background, true);
			break;
		case 1004:
		{
			bool left;
			object_file >> left;
			add_a_new_smart_object(new Mini_Axe(object_position_x, object_position_y, left), adding_to_background, true);
			break;
		}
		case 1006:
			add_a_new_dumb_object(new Displayable_Object(object_position_x, object_position_y, Pic::house), adding_to_background, true);
			break;
		case 1007:
			add_a_new_smart_hazard(new Crushing_Spikes(object_position_x, object_position_y), true, true);
			break;
		case 1009:
			add_a_new_smart_hazard(new Rainbow_Spike(object_position_x, object_position_y), true, true);
			break;
		case 1010:
			add_a_new_dumb_object(new Displayable_Object(object_position_x, object_position_y, Pic::z_key), true, true);
			break;
		case 1011:
			add_a_new_dumb_object(new Displayable_Object(object_position_x, object_position_y, Pic::x_key), true, true);
			break;
		case 1012:
		{
			double angle;
			object_file >> angle;
			add_a_new_dumb_object(new Displayable_Object(object_position_x, object_position_y, Pic::arrow_key, angle), true, true);
			break;
		}
		case 1013:
		{
			Super_Carock* new_enemy = new Super_Carock;
			add_a_new_enemy(new_enemy, true, true);
			add_a_new_smart_object(new Mushroom(10112, 544, new_enemy), true, true);
			vector<Point> end_points;
			end_points.push_back(Point(10096, 624));
			end_points.push_back(Point(10016, 704));
			end_points.push_back(Point(9936, 784));
			end_points.push_back(Point(9856, 864));
			end_points.push_back(Point(9776, 944));
			int counter = 0;
			for (double angle = 0; angle <= 288; angle += 72)
			{
				object_file >> angle;
				add_a_new_platform(new Boss_Platform(end_points[counter], angle, new_enemy), true, false);
				counter++;
			}
			break;
		}
		case 1014:
			add_a_new_smart_object(new Fake_Mushroom(object_position_x, object_position_y), true, false);
			break;
		case 1015:
			if (!save_file->has_star())
			{
				add_a_new_smart_object(new Star(object_position_x, object_position_y), true, false);
			}
			break;
		case 1016:
			add_a_new_platform(new Super_Spring(object_position_x, object_position_y, 2000), true, false);
			break;
		case 1017:
			if (!save_file->has_teleporter())
			{
				add_a_new_smart_object(new Teleporter(object_position_x, object_position_y), true, false);
			}
			break;
		case 1018:
			add_a_new_smart_object(new Congrats(object_position_x, object_position_y), false, false);
			break;
		case 1019:
			add_a_new_smart_object(new Portal(object_position_x, object_position_y), true, false);
			break;
		case 1020:
		{
			double width;
			double height;
			object_file >> width;
			object_file >> height;
			if (save_file->difficulty() == 0)
			{
				Tile* new_tile = new Tile(Pic::no_pic, object_position_x, object_position_y, width, height, false, true);
				add_a_new_extra_wall(new_tile, false, true);
			}
			break;
		}
		case 1021:
			add_a_new_smart_hazard(new Rising_Spikes(object_position_x, object_position_y), true, false);
			break;
		case 1022:
			add_a_new_dumb_object(new Displayable_Object(object_position_x, object_position_y, Pic::nope), true, false);
			break;
		default:
			cout << "Unable to place an object correctly! :(" << endl;
			cout << "Object value" << object_value << endl;
			break;
		}
	}
}

Screen::~Screen()
{
	the_events_.clear();
	for (auto render_iter = foreground_objects_.begin(); render_iter != foreground_objects_.end();)
	{
		auto erase_this = render_iter;
		render_iter++;
		delete (*erase_this);
		(*erase_this) = nullptr;
		foreground_objects_.erase(erase_this);
	}
	for (auto render_iter = background_objects_.begin(); render_iter != background_objects_.end();)
	{
		auto erase_this = render_iter;
		render_iter++;
		delete (*erase_this);
		(*erase_this) = nullptr;
		background_objects_.erase(erase_this);
	}
	for (auto counter = 0; counter != magic_teleport_boxes.size(); counter++)
	{
		delete magic_teleport_boxes[counter];
		magic_teleport_boxes[counter] = nullptr;
	}
}

void World::add_world_objects(string file_name)
{
	cout << "Map_Data/" + file_name << " loaded" << endl;
	fstream object_file("Map_Data/" + file_name);
	int object_value;
	double object_position_x;
	double object_position_y;
	while (!object_file.eof())
	{
		object_file >> object_value;
		object_file >> object_position_x;
		object_file >> object_position_y;
		switch (object_value)
		{
		case 0:
			break;
		case 1000:
			add_a_new_smart_hazard(new Chasing_Spikes(object_position_x, object_position_y), true, true);
			break;
		case 2:
		{
			string file_name;
			object_file >> file_name;
			add_a_new_smart_hazard(new Moon(object_position_x, object_position_y, file_name), false, true);
			break;
		}
		case 3:
			add_a_new_enemy(new Carock(static_cast<int>(object_position_x), object_position_y), false, true);
			break;
		case 34:
		{
			Rect box_on_file;
			double move_this_much_x = 0;
			double move_this_much_y = 0;
			box_on_file.x = object_position_x;
			box_on_file.y = object_position_y;
			object_file >> box_on_file.w;
			object_file >> box_on_file.h;
			object_file >> move_this_much_x;
			object_file >> move_this_much_y;
			string file_name;
			object_file >> file_name;
			background_image_ = new Background("Images/" + file_name, box_on_file, move_this_much_x, move_this_much_y);
			break;
		}
		default:
			cout << "Error in World::add_world_objects()" << endl;
			cout << "object_value: " << object_value << endl;
		}
	}
}

World::World(string world_data_file_name, int starting_position_x, int starting_position_y) :
	current_position_x_(starting_position_x),
	current_position_y_(starting_position_y),
	background_image_(nullptr)
{
	ifstream world_data("Map_Data/" + world_data_file_name);
	//First input the number of maps in the x direction... 0 cells means one.
	world_data >> world_number_;
	//This means all world_numbers == track numbers. Hopefully I don't ever have to change this for some reason.
	Game::background_music->switch_to_track_number(world_number_);
	if (world_number_ == -842150451)
		cout << "There's a problem loading world data '" << world_data_file_name << endl;
	//number of cells in y direction
	world_data >> x_number_of_cells_;
	world_data >> y_number_of_cells_;
	//Now that we know how many cells we'll have. Push back the_screens_ with nullptrs
	for (int y = 0; y != y_number_of_cells_; ++y)
	{
		vector<Screen*> new_vector;
		for (int x = 0; x != x_number_of_cells_; ++x)
		{
			new_vector.push_back(nullptr);
		}
		the_screens_.push_back(new_vector);
	}
	string world_object_file_name;
	world_data >> world_object_file_name;
	cout << world_object_file_name << endl;
	add_world_objects(world_object_file_name);
	the_tile_map_ = new Tiled_Map(x_number_of_cells_, y_number_of_cells_);
	int cell_x = 0;
	int cell_y = 0;
	string tile_file_name;
	string object_file_name;
	while (!world_data.eof())
	{
		world_data >> cell_x;
		world_data >> cell_y;
		world_data >> tile_file_name;
		world_data >> object_file_name;
		cout << object_file_name << endl;
		the_tile_map_->read_a_file(cell_x, cell_y, tile_file_name);
		the_screens_[cell_y][cell_x] = new Screen(cell_x, cell_y, object_file_name);
	}
	//Fill up the remaining uninitialized cells with a map with no tiles in it:
	for (int y = 0; y != y_number_of_cells_; ++y)
	{
		for (int x = 0; x != x_number_of_cells_; ++x)
		{
			if (the_screens_[y][x] == nullptr)
			{
				the_screens_[y][x] = new Screen(x, y, "Empty_File.txt");
				the_tile_map_->make_an_empty_map(x, y);
			}
		}
	}
	world_box_.x = 0;
	world_box_.y = 0;
	world_box_.w = x_number_of_cells_ * g_screen_width;
	world_box_.h = y_number_of_cells_ * g_screen_height;
}

World::~World()
{
	for (auto render_iter = foreground_objects_.begin(); render_iter != foreground_objects_.end();)
	{
		auto erase_this = render_iter;
		render_iter++;
		delete (*erase_this);
		(*erase_this) = nullptr;
		foreground_objects_.erase(erase_this);
	}
	for (auto render_iter = background_objects_.begin(); render_iter != background_objects_.end();)
	{
		auto erase_this = render_iter;
		render_iter++;
		delete (*erase_this);
		(*erase_this) = nullptr;
		background_objects_.erase(erase_this);
	}
	for (int y = 0; y != y_number_of_cells_; ++y)
	{
		for (int x = 0; x != x_number_of_cells_; ++x)
		{
			delete the_screens_[y][x];
			the_screens_[y][x] = nullptr;
		}
	}
	delete the_tile_map_;
	the_tile_map_ = nullptr;
}

void World::switch_to_this_screen(int x, int y)
{
	if (background_image_ != nullptr)
	{
		background_image_->shift(x, y);
	}
	current_position_x_ = x;
	current_position_y_ = y;
	Game::current_screen = the_screens_[current_position_y_][current_position_x_];
	Game::camera->remove_offset();
	//Whenever you switch to another screen, the bullets should disappear
	Game::the_bullets.clear();
}

void World::left_one_screen()
{
	if (current_position_x_ - 1 == -1)
	{
		Game::the_kid->kill_the_kid();
		return;
	}
	current_position_x_--;
	switch_to_this_screen(current_position_x_, current_position_y_);
	Game::camera->set_main_position_x(Game::camera->x_main() - g_screen_width);
}

void World::right_one_screen()
{
	if (current_position_x_ + 1 == x_number_of_cells_)
	{
		Game::the_kid->kill_the_kid();
		return;
	}
	current_position_x_++;
	switch_to_this_screen(current_position_x_, current_position_y_);
	Game::camera->set_main_position_x(Game::camera->x_main() + g_screen_width);
}

void World::up_one_screen()
{
	if (current_position_y_ - 1 == -1)
	{
		Game::the_kid->kill_the_kid();
		return;
	}
	current_position_y_--;
	switch_to_this_screen(current_position_x_, current_position_y_);
	Game::camera->set_main_position_y(Game::camera->y_main() - g_screen_height);
}

void World::down_one_screen()
{
	if (current_position_y_ + 1 == y_number_of_cells_)
	{
		Game::the_kid->kill_the_kid();
		return;
	}
	current_position_y_++;
	switch_to_this_screen(current_position_x_, current_position_y_);
	Game::camera->set_main_position_y(Game::camera->y_main() + g_screen_height);
}

void World::world_logic()
{
	for_each(world_smart_hazards_.begin(), world_smart_hazards_.end(), mem_fun(&Smart_Hazard::logic));
	//Everything that kills the kid must be done after this point.
	if (Game::the_kid->dead())
	{
		return;
	}
	for (int counter = 0; counter != world_smart_hazards_.size(); counter++)
	{
		if (world_smart_hazards_[counter]->test_collision_with_the_kid())
		{
			Game::the_kid->kill_the_kid();
		}
	}
	//Collision detection with bullets
	for (auto b_iter = Game::the_bullets.begin(); b_iter != Game::the_bullets.end(); ++b_iter)
	{
		for (auto e_iter = world_enemies_.begin(); e_iter != world_enemies_.end(); ++e_iter)
		{
			if ((*e_iter)->bullet_collision(*b_iter))
			{
				break;
			}
		}
	}
	//Logic of enemies; must be after collision_detection.
	for_each(world_enemies_.begin(), world_enemies_.end(), mem_fun(&Enemy::common_logic));
	for (int counter = 0; counter != world_enemies_.size(); counter++)
	{
		if (world_enemies_[counter]->kid_collision())
		{
			Game::the_kid->kill_the_kid();
			return;
		}
	}
}

//I separated this from Bullet::logic() to allow for shooting non-passable walls.
void bullet_world_collision(Bullet* the_bullet)
{
	if (!Game::current_world->the_tile_map()->is_this_spot_passable(the_bullet->middle_of_box()))
	{
		the_bullet->prepare_to_delete();
	}
	for (auto iter = Game::current_screen->extra_walls.begin(); iter != Game::current_screen->extra_walls.end(); ++iter)
	{
		if (rect_rect_collision((*iter)->display_outline(), the_bullet->the_box()))
		{
			the_bullet->prepare_to_delete();
		}
	}
	//For now some boundary checks will do just fine. We'll need this in the game anyway.
	if (the_bullet->location_x() > Game::camera->x_right())
	{
		the_bullet->prepare_to_delete();
	}
	if (the_bullet->location_x() < Game::camera->x())
	{
		the_bullet->prepare_to_delete();
	}
}

void Spike_Pillar_Base::logic()
{
	timer_ += g_timer.deltaT_in_seconds() * frequency_;
	location_.y = starting_location_y_ + sin(timer_) * distance_;
}

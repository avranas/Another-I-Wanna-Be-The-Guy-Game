#ifndef TILED_MAP_H_
#define TILED_MAP_H_
#include <SDL.h>
#include "SDL_image.h"
#include "GameState.h"
#include "timer.h"
#include <string>
#include "globals.h"
#include "Save Box.h"
#include "Hazards.h"
#include "Moving Platform.h"
#include "Doorway.h"
#include "Simple Object.h"
#include <fstream>

const static int G_TILE_WIDTH = 16;
const static int G_TILE_HEIGHT = 16;
const static int G_NUMBER_OF_TILES_X = 80;
const static int G_NUMBER_OF_TILES_Y = 45;

class Tile : public Displayable_Object
{
public:
	Tile(Texture* pic, double x, double y, bool passable) :
		Displayable_Object(x, y),
		passable_(passable)
	{
		if (pic != nullptr)
			pic->init();
		the_pics_.push_back(pic);
	}
	inline double left_of_tile()const { return location_.x; }
	inline double top_of_tile()const { return location_.y; }
	inline double right_of_tile()const { return location_.x + G_TILE_WIDTH; }
	inline double bottom_of_tile()const { return location_.y + G_TILE_HEIGHT; }

	inline bool passable()
	{
		return passable_;
	}

private:
	bool passable_;
};

class Background
{
public:
	Background(Texture* pic) :
		background_image_(pic),
		image_box_(pic->box_on_sprite_sheet(0)),
		repeat_x_(image_box_.w / g_base_screen_width),
		repeat_y_(image_box_.h / g_base_screen_height)
	{}
	inline void render()
	{
		for (int x = 0; repeat_x_; ++x)
		{
			for (int y = 0; y != repeat_y_; ++y)
			{
				SDL_Rect display_this(image_box_.x * x, image_box_.y * y, image_box_.w, image_box_.h);
				//apply_surface(background_image_->pic(), image_box_, display_this);
			}
		}
	}
private:
	Texture* background_image_;
	SDL_Rect image_box_;
	const int repeat_x_;
	const int repeat_y_;
};

class Tiled_Map
{
public:
	Tiled_Map(string file_name);
	~Tiled_Map();
	inline void Show_Map()
	{
		for (int y = 0; y != G_NUMBER_OF_TILES_Y; ++y)
		{
			for (int x = 0; x != G_NUMBER_OF_TILES_X; ++x)
			{
				if (the_tiles[x][y] != nullptr)
				{
					the_tiles[x][y]->render();
				}
			}
		}
	}

	inline Tile* get_tile(int x, int y)
	{
		return the_tiles[x][y];
	}

	inline void destroy_a_tile(int x, int y)
	{
		delete the_tiles[x][y];
		the_tiles[x][y] = nullptr;
		the_tiles[x][y] = new Tile(nullptr, x, y, true);
	}

	inline bool is_this_tile_passable(int x, int y)
	{
		if (x >= G_NUMBER_OF_TILES_X)
		{
			return true;
		}
		if (y >= G_NUMBER_OF_TILES_X)
		{
			return true;
		}
		if (x < 0)
		{
			return true;
		}
		if (y < 0)
		{
			return true;
		}
		return the_tiles[x][y]->passable();
	}

	inline int number_of_tiles_x()
	{
		return G_NUMBER_OF_TILES_X - 1;
	}

	inline int number_of_tiles_y()
	{
		return G_NUMBER_OF_TILES_Y - 1;
	}

private:
	Tile* the_tiles[G_NUMBER_OF_TILES_X][G_NUMBER_OF_TILES_Y];
};

struct Screen
{
	//a 2D vector of "Screens" are constructed when the world is.
	//Send in the screen tile data, then the screen object data
	Screen(string tile_file_name, string object_file_name);
	~Screen();
	vector<Save_Box*> the_save_boxes;
	vector<Hazard*> the_hazards;
	vector<Moving_Platform*> the_moving_platforms;
	vector<Doorway*> the_doorways;
	vector<Simple_Object*> the_simple_objects;
	Tiled_Map* the_tile_map;
};

class World
{
public:
	World(string world_data_file_name, int starting_position_x, int starting_position_y);
	~World();
	void switch_to_this_screen(int y, int x);

	inline void left_one_screen()
	{
		current_position_x_--;
		switch_to_this_screen(current_position_y_, current_position_x_);
	}

	inline void right_one_screen()
	{
		current_position_x_++;
		switch_to_this_screen(current_position_y_, current_position_x_);
	}

	inline void up_one_screen()
	{
		current_position_y_--;
		switch_to_this_screen(current_position_y_, current_position_x_);
	}

	inline void down_one_screen()
	{
		current_position_y_++;
		switch_to_this_screen(current_position_y_, current_position_x_);
	}

	inline int current_position_x()
	{
		return current_position_x_;
	}

	inline int current_position_y()
	{
		return current_position_y_;
	}

	inline int world_number()const
	{
		return world_number_;
	}

private:
	int x_number_of_cells_;
	int y_number_of_cells_;
	vector<vector<Screen*>> the_screens_;
	int current_position_x_;
	int current_position_y_;
	int world_number_;
};

namespace Game
{
	extern World* current_world;
	extern Screen* current_screen;
}

#endif

#include "stdafx.h"
#include "Tiled_Map.h"
#include "The Kid.h"

Tiled_Map::Tiled_Map(std::string file_name)
{
	std::ifstream map_data(file_name);
	char map_value;
	double pass_x;
	double pass_y;
	pass_x = G_TILE_WIDTH * -1;	//With this code, we're adding G_TILE_WIDTH once before we use it so "* -1" is necessary
	pass_y = G_TILE_HEIGHT * -1;
	/*This code reads a string of 1900 chars with numbers 0 through 3 (for now). Each number
	corresponds to a different 16x 16 tile shown below that will be placed on the screen in a 50x38 grid*/
	for (int y = 0; y < G_NUMBER_OF_TILES_Y; y++)
	{
		pass_y += G_TILE_HEIGHT;
		pass_x = G_TILE_WIDTH * -1;
		for (int x = 0; x < G_NUMBER_OF_TILES_X; x++)
		{
			pass_x += G_TILE_WIDTH;
			map_data >> map_value;
			switch (map_value)
			{
			case '_':
				the_tiles[x][y] = new Tile(nullptr, pass_x, pass_y, true);
				break;
			case '0':
				the_tiles[x][y] = new Tile(&invisible_tile_pic, pass_x, pass_y, false);
				break;
			case '1':
				the_tiles[x][y] = new Tile(&block_pic, pass_x, pass_y, false);
				break;
			default:
				std::cout << "There was an error in tile placement." << std::endl;
			}
		}
	}
}

Tiled_Map::~Tiled_Map()
{
	for (int y = 0; y != G_NUMBER_OF_TILES_Y; ++y)
	{
		for (int x = 0; x != G_NUMBER_OF_TILES_X; ++x)
		{
			delete the_tiles[x][y];
			the_tiles[x][y] = nullptr;
		}
	}
}

Screen::Screen(std::string tile_file_name, std::string object_file_name) :
	the_tile_map(new Tiled_Map(tile_file_name))
{
	std::ifstream object_file(object_file_name);
	//This function loops through the file until there is nothing left
	int object_value = 0;
	double object_position_x = 0;
	double object_position_y = 0;
	while (object_value != -842150451)
	{
		object_file >> object_value;
		object_file >> object_position_x;
		object_file >> object_position_y;
		switch (object_value)
		{
		case 0:
			the_hazards.push_back(new Spike_Up(object_position_x, object_position_y));
			break;
		case 1:
			the_hazards.push_back(new Spike_Down(object_position_x, object_position_y));
			break;
		case 2:
		{
			std::string file_name;
			object_file >> file_name;
			the_hazards.push_back(new Moon(object_position_x, object_position_y, file_name));
			break;
		}
		case 3:
			the_hazards.push_back(new Spinning_Saw(object_position_x, object_position_y));
			the_hazards.push_back(new Plaque(object_position_x - 15, object_position_y - 15));
			for (int counter = 0; counter <= Spinning_Saw::DISTANCE / CHAIN_DIAMETER; ++counter)
				the_hazards.push_back(new Chain(object_position_x, object_position_y, Spinning_Saw::DISTANCE - (counter * CHAIN_DIAMETER)));
			break;
		case 4:
			the_hazards.push_back(new Pendulum(object_position_x, object_position_y));
			the_hazards.push_back(new Plaque(object_position_x - 15, object_position_y - 15));
			for (int counter = 0; counter <= Pendulum::DISTANCE / CHAIN_DIAMETER; ++counter)
				the_hazards.push_back(new Pendulum_Chain(object_position_x, object_position_y, Pendulum::DISTANCE - (counter * CHAIN_DIAMETER)));
			break;
		case 5:
			the_save_boxes.push_back(new Save_Box(object_position_x, object_position_y));
			break;
		case 6:
		{
			std::string file_name;
			object_file >> file_name;
			the_moving_platforms.push_back(new Moving_Platform(object_position_x, object_position_y, 32, 8, file_name));
			break;
		}
		case 8:
			the_hazards.push_back(new Troll_Spike_Down(object_position_x, object_position_y));
			break;
		case 9:
		{
			std::string file_name;
			object_file >> file_name;
			the_doorways.push_back(new Doorway(object_position_x, object_position_y, file_name));
			break;
		}
		case 10:
			the_simple_objects.push_back(new Gate(object_position_x, object_position_y));
			break;
		case 9001:
			return;
		default:
			std::cout << "Unable to place a hazard correctly" << std::endl;
			break;
		}
	}
}

Screen::~Screen()
{
	for (int x = 0; x != the_hazards.size(); x++)
	{
		delete the_hazards[x];
		the_hazards[x] = nullptr;
	}
	for (int x = 0; x != the_save_boxes.size(); x++)
	{
		delete the_save_boxes[x];
		the_save_boxes[x] = nullptr;
	}
	for (int x = 0; x != the_moving_platforms.size(); x++)
	{
		delete the_moving_platforms[x];
		the_moving_platforms[x] = nullptr;
	}
	for (int x = 0; x != the_doorways.size(); x++)
	{
		delete the_doorways[x];
		the_doorways[x] = nullptr;
	}
	for (int x = 0; x != the_simple_objects.size(); x++)
	{
		delete the_simple_objects[x];
		the_simple_objects[x] = nullptr;
	}
	delete the_tile_map;
	the_tile_map = nullptr;
}

World::World(std::string world_data_file_name, int starting_position_x, int starting_position_y) :
	current_position_x_(starting_position_x),
	current_position_y_(starting_position_y)
{
	std::ifstream world_data(world_data_file_name);
	//First input the number of maps in the x direction... 0 cells means one.
	world_data >> world_number_;
	if (world_number_ == -842150451)
	{
		std::cout << "There was a problem loading world data '" << world_data_file_name << std::endl;
	}
	//number of cells in y direction
	world_data >> x_number_of_cells_;
	world_data >> y_number_of_cells_;
	//Now that we know how many cells we'll have. Push back the_screens_ with nullptrs
	for (int y = 0; y != y_number_of_cells_; ++y)
	{
		std::vector<Screen*> new_vector;
		for (int x = 0; x != x_number_of_cells_; ++x)
		{
			new_vector.push_back(nullptr);
		}
		the_screens_.push_back(new_vector);
	}
	int cell_x = 0;
	int cell_y = 0;
	std::string tile_file_name;
	std::string object_file_name;
	while (true)
	{
		world_data >> cell_x;
		//At the end of each world file document I put 9001 to tell the program to stop reading data.
		if (cell_x == 9001)
			break;
		world_data >> cell_y;
		world_data >> tile_file_name;
		world_data >> object_file_name;
		the_screens_[cell_y][cell_x] = new Screen(tile_file_name, object_file_name);
	}
	//Fill up the remaining uninitialized cells with a map with no tiles in it:
	for (int y = 0; y != y_number_of_cells_; ++y)
	{
		for (int x = 0; x != x_number_of_cells_; ++x)
		{
			if (the_screens_[y][x] == nullptr)
				the_screens_[y][x] = new Screen("Empty Map.txt", "Empty Map Objects.txt");
		}
	}
}

World::~World()
{
	for (int y = 0; y != y_number_of_cells_; ++y)
	{
		for (int x = 0; x != x_number_of_cells_; ++x)
		{
			delete the_screens_[y][x];
			the_screens_[y][x] = nullptr;
		}
	}
}

void World::switch_to_this_screen(int y, int x)
{
	Game::current_screen = the_screens_[y][x];
	Game::the_bullets.clear();
	current_position_x_ = x;
	current_position_y_ = y;
}

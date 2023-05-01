#ifndef WORLDS_AND_MAPS_H_
#define WORLDS_AND_MAPS_H_
#include <SDL.h>
#include <map>
#include "SDL_image.h"
#include "GameState.h"
#include "globals.h"
#include "Save Box.h"
#include "Hazards.h"
#include "Special Platform.h"
#include "Doorway.h"
#include "Smart Object.h"
#include "Game Event.h"
#include "Enemy.h"
#include "text_box.h"
#include "Enemy_And_Smart_Object_Pairs.h"
#include "Special_Platform_And_Enemy_Pairs.h"

using namespace std;

void load_tiles();

namespace Tiles
{
	extern Texture* surface_w1;
	extern Texture* ground_w1;
	extern Texture* top_left_w1;
	extern Texture* top_right_w1;
	extern Texture* left_w1;
	extern Texture* right_w1;
	extern Texture* bottom_right_w1;
	extern Texture* bottom_left_w1;
	extern Texture* bottom_w;
	extern Texture* top_right_corner_w1;
	extern Texture* top_left_corner_w1;
	extern Texture* pipe_top_left_tile;
	extern Texture* pipe_top_right_tile;
	extern Texture* pipe_bottom_left_tile;
	extern Texture* pipe_bottom_right_tile;
}

namespace Tile_Map
{
	extern map<char, Texture*> world_1_tiles;
}

extern map<char, Texture*>* current_tile_map;

class Tile : public Displayable_Object
{
public:
	//Initialize actual tiles with this
	explicit Tile(Texture* pic, double x, double y, bool passable, bool sticky) :
		Displayable_Object(x, y),
		passable_(passable),
		sticky_(sticky),
		height_(G_TILE_HEIGHT),
		width_(G_TILE_WIDTH)
	{
		if (pic != nullptr)
		{
			add_new_texture(pic);
		}
	}

	//Initialize extra walls with this.
	explicit Tile(Texture* pic, double x, double y, double width, double height, bool passable, bool sticky) :
		Displayable_Object(x, y),
		height_(height),
		width_(width),
		passable_(passable),
		sticky_(sticky)
	{
		add_new_texture(pic);
	}

	virtual ~Tile() {}
	virtual void logic() {}
	void dislodge();

	inline double left_of_tile()const
	{
		return location_.x;
	}

	inline double top_of_tile()const
	{
		return location_.y;
	}

	inline double right_of_tile()const
	{
		return location_.x + width_;
	}

	inline double bottom_of_tile()const
	{
		return location_.y + height_;
	}


	inline bool exists()
	{
		return !the_pics_.empty();
	}

	inline bool passable()
	{
		return passable_;
	}

	inline bool sticky()
	{
		return sticky_;
	}

	inline bool handle_collision_with_tile_on_top(Rect* hit_box, Point& projection)
	{
		if (
			projection.y + hit_box->h > top_of_tile() &&
			hit_box->y_bottom() <= top_of_tile() &&
			projection.x < right_of_tile() &&
			projection.x + hit_box->w > left_of_tile()
			)
		{
			hit_box->y = top_of_tile() - hit_box->h;
			return true;
		}
		return false;
	}

	inline bool handle_collision_with_tile_on_bottom(Rect* hit_box, Point& projection)
	{
		if (
			projection.y < bottom_of_tile() &&
			hit_box->y >= bottom_of_tile() &&
			projection.x < right_of_tile() &&
			projection.x + hit_box->w > left_of_tile()
			)
		{
			hit_box->y = bottom_of_tile() + 0.1;
			return true;
		}
		return false;
	}

	inline bool handle_collision_with_tile_to_the_left(Rect* hit_box, Point& projection)
	{
		if (
			projection.x + hit_box->w > left_of_tile() &&
			hit_box->x_right() <= left_of_tile() &&
			projection.y + hit_box->h >= top_of_tile() &&
			projection.y <= bottom_of_tile()
			)
		{
			hit_box->x = left_of_tile() - 0.1 - hit_box->w;
			return true;
		}
		return false;
	}

	inline bool handle_collision_with_tile_to_the_right(Rect* hit_box, Point& projection)
	{
		if (
			projection.x < right_of_tile() &&
			hit_box->x >= right_of_tile() &&
			projection.y + hit_box->h >= top_of_tile() &&
			projection.y <= bottom_of_tile()
			)
		{
			hit_box->x = right_of_tile() + 0.1;
			return true;
		}
		return false;
	}

private:
	bool passable_;
	bool sticky_;
	double width_;
	double height_;
};

class Spinning_Tile : public Smart_Object
{
public:
	Spinning_Tile(Tile* old_tile) :
		Smart_Object(old_tile->location_x(), old_tile->location_y()),
		time_left_(2000),
		velocity_x_(rand() % 2000 - 1000),
		velocity_y_(rand() % 2000 - 1000),
		force_from_gravity_(0),
		SPINNING_SPEED(static_cast<double>(rand() % 100 - 50))
	{
		force_from_gravity_ = velocity_y_;
		add_new_texture(old_tile->current_texture());
	}

	void logic()
	{
		angle_ += SPINNING_SPEED * g_timer.deltaT_in_seconds();
		modify_velocity_with_gravity(&velocity_y_, &force_from_gravity_);
		location_.x += velocity_x_ * g_timer.deltaT_in_seconds();
		location_.y += velocity_y_;
		time_left_ -= g_timer.deltaT();
		if (time_left_ <= 0)
			prepare_to_delete();
	}

private:
	double time_left_;
	const double SPINNING_SPEED;
	double velocity_x_;
	double velocity_y_;
	double force_from_gravity_;
};

class Tiled_Map
{
public:
	Tiled_Map(int number_of_screens_x, int number_of_screens_y);
	~Tiled_Map();
	void read_a_file(int screen_x, int screen_y, string tile_file_name);
	void make_an_empty_map(int screen_x, int screen_y);
	void dislodge_tiles_in_box(Rect* in_here);
	void create_a_tile(int x, int y, char type);

	void Show_Map()
	{
		int start_y = static_cast<int>(Game::camera->y() / G_TILE_HEIGHT);
		int end_y = start_y + TILES_PER_SCREEN_Y + 1;
		if (end_y > static_cast<int>(the_tiles[0].size()))
			end_y = static_cast<int>(the_tiles[0].size());
		int start_x = static_cast<int>(Game::camera->x() / G_TILE_WIDTH);
		int end_x = start_x + TILES_PER_SCREEN_X + 1;
		if (end_x > static_cast<int>(the_tiles.size()))
			end_x = static_cast<int>(the_tiles.size());
		if (start_x < 0)
			start_x = 0;
		if (start_y < 0)
			start_y = 0;
		for (int y = start_y; y != end_y; ++y)
		{
			for (int x = start_x; x != end_x; ++x)
			{
				if (the_tiles[x][y]->exists())
				{
					the_tiles[x][y]->draw_to_screen();
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
		the_tiles[x][y] = new Tile(nullptr, x, y, true, false);
	}

	inline bool is_this_spot_passable(Point& here) const
	{
		int check_x = static_cast<int>(here.x / G_TILE_WIDTH);
		int check_y = static_cast<int>(here.y / G_TILE_HEIGHT);
		return is_this_tile_passable(check_x, check_y);
	}

	inline bool is_this_tile_passable(int x, int y) const
	{
		if (x >= the_tiles.size())
		{
			return true;
		}
		if (y >= the_tiles[0].size())
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

	inline bool is_this_tile_sticky(int x, int y)
	{
		if (x >= the_tiles.size())
		{
			return true;
		}
		if (y >= the_tiles[0].size())
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
		return the_tiles[x][y]->sticky();
	}

	inline int number_of_tiles_x()
	{
		return TILES_PER_SCREEN_X - 1;
	}

	inline int number_of_tiles_y()
	{
		return TILES_PER_SCREEN_Y - 1;
	}

	inline void test_for_sticky_collision(Rect* location, Point& projection, double* xVelocity, double* yVelocity)
	{
		for (int x = static_cast<int>(projection.x / G_TILE_WIDTH); x <= static_cast<int>((projection.x + location->w) / G_TILE_WIDTH); ++x)
		{
			for (int y = static_cast<int>(projection.y / G_TILE_HEIGHT); y <= static_cast<int>((projection.y + location->h) / G_TILE_WIDTH); ++y)
			{
				if (is_this_tile_sticky(x, y) && !is_this_tile_passable(x, y))
				{
					*xVelocity = 0;
					*yVelocity = 0;
				}
			}
		}
	}

private:
	vector<vector<Tile*>> the_tiles;
};

class Screen
{
public:
	//a 2D vector of "Screens" are constructed when the world is.
	//Send in the screen tile data, then the screen object data
	Screen(int world_position_x, int world_position_y, string object_file_name);
	~Screen();
	void initialize_event(int event_code);
	void current_screen_logic();
	void screen_logic();
	void delete_objects_if_ready();
	void show_collision_boundaries();
	Doorway* search_for_available_doors();
	void collision_test_with_enemies(Bullet* the_bullet);

	Smart_Object* grab_a_smart_object(int position)
	{
		return the_smart_objects_[position];
	}

	Smart_Hazard* grab_a_smart_hazard(int position)
	{
		return the_smart_hazards_[position];
	}

	Enemy* grab_an_enemy(int position)
	{
		return the_enemies_[position];
	}

	Tile* grab_an_extra_wall(int position)
	{
		return extra_walls[position];
	}

	Hazard* grab_a_dumb_hazard(int position)
	{
		return the_dumb_hazards_[position];
	}

	size_t number_of_smart_objects()
	{
		return the_smart_objects_.size();
	}

	size_t number_of_enemies()
	{
		return the_enemies_.size();
	}

	size_t number_of_smart_hazards()
	{
		return the_smart_hazards_.size();
	}

	size_t number_of_extra_walls()
	{
		return extra_walls.size();
	}

	inline void add_to_render_list(Displayable_Object* new_object, bool add_to_background, bool add_to_end)
	{
		if (add_to_background)
		{
			if (add_to_end)
			{
				background_objects_.push_back(new_object);
			}
			else
			{
				background_objects_.insert(background_objects_.begin(), new_object);
			}
		}
		else
		{
			if (add_to_end)
			{
				foreground_objects_.push_back(new_object);
			}
			else
			{
				foreground_objects_.insert(foreground_objects_.begin(), new_object);
			}
		}
	}

	inline void add_a_new_smart_hazard(Smart_Hazard* new_hazard, bool add_to_background, bool add_to_end)
	{
		the_smart_hazards_.push_back(new_hazard);
		add_to_render_list(new_hazard, add_to_background, add_to_end);
	}

	inline void add_a_new_dumb_hazard(Hazard* new_hazard, bool add_to_background, bool add_to_end)
	{
		the_dumb_hazards_.push_back(new_hazard);
		add_to_render_list(new_hazard, add_to_background, add_to_end);
	}

	inline void add_a_new_platform(Special_Platform* new_platform, bool add_to_background, bool add_to_end)
	{
		the_special_platforms.push_back(new_platform);
		add_to_render_list(new_platform, add_to_background, add_to_end);
	}

	inline void add_a_new_doorway(Doorway* new_doorway, bool add_to_background, bool add_to_end)
	{
		the_doorways_.push_back(new_doorway);
		add_to_render_list(new_doorway, add_to_background, add_to_end);
	}

	inline void add_a_new_smart_object(Smart_Object* new_smart_object, bool add_to_background, bool add_to_end)
	{
		the_smart_objects_.push_back(new_smart_object);
		add_to_render_list(new_smart_object, add_to_background, add_to_end);
	}

	inline void add_a_new_dumb_object(Displayable_Object* new_dumb_object, bool add_to_background, bool add_to_end)
	{
		the_dumb_objects_.push_back(new_dumb_object);
		add_to_render_list(new_dumb_object, add_to_background, add_to_end);
	}

	inline void add_a_new_enemy(Enemy* new_enemy, bool add_to_background, bool add_to_end)
	{
		the_enemies_.push_back(new_enemy);
		for (int counter = 0; counter != new_enemy->the_parts_.size(); counter++)
			add_to_render_list(new_enemy->the_parts_[counter], add_to_background, add_to_end);
	}

	inline void add_a_new_event(Game_Event* new_event)
	{
		the_events_.push_back(new_event);
	}

	inline void add_a_new_extra_wall(Tile* new_tile, bool add_to_background, bool add_to_end)
	{
		extra_walls.push_back(new_tile);
		add_to_render_list(new_tile, add_to_background, add_to_end);
	}

	inline void add_a_new_particle(Particle* new_particle, bool add_to_background, bool add_to_end)
	{
		the_particles_.push_back(new_particle);
		add_to_render_list(new_particle, add_to_background, add_to_end);
	}

	inline void render_background()
	{
		for_each(background_objects_.begin(), background_objects_.end(), mem_fun(&Displayable_Object::draw_to_screen));
	}

	inline void render_foreground()
	{
		for_each(foreground_objects_.begin(), foreground_objects_.end(), mem_fun(&Displayable_Object::draw_to_screen));
	}

	inline bool all_enemies_are_dead()
	{
		if (the_enemies_.empty())
		{
			return true;
		}
		return false;
	}
	inline void remove_all_events()
	{
		the_events_.clear();
	}
	inline void remove_all_smart_hazards()
	{
		for (int counter = 0; counter != the_smart_hazards_.size(); counter++)
		{
			the_smart_hazards_[counter]->prepare_to_delete();
		}
	}
	inline void remove_all_dumb_hazards()
	{
		for (int counter = 0; counter != the_dumb_hazards_.size(); counter++)
		{
			the_dumb_hazards_[counter]->prepare_to_delete();
		}
	}
	inline void remove_all_doors()
	{
		for (int counter = 0; counter != the_doorways_.size(); counter++)
		{
			the_doorways_[counter]->prepare_to_delete();
		}
	}
	inline void remove_all_smart_objects()
	{
		for (int counter = 0; counter != the_smart_objects_.size(); counter++)
		{
			the_smart_objects_[counter]->prepare_to_delete();
		}
	}
	inline void remove_all_dumb_objects()
	{
		for (int counter = 0; counter != the_dumb_objects_.size(); counter++)
		{
			the_dumb_objects_[counter]->prepare_to_delete();
		}
	}
	inline void kill_all_enemies()
	{
		for (int counter = 0; counter != the_enemies_.size(); counter++)
		{
			the_enemies_[counter]->kill_this_enemy();
		}
	}
	inline void remove_all_extra_walls()
	{
		for (int counter = 0; counter != the_enemies_.size(); counter++)
		{
			extra_walls[counter]->prepare_to_delete();
		}
	}

public:
	vector<Special_Platform*> the_special_platforms;
	vector<Magic_Teleport_Box*> magic_teleport_boxes;
	vector<Tile*> extra_walls;
private:
	vector<Game_Event*> the_events_;
	vector<Smart_Hazard*> the_smart_hazards_;
	vector<Hazard*> the_dumb_hazards_;
	vector<Doorway*> the_doorways_;
	vector<Smart_Object*> the_smart_objects_;
	vector<Displayable_Object*> the_dumb_objects_;
	vector<Enemy*> the_enemies_;
	vector<Particle*> the_particles_;
	list<Displayable_Object*> foreground_objects_;
	list<Displayable_Object*> background_objects_;
	int world_position_x_;
	int world_position_y_;
};


class World
{
public:
	World(string world_data_file_name, int starting_position_x, int starting_position_y);
	~World();
	void switch_to_this_screen(int y, int x);
	void left_one_screen();
	void right_one_screen();
	void up_one_screen();
	void down_one_screen();
	void delete_objects_if_ready();
	void show_collision_boundaries();
	void perform_all_world_logic();
	void world_logic();

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

	inline int world_width()
	{
		return g_screen_width * x_number_of_cells_;
	}

	inline int world_height()
	{
		return g_screen_height * y_number_of_cells_;
	}

	inline void render_tile_map()
	{
		the_tile_map_->Show_Map();
	}

	inline void render_background()
	{
		if (background_image_ != nullptr)
			background_image_->render();
		for_each(background_objects_.begin(), background_objects_.end(), mem_fun(&Displayable_Object::draw_to_screen));
	}

	inline void render_foreground()
	{
		for_each(foreground_objects_.begin(), foreground_objects_.end(), mem_fun(&Displayable_Object::draw_to_screen));
	}

	inline Tiled_Map* the_tile_map()
	{
		return the_tile_map_;
	}

	inline void add_to_render_list(Displayable_Object* new_object, bool add_to_background, bool add_to_end)
	{
		if (add_to_background)
		{
			if (add_to_end)
			{
				background_objects_.push_back(new_object);
			}
			else
			{
				background_objects_.insert(background_objects_.begin(), new_object);
			}
		}
		else
		{
			if (add_to_end)
			{
				foreground_objects_.push_back(new_object);
			}
			else
			{
				foreground_objects_.insert(foreground_objects_.begin(), new_object);
			}
		}
	}

	inline void add_a_new_smart_hazard(Smart_Hazard* new_hazard, bool add_to_background, bool add_to_end)
	{
		world_smart_hazards_.push_back(new_hazard);
		add_to_render_list(new_hazard, add_to_background, add_to_end);
	}

	inline void add_a_new_enemy(Enemy* new_enemy, bool add_to_background, bool add_to_end)
	{
		world_enemies_.push_back(new_enemy);
		for (int counter = 0; counter != new_enemy->the_parts_.size(); counter++)
		{
			add_to_render_list(new_enemy->the_parts_[counter], add_to_background, add_to_end);
		}
	}

private:
	void add_world_objects(string file_name);

	Tiled_Map* the_tile_map_;
	int x_number_of_cells_;
	int y_number_of_cells_;
	vector<vector<Screen*>> the_screens_;
	int current_position_x_;
	int current_position_y_;
	int world_number_;
	Rect world_box_;
	Background* background_image_;
	//These can be in multiple screens
	vector<Smart_Hazard*> world_smart_hazards_;
	vector<Smart_Object*> world_smart_objects_;
	vector<Enemy*> world_enemies_;
	vector<Special_Platform*> world_special_platforms_;
	list<Displayable_Object*> world_background_;
	vector<Tile*> world_extra_walls_;
	//I call a rect collision test for whether or not I want to render these.
	list<Displayable_Object*> foreground_objects_;
	list<Displayable_Object*> background_objects_;
};

namespace Game
{
	extern World* current_world;
	extern Screen* current_screen;
}

void bullet_world_collision(Bullet* the_bullet);

class Spike_Pillar_Base : public Tile
{
public:
	Spike_Pillar_Base(double x, double y, double distance, double frequency, double init_percent_thru_cycle) :
		Tile(Pic::spike_pillar_base, x, y, (double)32, (double)336, false, false),
		distance_(distance),
		frequency_(frequency),
		starting_location_y_(y),
		starting_location_x_(x),
		timer_(frequency* init_percent_thru_cycle)
	{}

	void logic();

private:
	const double starting_location_x_;
	const double starting_location_y_;
	double timer_;
	double distance_;
	double frequency_;
};

#endif

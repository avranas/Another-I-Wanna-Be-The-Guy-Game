#ifndef IMAGE_PROCESSING_H_
#define IMAGE_PROCESSING_H_
#include <SDL.h>
#include "SDL_image.h"
#include "globals.h"
#include <string>
#include <fstream>

class Camera
{
public:
	Camera() :
		main_box(Rect(0, 0, g_screen_width, g_screen_height)),
		offset_x_(0),
		offset_y_(0),
		momentum_(0),
		start_momentum_(0),
		start_time_(0),
		timer_(0),
		minimum_momentum_(0)
	{}

	inline Rect box()
	{
		Rect box = main_box;
		box.x += offset_x_;
		box.x += offset_y_;
		return main_box;
	}

	//loging() is called every frame in MainGame::logic() to handle effects like screen shaking
	void logic();
	inline double x() { return main_box.x + offset_x_; }
	inline double x_main() { return main_box.x; }
	inline double y_main() { return main_box.y; }
	inline double y() { return main_box.y + offset_y_; }
	inline double y_bottom() { return main_box.y + offset_y_ + main_box.h; }
	inline double x_right() { return main_box.x + offset_x_ + main_box.w; }
	inline double y_bottom_main() { return main_box.y + main_box.h; }
	inline double x_right_main() { return main_box.x + main_box.w; }
	inline void set_main_position_x(double value) { main_box.x = value; }
	inline void set_main_position_y(double value) { main_box.y = value; }
	inline void set_offset_y(double value) { offset_y_ = value; }
	inline void set_offset_x(double value) { offset_x_ = value; }

	inline void remove_offset()
	{
		offset_x_ = 0;
		offset_y_ = 0;
	}

	//If this function gets called every loop, the screen will shake with this much power.
	inline void shaking_minimum(double value)
	{
		minimum_momentum_ = value;
	}

	void set_rand_offset(double power)
	{
		if (power < 2)
			return;
		offset_x_ = rand() % static_cast<int>(power) - (power / 2);
		offset_y_ = rand() % static_cast<int>(power) - (power / 2);
	}

	inline void bump(double power, int time)
	{
		momentum_ = power;
		start_momentum_ = power;
		timer_ = time;
		start_time_ = time;
	}

private:
	//used for bump
	double momentum_;
	double start_momentum_;
	int start_time_;
	int timer_;
	double minimum_momentum_;
	Rect main_box;
	double offset_x_;
	double offset_y_;
	bool reset_minimum_after_cycle_;
};

namespace Game
{
	extern Camera* camera;
}

SDL_Texture* load_texture(string path);

//all pics and animations
class Texture
{
public:
	Texture(string pic_file_name, string data_file_name);
	Texture(SDL_Texture* new_texture, int width, int height);

	~Texture()
	{
		SDL_DestroyTexture(pic_);
	}

	inline bool animated()
	{
		if (number_of_frames_ > 0)
			return true;
		else
			return false;
	}

	inline bool initialized()
	{
		return initialized_;
	}

	inline void init()
	{
		if (!initialized_)
		{
			pic_ = load_texture(pic_file_name_);
			cout << pic_file_name_ << " loaded" << endl;
			initialized_ = true;
		}
	}

	inline void deinit()
	{
		if (initialized_)
		{
			cout << pic_file_name_ << " deinitialized!" << endl;
			SDL_DestroyTexture(pic_);
			pic_ = nullptr;
			initialized_ = false;
		}
	}

	inline SDL_Texture* pic()
	{
		return pic_;
	}

	inline string data_file_name()
	{
		return data_file_name_;
	}

	inline SDL_Rect box_on_sprite_sheet(int frame_number)
	{
		SDL_Rect return_this;
		return_this.x = location_on_sprite_sheet_[frame_number].x;
		return_this.y = location_on_sprite_sheet_[frame_number].y;
		return_this.w = width_;
		return_this.h = height_;
		return return_this;
	}

	inline int number_of_frames()
	{
		return number_of_frames_;
	}

	inline bool loop()
	{
		return loop_;
	}

	inline int frame_rate()
	{
		return frame_rate_;
	}

	//Doesn't use angle, size_ratio, or camera
	inline void simple_render(int frame_number, double x, double y)
	{
		SDL_Rect destination;
		destination.x = static_cast<int>(x);
		destination.y = static_cast<int>(y);
		destination.w = static_cast<int>(width_);
		destination.h = static_cast<int>(height_);
		SDL_RenderCopy(
			g_renderer,
			pic_,
			&SDL_Rect(
				static_cast<int>(location_on_sprite_sheet_[frame_number].x),
				static_cast<int>(location_on_sprite_sheet_[frame_number].y),
				static_cast<int>(width_),
				static_cast<int>(height_)
			),
			&destination
		);
	}

	inline SDL_Rect box_on_window(double x, double y)const
	{
		SDL_Rect return_this;
		return_this.x = static_cast<int>(x);
		return_this.y = static_cast<int>(y);
		return_this.w = width_;
		return_this.h = height_;
		return return_this;
	}

	inline double width()
	{
		return width_;
	}

	inline double height()
	{
		return height_;
	}

	inline void modify_width(int value)
	{
		width_ = value;
	}

	inline void modify_height(int value)
	{
		height_ = value;
	}

	inline double location_on_sprite_sheet_x(int frame_number)
	{
		return location_on_sprite_sheet_[frame_number].x;
	}

	inline double location_on_sprite_sheet_y(int frame_number)
	{
		return location_on_sprite_sheet_[frame_number].y;
	}

private:
	string pic_file_name_;
	string data_file_name_;
	SDL_Texture* pic_;
	bool initialized_;
	int width_;
	int height_;
	vector<SDL_Point> location_on_sprite_sheet_;
	bool loop_;
	int number_of_frames_;
	int frame_rate_;
	double setback_x_;
	double setback_y_;
};

//inherited values and methods of all objects that use a texture
class Displayable_Object
{
public:
	inline void draw_to_screen()
	{
		if (animated())
			animate();
		SDL_RenderCopyEx(
			g_renderer,
			current_pic(),
			&box_in_file(),
			&box_on_window(),
			angle(),
			nullptr,
			direction()
		);
	}

	inline void simple_render()
	{
		the_pics_[current_animation_]->simple_render(current_frame_, location_.x, location_.y);
	}

	Displayable_Object(double x, double y);
	Displayable_Object(double x, double y, Texture* the_pic);
	Displayable_Object(double x, double y, Texture* the_pic, double angle);

	inline Point location()
	{
		return location_;
	}

	inline SDL_Rect box_on_window()
	{
		return SDL_Rect(
			static_cast<int>((location_.x /*- pic_->setback_.x*/ - Game::camera->x()) - (width() * (width_ratio_ - 1) / 2)),
			static_cast<int>((location_.y /*- setback_.y */ - Game::camera->y()) - (height() * (height_ratio_ - 1) / 2)),
			static_cast<int>(width() * width_ratio_),
			static_cast<int>(height() * height_ratio_)
		);
	}

	inline SDL_Rect box_in_file()
	{
		return SDL_Rect(
			static_cast<int>(the_pics_[current_animation_]->location_on_sprite_sheet_x(current_frame_)),
			static_cast<int>(the_pics_[current_animation_]->location_on_sprite_sheet_y(current_frame_)),
			static_cast<int>(width()),
			static_cast<int>(height()
				));
	}

	inline bool animated()
	{
		return the_pics_[current_animation_]->animated();
	}

	void grow()
	{
		height_ratio_ *= 1.5;
		width_ratio_ *= 1.5;
	}

	void shrink()
	{
		height_ratio_ /= 1.5;
		width_ratio_ /= 1.5;
	}

	virtual ~Displayable_Object() {}

	inline Rect display_outline()
	{
		Rect outline;
		outline.x = location_.x - (width() * (width_ratio_ - 1) / 2);
		outline.y = location_.y - (height() * (height_ratio_ - 1) / 2);
		outline.w = width() * width_ratio_;
		outline.h = height() * height_ratio_;
		return outline;
	}

	//Does actual deleting if delete flag is on, then returns the flag.
	inline bool delete_if_ready()
	{
		if (ready_to_delete_)
		{
			delete this;
			return true;
		}
		return false;
	}

	//Returns delete flag. Doesn't do any deleting
	inline bool ready_to_delete()
	{
		return ready_to_delete_;
	}

	inline void prepare_to_delete()
	{
		ready_to_delete_ = true;
	}

	void animate();

	inline void switch_to_animation(int value)
	{
		if (current_animation_ == value)
			return;
		current_animation_ = value;
		time_in_current_frame_ = 0;
		current_frame_ = 0;
		animation_over_ = false;
	}

	inline void move_x(double value)
	{
		location_.x += value;
	}

	inline void move_y(double value)
	{
		location_.y += value;
	}

	inline double location_x()
	{
		return location_.x;
	}

	inline double location_y()
	{
		return location_.y;
	}

	inline double mid_point_x()
	{
		return location_.x + the_pics_[current_animation_]->width() / 2;
	}

	inline double mid_point_y()
	{
		return location_.y + the_pics_[current_animation_]->height() / 2;
	}

	inline void new_location_x(double value)
	{
		location_.x = value;
	}

	inline void new_location_y(double value)
	{
		location_.y = value;
	}

	//When these functions are used, the size changes for all instances of the object. For this reason\
	you should never use them, ever. I'm only using this for boss health bars until I think of a better solution.
	inline void modify_height_of_current_pic(int value)
	{
		the_pics_[current_animation_]->modify_height(value);
	}

	inline void modify_width_of_current_pic(int value)
	{
		the_pics_[current_animation_]->modify_width(value);
	}

	inline void center_over_this_point(Point& here)
	{
		location_.x = here.x - width() / 2;
		location_.y = here.y - height() / 2;
	}

	inline void render_outline()
	{
		SDL_Rect rect;
		Rect outline = display_outline();
		rect.x = static_cast<int>(outline.x);
		rect.y = static_cast<int>(outline.y);
		rect.w = static_cast<int>(outline.w);
		rect.h = static_cast<int>(outline.h);
		SDL_RenderDrawRect(g_renderer, &rect);
	}

	inline double width()
	{
		return the_pics_[current_animation_]->width();
	}

	inline double height()
	{
		return the_pics_[current_animation_]->height();
	}

	inline Point middle_of_box()
	{
		Point mid;
		mid.x = location_.x + the_pics_[current_animation_]->width() / 2;
		mid.y = location_.y + the_pics_[current_animation_]->height() / 2;
		return mid;
	}

	//returns the original
	inline Texture* change_pic_for_some_time(Texture* to_this, int position)
	{
		Texture* temp_pic_ = the_pics_[position];
		temp_position_ = position;
		the_pics_[position] = to_this;
		return temp_pic_;
	}

	inline void revert_to_this_pic(Texture* temp_pic_)
	{
		the_pics_[temp_position_] = temp_pic_;
	}

	inline void modify_radius_of_circle(double value)
	{
		width_ratio_ = value / width();
	}

	//Never change the size of a circle with this; use modify_radius_of_circle() instead.
	inline void modify_height_of_current_collision(double value)
	{
		height_ratio_ = value / height();
	}

	//Never change the size of a circle with this; use modify_radius_of_circle() instead.
	inline void modify_width_of_current_collision(double value)
	{
		width_ratio_ = value / width();
	}

	inline double angle() { return angle_; }

	inline Texture* current_texture()
	{
		return the_pics_[current_animation_];
	}

	inline SDL_Texture* current_pic()
	{
		return the_pics_[current_animation_]->pic();
	}

	inline SDL_RendererFlip direction()
	{
		return direction_;
	}

	inline double width_ratio()
	{
		return width_ratio_;
	}

	inline double height_ratio()
	{
		return height_ratio_;
	}

protected:
	inline void add_new_texture(Texture* new_pic)
	{
		new_pic->init();
		the_pics_.push_back(new_pic);
	}

	//SDL_FLIP_NONE if facing right, SDL_FLIP_HORIZONTAL if facing left.
	SDL_RendererFlip direction_;
	Point location_;
	double angle_;
	vector<Texture*> the_pics_;
	int current_animation_;
	int time_in_current_frame_;
	int current_frame_;
	bool animation_over_;
	double width_ratio_;
	double height_ratio_;
	bool ready_to_delete_;
	int temp_position_;
private:
};

//An effect that plays for a certain amount of time then disappears.
//Can have speed and velocity if you want.
class Particle : public Displayable_Object
{
public:
	Particle(Texture* pic, double time_spent_active, double location_x, double location_y,
		double init_velocity_x, double init_velocity_y, bool uses_gravity) :
		Displayable_Object(location_x, location_y),
		velocity_x_(init_velocity_x),
		velocity_y_(init_velocity_y),
		force_from_gravity_(init_velocity_y),
		uses_gravity_(uses_gravity),
		timer_(time_spent_active)
	{
		add_new_texture(pic);
	}

	//I do not intend to include special logic. Everything that gets writen to the screen\
	calls this function once. 
	void particle_logic()
	{
		if (timer_.countdown_logic())
			prepare_to_delete();
		location_.x += velocity_x_ * g_timer.deltaT_in_seconds();
		location_.y += velocity_y_ * g_timer.deltaT_in_seconds();
		if (uses_gravity_)
			modify_velocity_with_gravity(&velocity_y_, &force_from_gravity_);
	}

private:
	bool uses_gravity_;
	double velocity_x_;
	double velocity_y_;
	double force_from_gravity_;
	Countdown timer_;
};

class Particle_Generator
{
public:
	void make_a_particle(Particle* new_particle, bool add_to_front)
	{
		particle_queue_.push_back(new_particle);
		add_to_front_.push_back(add_to_front);
	}

	//Call this function once per loop to add all waiting particles to the main game
	//Be sure to delete your particles after this
	inline Particle* grab_and_remove_back_particle()
	{
		Particle* new_particle = particle_queue_[particle_queue_.size() - 1];
		particle_queue_.pop_back();
		return new_particle;
	}

	inline bool empty()
	{
		return particle_queue_.empty();
	}

	inline bool add_to_front()
	{
		bool add_to_front = add_to_front_[add_to_front_.size() - 1];
		add_to_front_.pop_back();
		return add_to_front;
	}

private:
	vector<Particle*> particle_queue_;
	vector<bool> add_to_front_;
};


namespace Pic
{
	extern Texture* no_pic;
	extern Texture* the_kid_idle;
	extern Texture* the_kid_walking;
	extern Texture* the_kid_jumping;
	extern Texture* the_kid_falling;
	extern Texture* moon;
	extern Texture* reverse_moon;
	extern Texture* spike;
	extern Texture* blank_tile;
	extern Texture* block;
	extern Texture* blue_tile;
	extern Texture* bullet;
	extern Texture* buzzsaw;
	extern Texture* chain;
	extern Texture* plaque;
	extern Texture* save_box_idle;
	extern Texture* save_box_saving;
	extern Texture* game_over;
	extern Texture* Blood_1;
	extern Texture* Blood_2;
	extern Texture* Blood_3;
	extern Texture* Dead_Kid_1;
	extern Texture* Dead_Kid_2;
	extern Texture* Dead_Kid_3;
	extern Texture* platform_1;
	extern Texture* doorway_idle;
	extern Texture* gate_closed;
	extern Texture* gate_opening;
	extern Texture* gate_open;
	extern Texture* invisible_tile;
	extern Texture* falling_platform_idle;
	extern Texture* falling_platform_falling;
	extern Texture* falling_platform_fallen;
	extern Texture* falling_platform_falling_reverse;
	extern Texture* falling_platform_fallen_reverse;
	extern Texture* carock_idle;
	extern Texture* carock_idle_alt;
	extern Texture* carock_casting;
	extern Texture* carock_casting_alt;
	extern Texture* magic_projectile;
	extern Texture* doorway_open;
	extern Texture* frog_idle;
	extern Texture* frog_jumping;
	extern Texture* door_tile;
	extern Texture* door_tile_edge;
	extern Texture* yellow_switch_tile;
	extern Texture* yellow_switch_idle;
	extern Texture* yellow_switch_pressed;
	extern Texture* green_switch_idle;
	extern Texture* green_switch_pressed;
	extern Texture* lava_top_tile;
	extern Texture* lava_tile;
	extern Texture* bridge_tile;
	extern Texture* axe;
	extern Texture* hole_in_the_wall;
	extern Texture* big_kid;
	extern Texture* finger;
	extern Texture* title;
	extern Texture* barry_idle;
	extern Texture* barry_idle_alt;
	extern Texture* barry_attack;
	extern Texture* barry_attack_alt;
	extern Texture* barry_dying;
	extern Texture* boss_bar_text;
	extern Texture* boss_bar_frame;
	extern Texture* health;
	extern Texture* anti_health;
	extern Texture* spring_idle;
	extern Texture* spring_active;
	extern Texture* hadouken;
	extern Texture* fireball;
	extern Texture* empty_green_tile;
	extern Texture* empty_yellow_tile;
	extern Texture* green_switch_tile;
	extern Texture* thundercloud_waiting;
	extern Texture* thundercloud_warning;
	extern Texture* thundercloud_active;
	extern Texture* thunderbolt;
	extern Texture* big_fireball;
	extern Texture* thrown_rock;
	extern Texture* wuss_box_idle;
	extern Texture* wuss_box_saving;
	extern Texture* controller_warning;
	extern Texture* trolley_idle;
	extern Texture* trolley_moving_left;
	extern Texture* trolley_moving_right;
	extern Texture* windmill;
	extern Texture* link_shoveling;
	extern Texture* empty_blue_tile;
	extern Texture* explosion1;
	extern Texture* explosion2;
	extern Texture* explosion3;
	extern Texture* explosion4;
	extern Texture* explosion5;
	extern Texture* pounder_idle;
	extern Texture* pounder_idle_alt;
	extern Texture* pounder_top_fist;
	extern Texture* pounder_top_fist_alt;
	extern Texture* pounder_side_fist;
	extern Texture* pounder_side_fist_alt;
	extern Texture* shockwave;
	extern Texture* one_way_platform_1;
	extern Texture* mini_axe;
	extern Texture* spike_wall;
	extern Texture* missile;
	extern Texture* metroid_door_idle;
	extern Texture* metroid_door_flashing;
	extern Texture* metroid_door_opening;
	extern Texture* metroid_door_back;
	extern Texture* house;
	extern Texture* save_switch_idle;
	extern Texture* save_switch_pressed;
	extern Texture* wuss_switch_idle;
	extern Texture* wuss_switch_pressed;
	extern Texture* falling_spike;
	extern Texture* platform_2;
	extern Texture* crushing_spikes;
	extern Texture* chasing_spikes;
	extern Texture* spike_pillar_base;
	extern Texture* x_key;
	extern Texture* z_key;
	extern Texture* t_key;
	extern Texture* arrow_key;
	extern Texture* boss_platform;
	extern Texture* sparkle;
	extern Texture* mushroom;
	extern Texture* smoke;
	extern Texture* star;
	//extern Texture* star_hud_empty;
	extern Texture* star_hud_filled;
	extern Texture* fake_mushroom;
	extern Texture* super_spring_idle;
	extern Texture* super_spring_active;
	extern Texture* super_spring_fail;
	extern Texture* pipe;
	extern Texture* teleporter;
	extern Texture* teleporter_hud_filled;
	//extern Texture* teleporter_hud_empty;
	extern Texture* fake_mushroom_2;
	extern Texture* big_pipe;
	extern Texture* reverse_big_pipe;
	extern Texture* congrats;
	extern Texture* portal;
	extern Texture* title_background;
	extern Texture* alex_2;
	extern Texture* rising_spikes;
	extern Texture* nope;
	extern Texture* firework;
	extern Texture* trail;
	extern Texture* spark;
	extern Texture* bar;
	extern Texture* ball;
}

//Uploads a box in a .png file, then stretches it to fit the back of a screen.
class Background
{
public:
	Background(string file_name, Rect box_on_file, double move_this_much_x, double move_this_much_y) :
		file_name_(file_name),
		box_on_window_(Rect(0, 0, g_screen_width, g_screen_height)),
		pic_(nullptr),
		box_on_file_(box_on_file),
		move_this_much_x_(move_this_much_x),
		move_this_much_y_(move_this_much_y)
	{
		change_pic();
	}

	~Background()
	{
		SDL_DestroyTexture(pic_);
	}

	void shift(int screen_x, int screen_y)
	{
		box_on_file_.x = screen_x * static_cast<int>(move_this_much_x_) * g_screen_width;
		box_on_file_.y = screen_y * static_cast<int>(move_this_much_y_) * g_screen_height;
		//change_pic();
	}

	void render()
	{
		if (pic_ != nullptr)
			SDL_RenderCopy(
				g_renderer,
				pic_,
				&box_on_file_,
				&box_on_window_
			);
	}

	void change_pic()
	{
		SDL_DestroyTexture(pic_);
		pic_ = nullptr;
		pic_ = load_texture(file_name_);
	}

private:
	SDL_Texture* pic_;
	std::string file_name_;
	SDL_Rect box_on_file_;
	const SDL_Rect box_on_window_;
	const double move_this_much_x_;
	const double move_this_much_y_;
};

extern std::map<int, Texture*> pic_map;

void load_pic_map();

class Cool_Background
{
public:
	Cool_Background(Texture* pic) :
		tile_width_(0),
		tile_height_(0),
		number_of_background_tiles_x_(0),
		number_of_background_tiles_y_(0)
	{
		Texture* title_screen_background = pic;
		tile_width_ = title_screen_background->width();
		tile_height_ = title_screen_background->height();
		number_of_background_tiles_x_ = g_screen_width / static_cast<int>(tile_width_);
		number_of_background_tiles_y_ = g_screen_height / static_cast<int>(tile_height_) + 1;
		for (int x = 0; x != number_of_background_tiles_x_; x++)
		{
			std::vector<Displayable_Object*> new_vector;
			for (int y = 0; y != number_of_background_tiles_y_; y++)
			{
				new_vector.push_back(new Displayable_Object(x * tile_width_, y * tile_height_, title_screen_background));
			}
			title_screen_tiles_.push_back(new_vector);
		}
	}

	~Cool_Background()
	{
		for (int x = 0; x != number_of_background_tiles_x_; x++)
			for (int y = 0; y != number_of_background_tiles_y_; y++)
			{
				delete title_screen_tiles_[x][y];
				title_screen_tiles_[x][y] = nullptr;
			}
	}

	inline Displayable_Object* copy_a_tile(int x, int y)
	{
		return title_screen_tiles_[x][y];
	}


	void render()
	{
		for (int x = 0; x != number_of_background_tiles_x_; x++)
			for (int y = 0; y != number_of_background_tiles_y_; y++)
				title_screen_tiles_[x][y]->draw_to_screen();
	}

	void move()
	{
		for (int x = 0; x != number_of_background_tiles_x_; x++)
			for (int y = 0; y != number_of_background_tiles_y_; y++)
			{
				//I think I want the velocity to be const.
				title_screen_tiles_[x][y]->new_location_y(title_screen_tiles_[x][y]->location_y() - g_timer.deltaT_in_seconds() * 50);
				if (title_screen_tiles_[x][y]->location_y() < -tile_height_)
					title_screen_tiles_[x][y]->new_location_y(title_screen_tiles_[x][y]->location_y() + g_screen_height + tile_height_);
				//+ tile_height_ because when I move, a little piece would be missing otherwise.
			}
	}

	int number_of_background_tiles_x()
	{
		return number_of_background_tiles_x_;
	}

	int number_of_background_tiles_y()
	{
		return number_of_background_tiles_y_;
	}

private:
	double tile_width_;
	double tile_height_;
	int number_of_background_tiles_x_;
	int number_of_background_tiles_y_;
	std::vector<std::vector<Displayable_Object*>> title_screen_tiles_;
};

#endif
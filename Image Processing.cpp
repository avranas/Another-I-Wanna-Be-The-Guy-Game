#include "stdafx.h"
#include "Image Processing.h"
#include "Worlds and Maps.h"

namespace Game
{
	Camera* camera = new Camera;
}

void Camera::logic()
{
	if (start_time_ != 0)
	{
		momentum_ = (static_cast<double>(timer_) / (static_cast<double>(start_time_)) * start_momentum_);
		timer_ -= g_timer.deltaT();
	}
	if (minimum_momentum_ != 0)
	{
		if (momentum_ <= minimum_momentum_)
		{
			momentum_ = minimum_momentum_;
			minimum_momentum_ = 0;
		}
	}
	set_rand_offset(momentum_);
	if (timer_ <= 0)
	{
		start_momentum_ = 0;
		momentum_ = 0;
		start_time_ = 0;
		timer_ = 0;
		offset_x_ = 0;
		offset_y_ = 0;
	}
}

SDL_Texture* load_texture(string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;
	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(g_renderer, loadedSurface);
		if (newTexture == NULL)
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;
}

Texture::Texture(string pic_file_name, string data_file_name) :
	pic_file_name_("Images/" + pic_file_name),
	data_file_name_("Images/" + data_file_name),
	initialized_(false),
	setback_x_(0),
	setback_y_(0)
{
	//2022 Comment - Not sure why every Texture needs a .txt file but uhh okay
	//After the image is loaded into memory, we read the Texture data from a .txt file.
	//The following code explains how data is interpreted.
	ifstream animation_data(data_file_name_);
	//First input the frame rate. Enter 0 if the object isn't animated.
	animation_data >> frame_rate_;
	//This will happen if the file can't be read.
	if (frame_rate_ == -6.2774385622041925e+066)
		cout << "There's a problem loading data for animation '" << data_file_name_ << endl;
	//Second: total number of frames; 0 if it's not animated.
	animation_data >> number_of_frames_;
	//Third: width in pixels of the animation
	animation_data >> width_;
	//Fourth: height in pixels of the animation
	animation_data >> height_;
	//Five: The setback from the POINT of location, to the point where the object is displayed.
	animation_data >> setback_x_;
	animation_data >> setback_y_;
	//Next: x and y positions on the png file
	for (int counter = 0; counter <= number_of_frames_; counter++)
	{
		SDL_Point new_frame;
		animation_data >> new_frame.x;
		animation_data >> new_frame.y;
		location_on_sprite_sheet_.push_back(new_frame);
	}
	//Last: Does the animation loop? 1 for yes, 0 for no.
	//If the pic isn't animated, this won't matter since animation() will never be called.
	animation_data >> loop_;
	if (number_of_frames_ == 0)
		loop_ = false;
}

Texture::Texture(SDL_Texture* new_texture, int width, int height) :
	pic_file_name_("0"),
	data_file_name_("0"),
	initialized_(false),
	setback_x_(0),
	setback_y_(0)
{
	pic_ = new_texture;
	width_ = width;
	height_ = height;
	SDL_Point new_point;
	new_point.x = 0;
	new_point.y = 0;
	location_on_sprite_sheet_.push_back(new_point);
	loop_ = false;
	number_of_frames_ = 0;
	int frame_rate_ = 0;
	setback_x_ = 0;
	setback_y_ = 0;
}

Displayable_Object::Displayable_Object(double x, double y) :
	location_(Point(x, y)),
	current_animation_(0),
	direction_(SDL_FLIP_NONE),
	current_frame_(0),
	time_in_current_frame_(0),
	animation_over_(false),
	angle_(0),
	height_ratio_(1),
	width_ratio_(1),
	ready_to_delete_(false),
	temp_position_(0)
{}

//Copied from above.
Displayable_Object::Displayable_Object(double x, double y, Texture* the_pic) :
	location_(Point(x, y)),
	current_animation_(0),
	direction_(SDL_FLIP_NONE),
	current_frame_(0),
	time_in_current_frame_(0),
	animation_over_(false),
	angle_(0),
	height_ratio_(1),
	width_ratio_(1),
	ready_to_delete_(false),
	temp_position_(0)
{
	add_new_texture(the_pic);
}

//Copied from above again. Only difference is the angle
Displayable_Object::Displayable_Object(double x, double y, Texture* the_pic, double angle) :
	location_(Point(x, y)),
	current_animation_(0),
	direction_(SDL_FLIP_NONE),
	current_frame_(0),
	time_in_current_frame_(0),
	animation_over_(false),
	angle_(angle),
	height_ratio_(1),
	width_ratio_(1),
	ready_to_delete_(false),
	temp_position_(0)
{
	add_new_texture(the_pic);
}

void Displayable_Object::animate()
{
	if (the_pics_[current_animation_]->number_of_frames() == 0)
		return;
	if (animation_over_)
		return;
	time_in_current_frame_ += g_timer.deltaT();
	if (time_in_current_frame_ >= time_per_frame(the_pics_[current_animation_]->frame_rate()))
	{
		time_in_current_frame_ -= time_per_frame(the_pics_[current_animation_]->frame_rate());
		current_frame_++;
		if (current_frame_ > the_pics_[current_animation_]->number_of_frames())
		{
			if (the_pics_[current_animation_]->loop())
				current_frame_ = 0;
			else
			{
				animation_over_ = true;
				current_frame_--;
			}
		}
	}
}

namespace Pic
{
	Texture* no_pic = new Texture("no_pic.png", "no_pic.txt");
	Texture* the_kid_idle = new Texture("The Kid Idle.png", "The Kid Idle.txt");
	Texture* the_kid_walking = new Texture("The Kid Walking.png", "The Kid Walking.txt");
	Texture* the_kid_jumping = new Texture("The Kid Jumping.png", "The Kid Jumping.txt");
	Texture* the_kid_falling = new Texture("The Kid Falling.png", "The Kid Falling.txt");
	Texture* moon = new Texture("Moon.png", "Moon.txt");
	Texture* reverse_moon = new Texture("Moon.png", "Reverse Moon.txt");
	Texture* spike = new Texture("Spike.png", "Spike.txt");
	Texture* blank_tile = new Texture("blank_tile.png", "All Tiles.txt");
	Texture* block = new Texture("block.png", "All Tiles.txt");
	Texture* blue_tile = new Texture("Blue Tile.png", "All Tiles.txt");
	Texture* bullet = new Texture("Bullet.png", "Bullet.txt");
	Texture* buzzsaw = new Texture("Buzzsaw.png", "Buzzsaw.txt");
	Texture* chain = new Texture("Chain.png", "Chain.txt");
	Texture* plaque = new Texture("Plaque.png", "Plaque.txt");
	Texture* save_box_idle = new Texture("Save Box Idle.png", "Save Box Idle.txt");
	Texture* save_box_saving = new Texture("Save Box Saving.png", "Save Box Saving.txt");
	Texture* game_over = new Texture("Game Over.png", "Game Over.txt");
	Texture* Blood_1 = new Texture("Blood 1.png", "Blood.txt");
	Texture* Blood_2 = new Texture("Blood 2.png", "Blood.txt");
	Texture* Blood_3 = new Texture("Blood 3.png", "Blood.txt");
	Texture* Dead_Kid_1 = new Texture("Dead Kid 1.png", "Dead Kid 1.txt");
	Texture* Dead_Kid_2 = new Texture("Dead Kid 2.png", "Dead Kid 2.txt");
	Texture* Dead_Kid_3 = new Texture("Dead Kid 3.png", "Dead Kid 3.txt");
	Texture* platform_1 = new Texture("Platform 1 Pic.png", "Platform 1 Pic.txt");
	Texture* doorway_idle = new Texture("Doorway Idle.png", "Doorway.txt");
	Texture* doorway_open = new Texture("Doorway Open.png", "Doorway.txt");
	Texture* gate_closed = new Texture("Gate Closed.png", "Gate.txt");
	Texture* gate_opening = new Texture("Gate Opening.png", "Gate Opening.txt");
	Texture* gate_open = new Texture("Gate Open.png", "Gate.txt");
	Texture* invisible_tile = new Texture("Invisible_tile.png", "All Tiles.txt");
	Texture* falling_platform_idle = new Texture("Falling Platform Idle.png", "Falling Platform Idle.txt");
	Texture* falling_platform_falling = new Texture("Falling Platform Falling.png", "Falling Platform Falling.txt");
	Texture* falling_platform_fallen = new Texture("Falling Platform Fallen.png", "Falling Platform Fallen.txt");
	Texture* falling_platform_falling_reverse = new Texture("Falling Platform Falling Reverse.png", "Falling Platform Falling Reverse.txt");
	Texture* falling_platform_fallen_reverse = new Texture("Falling Platform Fallen Reverse.png", "Falling Platform Fallen.txt");
	Texture* carock_idle = new Texture("Carock Idle.png", "Carock Idle.txt");
	Texture* carock_idle_alt = new Texture("Carock Idle Alt.png", "Carock Idle.txt");
	Texture* carock_casting = new Texture("Carock Casting.png", "Carock Casting.txt");
	Texture* carock_casting_alt = new Texture("Carock Casting Alt.png", "Carock Casting.txt");
	Texture* magic_projectile = new Texture("Magic Projectile.png", "Magic Projectile.txt");
	Texture* frog_idle = new Texture("Frog Idle.png", "Frog Idle.txt");
	Texture* frog_jumping = new Texture("Frog Jumping.png", "Frog Jumping.txt");
	Texture* door_tile = new Texture("Door Tile.png", "All Tiles.txt");
	Texture* door_tile_edge = new Texture("Door Tile Edge.png", "All Tiles.txt");
	Texture* yellow_switch_tile = new Texture("Yellow Switch Tile.png", "All Tiles.txt");
	Texture* yellow_switch_idle = new Texture("Yellow Switch.png", "Switch.txt");
	Texture* yellow_switch_pressed = new Texture("Yellow Switch Pressed.png", "Switch Pressed.txt");
	Texture* green_switch_idle = new Texture("Green Switch.png", "Switch.txt");
	Texture* green_switch_pressed = new Texture("Green Switch Pressed.png", "Switch Pressed.txt");
	Texture* lava_top_tile = new Texture("Lava Top Tile.png", "All Tiles.txt");
	Texture* lava_tile = new Texture("Lava Tile.png", "All Tiles.txt");
	Texture* bridge_tile = new Texture("Bridge Tile.png", "All Tiles.txt");
	Texture* empty_green_tile = new Texture("Empty Green Tile.png", "All Tiles.txt");
	Texture* empty_yellow_tile = new Texture("Empty Yellow Tile.png", "All Tiles.txt");
	Texture* green_switch_tile = new Texture("Green Switch Tile.png", "All Tiles.txt");
	Texture* axe = new Texture("Axe.png", "Axe.txt");
	Texture* hole_in_the_wall = new Texture("Hole in the Wall.png", "Switch.txt");
	Texture* big_kid = new Texture("Big Kid.png", "Big Kid.txt");
	Texture* finger = new Texture("Finger.png", "Finger.txt");
	Texture* title = new Texture("Title Pic.png", "Title Pic.txt");
	Texture* barry_idle = new Texture("Barry Idle.png", "Barry Idle.txt");
	Texture* barry_idle_alt = new Texture("Barry Idle Alt.png", "Barry Idle.txt");
	Texture* barry_attack = new Texture("Barry Attack.png", "Barry Attack.txt");
	Texture* barry_attack_alt = new Texture("Barry Attack Alt.png", "Barry Attack.txt");
	Texture* barry_dying = new Texture("Barry Dying.png", "Barry Attack.txt");
	Texture* boss_bar_text = new Texture("Boss Bar Text.png", "Boss Bar Text.txt");
	Texture* boss_bar_frame = new Texture("Boss Bar Frame.png", "Boss Bar Frame.txt");
	Texture* health = new Texture("Health.png", "Health.txt");
	Texture* anti_health = new Texture("Anti Health.png", "Health.txt");
	Texture* spring_idle = new Texture("Spring Idle.png", "Spring Idle.txt");
	Texture* spring_active = new Texture("Spring Active.png", "Spring Active.txt");
	Texture* hadouken = new Texture("Hadouken.png", "Hadouken.txt");
	Texture* fireball = new Texture("Fireball.png", "Fireball.txt");
	Texture* thundercloud_waiting = new Texture("Thundercloud Waiting.png", "Thundercloud.txt");
	Texture* thundercloud_warning = new Texture("Thundercloud Warning.png", "Thundercloud Warning.txt");
	Texture* thundercloud_active = new Texture("Thundercloud Active.png", "Thundercloud.txt");
	Texture* thunderbolt = new Texture("Thunderbolt.png", "Thunderbolt.txt");
	Texture* big_fireball = new Texture("Big Fireball.png", "Big Fireball.txt");
	Texture* thrown_rock = new Texture("Thrown Rock.png", "Thrown Rock.txt");
	Texture* wuss_box_idle = new Texture("Wuss Box Idle.png", "Save Box Idle.txt");
	Texture* wuss_box_saving = new Texture("Wuss Box Saving.png", "Save Box Saving.txt");
	Texture* controller_warning = new Texture("Controller Warning.png", "Controller Warning.txt");
	Texture* trolley_idle = new Texture("Trolley Idle.png", "Trolley Idle.txt");
	Texture* trolley_moving_left = new Texture("Trolley Moving Left.png", "Trolley Idle.txt");
	Texture* trolley_moving_right = new Texture("Trolley Moving Right.png", "Trolley Idle.txt");
	Texture* windmill = new Texture("Windmill.png", "Windmill.txt");
	Texture* link_shoveling = new Texture("Link Shoveling.png", "Link Shoveling.txt");
	Texture* empty_blue_tile = new Texture("Empty Blue Tile.png", "All Tiles.txt");
	Texture* explosion1 = new Texture("Explosion 1.png", "Explosion.txt");
	Texture* explosion2 = new Texture("Explosion 2.png", "Explosion.txt");
	Texture* explosion3 = new Texture("Explosion 3.png", "Explosion.txt");
	Texture* explosion4 = new Texture("Explosion 4.png", "Explosion.txt");
	Texture* explosion5 = new Texture("Explosion 5.png", "Explosion.txt");
	Texture* pounder_idle = new Texture("Pounder Idle.png", "Axe.txt");
	Texture* pounder_idle_alt = new Texture("Pounder Idle Alt.png", "Axe.txt");
	Texture* pounder_top_fist = new Texture("Pounder Top Fist.png", "Pounder Top Fist.txt");
	Texture* pounder_top_fist_alt = new Texture("Pounder Top Fist Alt.png", "Pounder Top Fist.txt");
	Texture* pounder_side_fist = new Texture("Pounder Side Fist.png", "Pounder Side Fist.txt");
	Texture* pounder_side_fist_alt = new Texture("Pounder Side Fist Alt.png", "Pounder Side Fist.txt");
	Texture* shockwave = new Texture("Shockwave.png", "Shockwave.txt");
	Texture* one_way_platform_1 = new Texture("One-Way Platform 1.png", "One-Way Platform 1.txt");
	Texture* mini_axe = new Texture("Mini Axe.png", "Switch.txt");
	Texture* spike_wall = new Texture("Spike Wall.png", "Spike Wall.txt");
	Texture* missile = new Texture("Missile.png", "Missile.txt");
	Texture* metroid_door_idle = new Texture("Metroid Door Idle.png", "Metroid Door.txt");
	Texture* metroid_door_flashing = new Texture("Metroid Door Flashing.png", "Metroid Door.txt");
	Texture* metroid_door_opening = new Texture("Metroid Door Opening.png", "Metroid Door Opening.txt");
	Texture* metroid_door_back = new Texture("Metroid Door Back.png", "Metroid Door Back.txt");
	Texture* house = new Texture("House.png", "House.txt");
	Texture* save_switch_idle = new Texture("Save Switch Idle.png", "Switch.txt");
	Texture* save_switch_pressed = new Texture("Save Switch Pressed.png", "Switch Pressed.txt");
	Texture* wuss_switch_idle = new Texture("Wuss Switch Idle.png", "Switch.txt");
	Texture* wuss_switch_pressed = new Texture("Wuss Switch Pressed.png", "Switch Pressed.txt");
	Texture* falling_spike = new Texture("Falling Spike.png", "Falling Spike.txt");
	Texture* platform_2 = new Texture("Moving Platform 2.png", "Moving Platform 2.txt");
	Texture* crushing_spikes = new Texture("Crushing Spikes.png", "Crushing Spikes.txt");
	Texture* chasing_spikes = new Texture("Chasing Spikes.png", "Chasing Spikes.txt");
	Texture* spike_pillar_base = new Texture("Spike Pillar Base.png", "Spike Pillar Base.txt");
	Texture* x_key = new Texture("X Key.png", "Switch.txt");
	Texture* z_key = new Texture("Z Key.png", "Switch.txt");
	Texture* t_key = new Texture("T Key.png", "Switch.txt");
	Texture* arrow_key = new Texture("Arrow Key.png", "Switch.txt");
	Texture* boss_platform = new Texture("Boss Platform.png", "Boss Platform.txt");
	Texture* sparkle = new Texture("Sparkle.png", "Sparkle.txt");
	Texture* mushroom = new Texture("Mushroom.png", "Mushroom.txt");
	Texture* smoke = new Texture("Smoke.png", "Smoke.txt");
	Texture* star = new Texture("Star.png", "Star.txt");
	//Texture* star_hud_empty = new Texture("Star HUD Empty.png", "Star HUD.txt");
	Texture* star_hud_filled = new Texture("Star HUD Filled.png", "Star HUD.txt");
	Texture* fake_mushroom = new Texture("Fake Mushroom.png", "Axe.txt");
	Texture* super_spring_idle = new Texture("Super Spring.png", "Super Spring.txt");
	Texture* super_spring_active = new Texture("Super Spring Active.png", "Super Spring Active.txt");
	Texture* super_spring_fail = new Texture("Super Spring Fail.png", "Super Spring Fail.txt");
	Texture* pipe = new Texture("Pipe.png", "Pipe.txt");
	Texture* teleporter = new Texture("Teleporter.png", "Teleporter.txt");
	Texture* teleporter_hud_filled = new Texture("Teleporter HUD Filled.png", "Teleporter HUD.txt");
	//Texture* teleporter_hud_empty = new Texture("Teleporter HUD Empty.png", "Teleporter HUD.txt");
	Texture* fake_mushroom_2 = new Texture("Fake Mushroom 2.png", "Axe.txt");
	Texture* big_pipe = new Texture("Big Pipe.png", "Axe.txt");
	Texture* reverse_big_pipe = new Texture("Reverse Big Pipe.png", "Axe.txt");
	Texture* congrats = new Texture("Congrats.png", "Congrats.txt");
	Texture* portal = new Texture("Portal.png", "Portal.txt");
	Texture* title_background = new Texture("Title Screen Background.png", "Title Screen Background.txt");
	Texture* alex_2 = new Texture("Alex 2.png", "Axe.txt");
	Texture* rising_spikes = new Texture("Rising Spikes.png", "Rising Spikes.txt");
	Texture* nope = new Texture("Nope.png", "Nope.txt");
	Texture* firework = new Texture("Firework.png", "Firework.txt");
	Texture* trail = new Texture("Trail.png", "Trail.txt"); //I screwed up here. Don't feel like fixing.
	Texture* spark = new Texture("Trail.png", "Spark.txt");
	Texture* ball = new Texture("ball.png", "ball.txt");
	Texture* bar = new Texture("slider.png", "slider.txt");
}

//Use this if you need to find a specific Texture* at runtime.
std::map<int, Texture*> pic_map;

void load_pic_map()
{
	using namespace Pic;
	pic_map[0] = blue_tile;
}

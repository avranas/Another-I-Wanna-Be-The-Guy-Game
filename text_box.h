#ifndef TEXT_BOX_H_
#define TEXT_BOX_H_

#include "Image Processing.h"
#include "globals.h"
#include <SDL_ttf.h>

namespace Font
{
	extern TTF_Font* Calibri_24;
}

namespace Color
{
	extern SDL_Color* black;
	extern SDL_Color* white;
	extern SDL_Color* red;
	extern SDL_Color* green;
	extern SDL_Color* blue;
	extern SDL_Color* important_message;
	extern SDL_Color* title_choices;
	extern SDL_Color* file_names;
	extern SDL_Color* file_info;
}

class Text_Box : public Displayable_Object
{
public:
	Text_Box(double x, double y, const char* the_text,
		TTF_Font* the_font, SDL_Color* the_color, double size_ratio) :
		Displayable_Object(x, y),
		the_text_(the_text),
		the_font_(the_font),
		the_color_(the_color)
	{
		width_ratio_ = size_ratio;
		height_ratio_ = size_ratio;
		the_pics_.push_back(nullptr);
		change_the_text(the_text);
	}

	~Text_Box()
	{
		delete the_pics_[0];
		the_pics_[0] = nullptr;
	}

	//Prob: changing the text to "\0" makes the game not work.
	void change_the_text(const char* new_text)
	{
		the_text_ = new_text;
		delete the_pics_[0];
		the_pics_[0] = nullptr;
		SDL_Surface* the_new_surface = nullptr;
		the_new_surface = TTF_RenderText_Solid(the_font_, the_text_.c_str(), *the_color_);
		if (the_new_surface == nullptr)
		{
			cout << "Unable to load image %s! SDL_image Error: %s\n" << endl << IMG_GetError();
		}
		SDL_Texture* temp_sdl_texture = nullptr;
		temp_sdl_texture = SDL_CreateTextureFromSurface(g_renderer, the_new_surface);
		if (temp_sdl_texture == nullptr)
		{
			cout << "Unable to load image %s! SDL_image Error: %s\n" << endl << IMG_GetError();
		}
		Texture* new_texture = nullptr;
		new_texture = new Texture(temp_sdl_texture, the_new_surface->w, the_new_surface->h);
		the_pics_[0] = new_texture;
		SDL_FreeSurface(the_new_surface);
	}
	string the_text()
	{
		return the_text_;
	}

private:
	string the_text_;
	TTF_Font* the_font_;
	SDL_Color* the_color_;
};

#endif

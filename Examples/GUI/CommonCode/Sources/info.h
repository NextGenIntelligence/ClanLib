/*
**  ClanLib SDK
**  Copyright (c) 1997-2013 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Mark Page
*/

#pragma once

class Info : public clan::GUIComponent
{
public:
	Info(clan::GUIComponent *parent);
	~Info();
	
	void set(int new_xpos, int new_ypos, const std::string &new_name);

	void set_comment(std::string &new_comment);

	void draw(clan::Canvas &canvas);

	void activate();

	void on_render(clan::Canvas &canvas, const clan::Rect &update_rect);

private:
	void calculate_geometry();

private:
	std::string name;
	std::string comment;

	unsigned int activated_time;
	bool activated_flag;
};


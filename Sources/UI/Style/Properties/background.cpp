/*
**  ClanLib SDK
**  Copyright (c) 1997-2015 The ClanLib Team
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
**    Magnus Norddahl
*/

#include "UI/precomp.h"
#include "background.h"

namespace clan
{
	BackgroundPropertyParser style_parser_background;
	BackgroundAttachmentPropertyParser style_parser_background_attachment;
	BackgroundClipPropertyParser style_parser_background_clip;
	BackgroundColorPropertyParser style_parser_background_color;
	BackgroundImagePropertyParser style_parser_background_image;
	BackgroundOriginPropertyParser style_parser_background_origin;
	BackgroundPositionPropertyParser style_parser_background_position;
	BackgroundRepeatPropertyParser style_parser_background_repeat;
	BackgroundSizePropertyParser style_parser_background_size;

	void force_link_style_parser_background()
	{
	}

	void BackgroundPropertyParser::parse(StylePropertySetter *setter, const std::string &name, const std::string &value, const std::initializer_list<StylePropertyInitializerValue> &args)
	{
		std::vector<StyleToken> tokens = StyleTokenizer::tokenize(value);

		StyleValue bgcolor;
		StyleValue bgimage;
		std::vector<StyleValue> bgimages;
		StyleValue bgrepeat;
		std::vector<StyleValue> bgrepeatsx;
		std::vector<StyleValue> bgrepeatsy;
		StyleValue bgattachment;
		std::vector<StyleValue> bgattachments;
		StyleValue bgposition;
		std::vector<StyleValue> bgpositionsx;
		std::vector<StyleValue> bgpositionsy;
		StyleValue bgorigin;
		std::vector<StyleValue> bgorigins;
		StyleValue bgclip;
		std::vector<StyleValue> bgclips;
		StyleValue bgsize;
		std::vector<StyleValue> bgsizesx;
		std::vector<StyleValue> bgsizesy;

		if (tokens.size() == 1 && tokens[0].type == StyleTokenType::ident && equals(tokens[0].value, "inherit"))
		{
			bgcolor = StyleValue::from_keyword("inherit");
			bgimage = StyleValue::from_keyword("inherit");
			bgrepeat = StyleValue::from_keyword("inherit");
			bgattachment = StyleValue::from_keyword("inherit");
			bgposition = StyleValue::from_keyword("inherit");
			bgorigin = StyleValue::from_keyword("inherit");
			bgclip = StyleValue::from_keyword("inherit");
			bgsize = StyleValue::from_keyword("inherit");

			setter->set_value("background-color", bgcolor);
			setter->set_value("background-image", bgimage);
			setter->set_value_array("background-images", bgimages);
			setter->set_value("background-repeat", bgrepeat);
			setter->set_value_array("background-repeats-x", bgrepeatsx);
			setter->set_value_array("background-repeats-y", bgrepeatsy);
			setter->set_value("background-attachment", bgattachment);
			setter->set_value_array("background-attachments", bgattachments);
			setter->set_value("background-position", bgposition);
			setter->set_value_array("background-positions-x", bgpositionsx);
			setter->set_value_array("background-positions-y", bgpositionsy);
			setter->set_value("background-origin", bgorigin);
			setter->set_value_array("background-origins", bgorigins);
			setter->set_value("background-clip", bgclip);
			setter->set_value_array("background-clips", bgclips);
			setter->set_value("background-size", bgsize);
			setter->set_value_array("background-sizes-x", bgsizesx);
			setter->set_value_array("background-sizes-y", bgsizesy);
			return;
		}

		size_t pos = 0;
		while (true) // for each layer
		{
			bool color_specified = false;
			bool image_specified = false;
			bool repeat_specified = false;
			bool attachment_specified = false;
			bool position_specified = false;
			bool boxes_specified = false;

			StyleValue layer_image = StyleValue::from_keyword("none");
			StyleValue layer_repeat_x = StyleValue::from_keyword("repeat");
			StyleValue layer_repeat_y = StyleValue::from_keyword("repeat");
			StyleValue layer_attachment = StyleValue::from_keyword("scroll");
			StyleValue layer_position_x = StyleValue::from_percentage(0.0f);
			StyleValue layer_position_y = StyleValue::from_percentage(0.0f);
			StyleValue layer_origin = StyleValue::from_keyword("padding-box");
			StyleValue layer_clip = StyleValue::from_keyword("border-box");
			StyleValue layer_size_x = StyleValue::from_keyword("auto");
			StyleValue layer_size_y = StyleValue::from_keyword("auto");

			while (true) // for each declaration in layer
			{
				if (!color_specified && parse_bgcolor(bgcolor, pos, tokens))
				{
					color_specified = true;
				}
				else if (!image_specified && parse_image(layer_image, pos, tokens))
				{
					image_specified = true;
				}
				else if (!repeat_specified && parse_repeat(layer_repeat_x, layer_repeat_y, pos, tokens))
				{
					repeat_specified = true;
				}
				else if (!attachment_specified && parse_attachment(layer_attachment, pos, tokens))
				{
					attachment_specified = true;
				}
				else if (!position_specified && parse_position(layer_position_x, layer_position_y, pos, tokens))
				{
					parse_size(layer_size_x, layer_size_y, pos, tokens);
					position_specified = true;
				}
				else if (!boxes_specified && parse_boxes(layer_origin, layer_clip, pos, tokens))
				{
					boxes_specified = true;
				}
				else
				{
					return;
				}

				if (pos == tokens.size())
					break;

				size_t peek_pos = pos;
				StyleToken token = next_token(peek_pos, tokens);
				if (token.type == StyleTokenType::delim && token.value == ",")
					break;
			}

			bgimages.push_back(layer_image);
			bgrepeatsx.push_back(layer_repeat_x);
			bgrepeatsy.push_back(layer_repeat_y);
			bgattachments.push_back(layer_attachment);
			bgpositionsx.push_back(layer_position_x);
			bgpositionsy.push_back(layer_position_y);
			bgorigins.push_back(layer_origin);
			bgclips.push_back(layer_clip);
			bgsizesx.push_back(layer_size_x);
			bgsizesy.push_back(layer_size_y);

			if (pos == tokens.size())
				break;

			StyleToken token = next_token(pos, tokens);
			if (token.type != StyleTokenType::delim || token.value != "," || color_specified)
				return;
		}

		setter->set_value("background-color", bgcolor);
		setter->set_value("background-image", bgimage);
		setter->set_value_array("background-images", bgimages);
		setter->set_value("background-repeat", bgrepeat);
		setter->set_value_array("background-repeats-x", bgrepeatsx);
		setter->set_value_array("background-repeats-y", bgrepeatsy);
		setter->set_value("background-attachment", bgattachment);
		setter->set_value_array("background-attachments", bgattachments);
		setter->set_value("background-position", bgposition);
		setter->set_value_array("background-positions-x", bgpositionsx);
		setter->set_value_array("background-positions-y", bgpositionsy);
		setter->set_value("background-origin", bgorigin);
		setter->set_value_array("background-origins", bgorigins);
		setter->set_value("background-clip", bgclip);
		setter->set_value_array("background-clips", bgclips);
		setter->set_value("background-size", bgsize);
		setter->set_value_array("background-sizes-x", bgsizesx);
		setter->set_value_array("background-sizes-y", bgsizesy);
	}

	bool BackgroundPropertyParser::parse_bgcolor(StyleValue &bgcolor, size_t &parse_pos, const std::vector<StyleToken> &tokens)
	{
		size_t pos = parse_pos;
		Colorf color;
		if (parse_color(tokens, pos, color))
		{
			bgcolor = StyleValue::from_color(color);
			parse_pos = pos;
			return true;
		}
		return false;
	}

	bool BackgroundPropertyParser::parse_image(StyleValue &layer_image, size_t &parse_pos, const std::vector<StyleToken> &tokens)
	{
		size_t pos = parse_pos;
		StyleToken token = next_token(pos, tokens);
		if (token.type == StyleTokenType::ident && equals(token.value, "none"))
		{
			layer_image = StyleValue::from_keyword("none");
			parse_pos = pos;
			return true;
		}
		else if (token.type == StyleTokenType::uri)
		{
			layer_image = StyleValue::from_url(token.value);
			parse_pos = pos;
			return true;
		}
		return false;
	}

	bool BackgroundPropertyParser::parse_repeat(StyleValue &layer_repeat_x, StyleValue &layer_repeat_y, size_t &parse_pos, const std::vector<StyleToken> &tokens)
	{
		size_t pos = parse_pos;
		StyleToken token = next_token(pos, tokens);

		if (token.type != StyleTokenType::ident)
			return false;

		StyleValue repeat_x, repeat_y;
		bool single_style = false;

		if (equals(token.value, "repeat"))
		{
			repeat_x = StyleValue::from_keyword("repeat");
		}
		else if (equals(token.value, "repeat-x"))
		{
			repeat_x = StyleValue::from_keyword("repeat");
			repeat_y = StyleValue::from_keyword("no-repeat");
			single_style = true;
		}
		else if (equals(token.value, "repeat-y"))
		{
			repeat_x = StyleValue::from_keyword("no-repeat");
			repeat_y = StyleValue::from_keyword("repeat");
			single_style = true;
		}
		else if (equals(token.value, "no-repeat"))
		{
			repeat_x = StyleValue::from_keyword("no-repeat");
		}
		else if (equals(token.value, "space"))
		{
			repeat_x = StyleValue::from_keyword("space");
		}
		else if (equals(token.value, "round"))
		{
			repeat_x = StyleValue::from_keyword("round");
		}
		else
		{
			return false;
		}

		if (pos == tokens.size() || single_style)
		{
			if (!single_style)
				repeat_y = repeat_x;

			layer_repeat_x = repeat_x;
			layer_repeat_y = repeat_y;
			parse_pos = pos;
			return true;
		}

		parse_pos = pos;
		repeat_y = repeat_x;

		token = next_token(pos, tokens);
		if (token.type == StyleTokenType::ident)
		{
			if (equals(token.value, "repeat"))
			{
				repeat_y = StyleValue::from_keyword("repeat");
				parse_pos = pos;
			}
			else if (equals(token.value, "no-repeat"))
			{
				repeat_y = StyleValue::from_keyword("no-repeat");
				parse_pos = pos;
			}
			else if (equals(token.value, "space"))
			{
				repeat_y = StyleValue::from_keyword("space");
				parse_pos = pos;
			}
			else if (equals(token.value, "round"))
			{
				repeat_y = StyleValue::from_keyword("round");
				parse_pos = pos;
			}
		}

		layer_repeat_x = repeat_x;
		layer_repeat_y = repeat_y;
		return true;
	}

	bool BackgroundPropertyParser::parse_attachment(StyleValue &layer_attachment, size_t &parse_pos, const std::vector<StyleToken> &tokens)
	{
		size_t pos = parse_pos;
		StyleToken token = next_token(pos, tokens);
		if (token.type != StyleTokenType::ident)
			return false;

		if (equals(token.value, "scroll"))
			layer_attachment = StyleValue::from_keyword("scroll");
		else if (equals(token.value, "fixed"))
			layer_attachment = StyleValue::from_keyword("fixed");
		else
			return false;

		parse_pos = pos;
		return true;
	}

	bool BackgroundPropertyParser::parse_position(StyleValue &layer_position_x, StyleValue &layer_position_y, size_t &parse_pos, const std::vector<StyleToken> &tokens)
	{
		size_t last_pos = parse_pos;
		size_t pos = last_pos;
		StyleToken token = next_token(pos, tokens);

		StyleValue bg_pos_x, bg_pos_y;
		bool x_specified = false;
		bool y_specified = false;
		bool center_specified = false;
		while (true)
		{
			if (token.type == StyleTokenType::ident)
			{
				if (!y_specified && equals(token.value, "top"))
				{
					bg_pos_y = StyleValue::from_keyword("top");
					y_specified = true;

					if (center_specified)
					{
						bg_pos_x = StyleValue::from_keyword("center");
						x_specified = true;
						center_specified = false;
					}
				}
				else if (!y_specified && equals(token.value, "bottom"))
				{
					bg_pos_y = StyleValue::from_keyword("bottom");
					y_specified = true;

					if (center_specified)
					{
						bg_pos_x = StyleValue::from_keyword("center");
						x_specified = true;
						center_specified = false;
					}
				}
				else if (!x_specified && equals(token.value, "left"))
				{
					bg_pos_x = StyleValue::from_keyword("left");
					x_specified = true;

					if (center_specified)
					{
						bg_pos_y = StyleValue::from_keyword("center");
						y_specified = true;
						center_specified = false;
					}
				}
				else if (!x_specified && equals(token.value, "right"))
				{
					bg_pos_x = StyleValue::from_keyword("right");
					x_specified = true;

					if (center_specified)
					{
						bg_pos_y = StyleValue::from_keyword("center");
						y_specified = true;
						center_specified = false;
					}
				}
				else if (equals(token.value, "center"))
				{
					if (center_specified)
					{
						bg_pos_x = StyleValue::from_keyword("center");
						x_specified = true;
						center_specified = false;
					}

					if (x_specified && !y_specified)
					{
						bg_pos_y = StyleValue::from_keyword("center");
						y_specified = true;
					}
					else if (y_specified && !x_specified)
					{
						bg_pos_x = StyleValue::from_keyword("center");
						x_specified = true;
					}
					else if (!x_specified && !y_specified)
					{
						center_specified = true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					break;
				}
			}
			else if (is_length(token))
			{
				StyleValue length;
				if (parse_length(token, length))
				{
					if (center_specified)
					{
						bg_pos_x = StyleValue::from_keyword("center");
						x_specified = true;
						center_specified = false;
					}

					if (!x_specified && !y_specified)
					{
						bg_pos_x = length;
						x_specified = true;
					}
					else if (x_specified && !y_specified)
					{
						bg_pos_y = length;
						y_specified = true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else if (token.type == StyleTokenType::percentage)
			{
				if (center_specified)
				{
					bg_pos_x = StyleValue::from_keyword("center");
					x_specified = true;
					center_specified = false;
				}

				if (!x_specified && !y_specified)
				{
					bg_pos_x = StyleValue::from_percentage(StringHelp::text_to_float(token.value));
					x_specified = true;
				}
				else if (x_specified && !y_specified)
				{
					bg_pos_y = StyleValue::from_percentage(StringHelp::text_to_float(token.value));
					y_specified = true;
				}
				else
				{
					return false;
				}
			}
			else if (token.type == StyleTokenType::delim && token.value == "-")
			{
				token = next_token(pos, tokens);
				if (is_length(token))
				{
					StyleValue length;
					if (parse_length(token, length))
					{
						length.number = -length.number;
						if (center_specified)
						{
							bg_pos_x = StyleValue::from_keyword("center");
							x_specified = true;
							center_specified = false;
						}

						if (!x_specified && !y_specified)
						{
							bg_pos_x = length;
							x_specified = true;
						}
						else if (x_specified && !y_specified)
						{
							bg_pos_y = length;
							y_specified = true;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else if (token.type == StyleTokenType::percentage)
				{
					if (center_specified)
					{
						bg_pos_x = StyleValue::from_keyword("center");
						x_specified = true;
						center_specified = false;
					}

					if (!x_specified && !y_specified)
					{
						bg_pos_x = StyleValue::from_percentage(-StringHelp::text_to_float(token.value));
						x_specified = true;
						parse_pos = pos;
					}
					else if (x_specified && !y_specified)
					{
						bg_pos_y = StyleValue::from_percentage(-StringHelp::text_to_float(token.value));
						y_specified = true;
						parse_pos = pos;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				break;
			}

			last_pos = pos;
			token = next_token(pos, tokens);
		}

		if (!x_specified)
			bg_pos_x = StyleValue::from_keyword("center");
		else if (!y_specified)
			bg_pos_y = StyleValue::from_keyword("center");

		parse_pos = last_pos;
		layer_position_x = bg_pos_x;
		layer_position_y = bg_pos_y;
		return true;
	}

	bool BackgroundPropertyParser::parse_size(StyleValue &layer_size_x, StyleValue &layer_size_y, size_t &parse_pos, const std::vector<StyleToken> &tokens)
	{
		size_t pos = parse_pos;
		StyleToken token = next_token(pos, tokens);
		if (token.type != StyleTokenType::delim || token.value != "/")
			return false;

		token = next_token(pos, tokens);

		StyleValue size_x, size_y;
		bool single_value = false;
		if (token.type == StyleTokenType::ident)
		{
			if (equals(token.value, "contain"))
			{
				size_x = StyleValue::from_keyword("contain");
				size_y = StyleValue::from_keyword("contain");
				single_value = true;
			}
			else if (equals(token.value, "cover"))
			{
				size_x = StyleValue::from_keyword("cover");
				size_y = StyleValue::from_keyword("cover");
				single_value = true;
			}
			else if (equals(token.value, "auto"))
			{
				size_x = StyleValue::from_keyword("auto");
			}
			else
			{
				return false;
			}
		}
		else if (is_length(token))
		{
			StyleValue length;
			if (parse_length(token, length))
			{
				size_x = length;
			}
			else
			{
				return false;
			}
		}
		else if (token.type == StyleTokenType::percentage)
		{
			size_x = StyleValue::from_percentage(StringHelp::text_to_float(token.value));
		}
		else
		{
			return false;
		}

		if (pos == tokens.size() || single_value)
		{
			layer_size_x = size_x;
			layer_size_y = size_y;
			parse_pos = pos;
			return true;
		}

		token = next_token(pos, tokens);
		if (token.type == StyleTokenType::ident && equals(token.value, "auto"))
		{
			size_y = StyleValue::from_keyword("auto");
			parse_pos = pos;
		}
		else if (is_length(token))
		{
			StyleValue length;
			if (parse_length(token, length))
			{
				size_y = length;
				parse_pos = pos;
			}
		}
		else if (token.type == StyleTokenType::percentage)
		{
			size_y = StyleValue::from_percentage(StringHelp::text_to_float(token.value));
			parse_pos = pos;
		}

		layer_size_x = size_x;
		layer_size_y = size_y;
		return true;
	}

	bool BackgroundPropertyParser::parse_boxes(StyleValue &layer_origin, StyleValue &layer_clip, size_t &parse_pos, const std::vector<StyleToken> &tokens)
	{
		size_t pos = parse_pos;
		StyleToken token = next_token(pos, tokens);
		if (token.type != StyleTokenType::ident)
			return false;

		if (equals(token.value, "border-box"))
		{
			layer_origin = StyleValue::from_keyword("border-box");
			layer_clip = StyleValue::from_keyword("border-box");
		}
		else if (equals(token.value, "padding-box"))
		{
			layer_origin = StyleValue::from_keyword("padding-box");
			layer_clip = StyleValue::from_keyword("padding-box");
		}
		else if (equals(token.value, "content-box"))
		{
			layer_origin = StyleValue::from_keyword("content-box");
			layer_clip = StyleValue::from_keyword("content-box");
		}
		else
		{
			return false;
		}

		parse_pos = pos;

		token = next_token(pos, tokens);
		if (token.type == StyleTokenType::ident)
		{
			if (equals(token.value, "border-box"))
			{
				layer_clip = StyleValue::from_keyword("border-box");
				parse_pos = pos;
			}
			else if (equals(token.value, "padding-box"))
			{
				layer_clip = StyleValue::from_keyword("padding-box");
				parse_pos = pos;
			}
			else if (equals(token.value, "content-box"))
			{
				layer_clip = StyleValue::from_keyword("content-box");
				parse_pos = pos;
			}
		}

		return true;
	}

	void BackgroundAttachmentPropertyParser::parse(StylePropertySetter *setter, const std::string &name, const std::string &value, const std::initializer_list<StylePropertyInitializerValue> &args)
	{
		std::vector<StyleToken> tokens = StyleTokenizer::tokenize(value);

		size_t pos = 0;
		StyleToken token = next_token(pos, tokens);

		StyleValue attachment;
		std::vector<StyleValue> attachments;

		if (token.type == StyleTokenType::ident && pos == tokens.size() && equals(token.value, "inherit"))
		{
			attachment = StyleValue::from_keyword("inherit");
			setter->set_value("background-attachment", attachment);
			setter->set_value_array("background-attachments", attachments);
		}
		else
		{
			attachment = StyleValue::from_keyword("array");
			attachments.clear();
			while (true)
			{
				if (token.type != StyleTokenType::ident)
					return;

				if (equals(token.value, "scroll"))
					attachments.push_back(StyleValue::from_keyword("scroll"));
				else if (equals(token.value, "fixed"))
					attachments.push_back(StyleValue::from_keyword("fixed"));
				else
					return;

				if (pos != tokens.size())
				{
					token = next_token(pos, tokens);
					if (token.type != StyleTokenType::delim || token.value != ",")
						return;
					token = next_token(pos, tokens);
				}
				else
				{
					break;
				}
			}
			setter->set_value("background-attachment", attachment);
			setter->set_value_array("background-attachments", attachments);
		}
	}

	void BackgroundClipPropertyParser::parse(StylePropertySetter *setter, const std::string &name, const std::string &value, const std::initializer_list<StylePropertyInitializerValue> &args)
	{
		std::vector<StyleToken> tokens = StyleTokenizer::tokenize(value);

		StyleValue background_clip;
		std::vector<StyleValue> background_clips;

		size_t pos = 0;
		StyleToken token = next_token(pos, tokens);

		if (token.type == StyleTokenType::ident && pos == tokens.size() && equals(token.value, "inherit"))
		{
			background_clip = StyleValue::from_keyword("inherit");
			setter->set_value("background-clip", background_clip);
			setter->set_value_array("background-clips", background_clips);
		}
		else
		{
			background_clip = StyleValue::from_keyword("array");
			background_clips.clear();
			while (true)
			{
				if (token.type != StyleTokenType::ident)
					return;

				if (equals(token.value, "border-box"))
				{
					background_clips.push_back(StyleValue::from_keyword("border-box"));
				}
				else if (equals(token.value, "padding-box"))
				{
					background_clips.push_back(StyleValue::from_keyword("padding-box"));
				}
				else if (equals(token.value, "content-box"))
				{
					background_clips.push_back(StyleValue::from_keyword("content-box"));
				}
				else
				{
					return;
				}

				if (pos == tokens.size())
					break;

				token = next_token(pos, tokens);
				if (token.type != StyleTokenType::delim || token.value != ",")
					return;

				token = next_token(pos, tokens);
			}

			setter->set_value("background-clip", background_clip);
			setter->set_value_array("background-clips", background_clips);
		}
	}

	void BackgroundColorPropertyParser::parse(StylePropertySetter *setter, const std::string &name, const std::string &value, const std::initializer_list<StylePropertyInitializerValue> &args)
	{
		std::vector<StyleToken> tokens = StyleTokenizer::tokenize(value);

		StyleValue background_color;

		size_t pos = 0;
		Colorf color;
		if (parse_color(tokens, pos, color) && pos == tokens.size())
		{
			background_color = StyleValue::from_color(color);
			setter->set_value("background-color", background_color);
		}
		else
		{
			StyleToken token = next_token(pos, tokens);
			if (token.type == StyleTokenType::ident && pos == tokens.size())
			{
				if (equals(token.value, "inherit"))
				{
					background_color = StyleValue::from_keyword("inherit");
					setter->set_value("background-color", background_color);
				}
			}
		}
	}

	void BackgroundImagePropertyParser::parse(StylePropertySetter *setter, const std::string &name, const std::string &value, const std::initializer_list<StylePropertyInitializerValue> &args)
	{
		std::vector<StyleToken> tokens = StyleTokenizer::tokenize(value);

		size_t pos = 0;
		StyleToken token = next_token(pos, tokens);

		StyleValue background_image;
		std::vector<StyleValue> background_images;

		if (token.type == StyleTokenType::ident && pos == tokens.size() && equals(token.value, "inherit"))
		{
			background_image = StyleValue::from_keyword("inherit");
			setter->set_value("background-image", background_image);
			setter->set_value_array("background-images", background_images);
		}
		else
		{
			background_image = StyleValue::from_keyword("array");
			background_images.clear();
			while (true)
			{
				if (token.type == StyleTokenType::ident && equals(token.value, "none"))
					background_images.push_back(StyleValue::from_keyword("none"));
				else if (token.type == StyleTokenType::uri)
					background_images.push_back(StyleValue::from_url(token.value));
				else
					return;

				if (pos != tokens.size())
				{
					token = next_token(pos, tokens);
					if (token.type != StyleTokenType::delim || token.value != ",")
						return;
					token = next_token(pos, tokens);
				}
				else
				{
					break;
				}
			}
		}

		setter->set_value("background-image", background_image);
		setter->set_value_array("background-images", background_images);
	}

	void BackgroundOriginPropertyParser::parse(StylePropertySetter *setter, const std::string &name, const std::string &value, const std::initializer_list<StylePropertyInitializerValue> &args)
	{
		std::vector<StyleToken> tokens = StyleTokenizer::tokenize(value);

		StyleValue background_origin;
		std::vector<StyleValue> background_origins;

		size_t pos = 0;
		StyleToken token = next_token(pos, tokens);

		if (token.type == StyleTokenType::ident && pos == tokens.size() && equals(token.value, "inherit"))
		{
			background_origin = StyleValue::from_keyword("inherit");
			setter->set_value("background-origin", background_origin);
			setter->set_value_array("background-origins", background_origins);
		}
		else
		{
			background_origin = StyleValue::from_keyword("array");
			background_origins.clear();
			while (true)
			{
				if (token.type != StyleTokenType::ident)
					return;

				if (equals(token.value, "border-box"))
				{
					background_origin = StyleValue::from_keyword("border-box");
				}
				else if (equals(token.value, "padding-box"))
				{
					background_origin = StyleValue::from_keyword("padding-box");
				}
				else if (equals(token.value, "content-box"))
				{
					background_origin = StyleValue::from_keyword("content-box");
				}
				else
				{
					return;
				}

				if (pos == tokens.size())
					break;

				token = next_token(pos, tokens);
				if (token.type != StyleTokenType::delim || token.value != ",")
					return;

				token = next_token(pos, tokens);
			}
		}

		setter->set_value("background-origin", background_origin);
		setter->set_value_array("background-origins", background_origins);
	}

	void BackgroundPositionPropertyParser::parse(StylePropertySetter *setter, const std::string &name, const std::string &value, const std::initializer_list<StylePropertyInitializerValue> &args)
	{
		std::vector<StyleToken> tokens = StyleTokenizer::tokenize(value);

		StyleValue position;
		std::vector<StyleValue> positions_x;
		std::vector<StyleValue> positions_y;

		size_t pos = 0;
		StyleToken token = next_token(pos, tokens);

		if (token.type == StyleTokenType::ident && equals(token.value, "inherit") && tokens.size() == 1)
		{
			position = StyleValue::from_keyword("inherit");
			setter->set_value("background-position", position);
			setter->set_value_array("background-positions-x", positions_x);
			setter->set_value_array("background-positions-y", positions_y);
			return;
		}

		position = StyleValue::from_keyword("array");

		bool done = false;
		while (!done)
		{
			StyleValue bg_pos_x, bg_pos_y;
			bool x_specified = false;
			bool y_specified = false;
			bool center_specified = false;

			while (true)
			{
				if (token.type == StyleTokenType::ident)
				{
					if (!y_specified && equals(token.value, "top"))
					{
						bg_pos_y = StyleValue::from_keyword("top");
						y_specified = true;

						if (center_specified)
						{
							bg_pos_x = StyleValue::from_keyword("center");
							x_specified = true;
							center_specified = false;
						}
					}
					else if (!y_specified && equals(token.value, "bottom"))
					{
						bg_pos_y = StyleValue::from_keyword("bottom");
						y_specified = true;

						if (center_specified)
						{
							bg_pos_x = StyleValue::from_keyword("center");
							x_specified = true;
							center_specified = false;
						}
					}
					else if (!x_specified && equals(token.value, "left"))
					{
						bg_pos_x = StyleValue::from_keyword("left");
						x_specified = true;

						if (center_specified)
						{
							bg_pos_y = StyleValue::from_keyword("center");
							y_specified = true;
							center_specified = false;
						}
					}
					else if (!x_specified && equals(token.value, "right"))
					{
						bg_pos_x = StyleValue::from_keyword("right");
						x_specified = true;

						if (center_specified)
						{
							bg_pos_y = StyleValue::from_keyword("center");
							y_specified = true;
							center_specified = false;
						}
					}
					else if (equals(token.value, "center"))
					{
						if (center_specified)
						{
							bg_pos_x = StyleValue::from_keyword("center");
							x_specified = true;
							center_specified = false;
						}

						if (x_specified && !y_specified)
						{
							bg_pos_y = StyleValue::from_keyword("center");
							y_specified = true;
						}
						else if (y_specified && !x_specified)
						{
							bg_pos_x = StyleValue::from_keyword("center");
							x_specified = true;
						}
						else if (!x_specified && !y_specified)
						{
							center_specified = true;
						}
						else
						{
							debug_parse_error(name, tokens);
							return;
						}
					}
				}
				else if (is_length(token))
				{
					StyleValue length;
					if (parse_length(token, length))
					{
						if (center_specified)
						{
							bg_pos_x = StyleValue::from_keyword("center");
							x_specified = true;
							center_specified = false;
						}

						if (!x_specified && !y_specified)
						{
							bg_pos_x = length;
							x_specified = true;
						}
						else if (x_specified && !y_specified)
						{
							bg_pos_y = length;
							y_specified = true;
						}
						else
						{
							debug_parse_error(name, tokens);
							return;
						}
					}
					else
					{
						debug_parse_error(name, tokens);
						return;
					}
				}
				else if (token.type == StyleTokenType::percentage)
				{
					if (center_specified)
					{
						bg_pos_x = StyleValue::from_keyword("center");
						x_specified = true;
						center_specified = false;
					}

					if (!x_specified && !y_specified)
					{
						bg_pos_x = StyleValue::from_percentage(StringHelp::text_to_float(token.value));
						x_specified = true;
					}
					else if (x_specified && !y_specified)
					{
						bg_pos_y = StyleValue::from_percentage(StringHelp::text_to_float(token.value));
						y_specified = true;
					}
					else
					{
						debug_parse_error(name, tokens);
						return;
					}
				}
				else if (token.type == StyleTokenType::delim && token.value == "-")
				{
					token = next_token(pos, tokens);
					if (is_length(token))
					{
						StyleValue length;
						if (parse_length(token, length))
						{
							length.number = -length.number;
							if (center_specified)
							{
								bg_pos_x = StyleValue::from_keyword("center");
								x_specified = true;
								center_specified = false;
							}

							if (!x_specified && !y_specified)
							{
								bg_pos_x = length;
								x_specified = true;
							}
							else if (x_specified && !y_specified)
							{
								bg_pos_y = length;
								y_specified = true;
							}
							else
							{
								debug_parse_error(name, tokens);
								return;
							}
						}
						else
						{
							debug_parse_error(name, tokens);
							return;
						}
					}
					else if (token.type == StyleTokenType::percentage)
					{
						if (center_specified)
						{
							bg_pos_x = StyleValue::from_keyword("center");
							x_specified = true;
							center_specified = false;
						}

						if (!x_specified && !y_specified)
						{
							bg_pos_x = StyleValue::from_percentage(-StringHelp::text_to_float(token.value));
							x_specified = true;
						}
						else if (x_specified && !y_specified)
						{
							bg_pos_y = StyleValue::from_percentage(-StringHelp::text_to_float(token.value));
							y_specified = true;
						}
						else
						{
							debug_parse_error(name, tokens);
							return;
						}
					}
					else
					{
						debug_parse_error(name, tokens);
						return;
					}
				}
				else
				{
					debug_parse_error(name, tokens);
					return;
				}

				if (pos == tokens.size())
				{
					done = true;
					break;
				}
				else
				{
					token = next_token(pos, tokens);
					if (token.type == StyleTokenType::delim && token.value == ",")
						break;
				}
			}

			if (!x_specified)
				bg_pos_x = StyleValue::from_keyword("center");
			else if (!y_specified)
				bg_pos_y = StyleValue::from_keyword("center");

			positions_x.push_back(bg_pos_x);
			positions_y.push_back(bg_pos_y);
		}

		setter->set_value("background-position", position);
		setter->set_value_array("background-positions-x", positions_x);
		setter->set_value_array("background-positions-y", positions_y);
	}

	void BackgroundRepeatPropertyParser::parse(StylePropertySetter *setter, const std::string &name, const std::string &value, const std::initializer_list<StylePropertyInitializerValue> &args)
	{
		std::vector<StyleToken> tokens = StyleTokenizer::tokenize(value);

		StyleValue background_repeat;
		std::vector<StyleValue> background_repeats_x;
		std::vector<StyleValue> background_repeats_y;

		size_t pos = 0;
		StyleToken token = next_token(pos, tokens);

		if (token.type == StyleTokenType::ident && pos == tokens.size() && equals(token.value, "inherit"))
		{
			background_repeat = StyleValue::from_keyword("inherit");
			setter->set_value("background-repeat", background_repeat);
			setter->set_value_array("background-repeats-x", background_repeats_x);
			setter->set_value_array("background-repeats-y", background_repeats_y);
		}
		else
		{
			background_repeat = StyleValue::from_keyword("array");
			background_repeats_x.clear();
			background_repeats_y.clear();
			while (true)
			{
				if (token.type != StyleTokenType::ident)
					return;

				StyleValue repeat_x, repeat_y;
				bool single_style = false;

				if (equals(token.value, "repeat"))
				{
					repeat_x = StyleValue::from_keyword("repeat");
				}
				else if (equals(token.value, "repeat-x"))
				{
					repeat_x = StyleValue::from_keyword("repeat");
					repeat_y = StyleValue::from_keyword("no-repeat");
					single_style = true;
				}
				else if (equals(token.value, "repeat-y"))
				{
					repeat_x = StyleValue::from_keyword("no-repeat");
					repeat_y = StyleValue::from_keyword("repeat");
					single_style = true;
				}
				else if (equals(token.value, "no-repeat"))
				{
					repeat_x = StyleValue::from_keyword("no-repeat");
				}
				else if (equals(token.value, "space"))
				{
					repeat_x = StyleValue::from_keyword("space");
				}
				else if (equals(token.value, "round"))
				{
					repeat_x = StyleValue::from_keyword("round");
				}
				else
				{
					return;
				}

				if (pos == tokens.size())
				{
					if (!single_style)
						repeat_y = repeat_x;
					background_repeats_x.push_back(repeat_x);
					background_repeats_y.push_back(repeat_y);
					break;
				}

				token = next_token(pos, tokens);
				if (token.type == StyleTokenType::delim && token.value == ",")
				{
					if (!single_style)
						repeat_y = repeat_x;
					background_repeats_x.push_back(repeat_x);
					background_repeats_y.push_back(repeat_y);
				}
				else if (token.type == StyleTokenType::ident && !single_style)
				{
					if (equals(token.value, "repeat"))
					{
						repeat_y = StyleValue::from_keyword("repeat");
					}
					else if (equals(token.value, "no-repeat"))
					{
						repeat_y = StyleValue::from_keyword("no-repeat");
					}
					else if (equals(token.value, "space"))
					{
						repeat_y = StyleValue::from_keyword("space");
					}
					else if (equals(token.value, "round"))
					{
						repeat_y = StyleValue::from_keyword("round");
					}
					else
					{
						return;
					}

					background_repeats_x.push_back(repeat_x);
					background_repeats_y.push_back(repeat_y);

					if (pos == tokens.size())
						break;

					if (token.type != StyleTokenType::delim || token.value != ",")
						return;
				}
				else
				{
					return;
				}

				token = next_token(pos, tokens);
			}
		}

		setter->set_value("background-repeat", background_repeat);
		setter->set_value_array("background-repeats-x", background_repeats_x);
		setter->set_value_array("background-repeats-y", background_repeats_y);
	}

	void BackgroundSizePropertyParser::parse(StylePropertySetter *setter, const std::string &name, const std::string &value, const std::initializer_list<StylePropertyInitializerValue> &args)
	{
		std::vector<StyleToken> tokens = StyleTokenizer::tokenize(value);

		StyleValue background_size;
		std::vector<StyleValue> background_sizes_x;
		std::vector<StyleValue> background_sizes_y;

		size_t pos = 0;
		StyleToken token = next_token(pos, tokens);

		if (token.type == StyleTokenType::ident && pos == tokens.size() && equals(token.value, "inherit"))
		{
			background_size = StyleValue::from_keyword("inherit");
			setter->set_value("background-size", background_size);
			setter->set_value_array("background-sizes-x", background_sizes_x);
			setter->set_value_array("background-sizes-y", background_sizes_y);
		}
		else
		{
			background_size = StyleValue::from_keyword("array");
			while (true)
			{
				StyleValue size_x, size_y;

				bool single_value = false;
				if (token.type == StyleTokenType::ident)
				{
					if (equals(token.value, "contain"))
					{
						size_x = StyleValue::from_keyword("contain");
						size_y = StyleValue::from_keyword("contain");
						single_value = true;
					}
					else if (equals(token.value, "cover"))
					{
						size_x = StyleValue::from_keyword("cover");
						size_y = StyleValue::from_keyword("cover");
						single_value = true;
					}
					else if (equals(token.value, "auto"))
					{
						size_x = StyleValue::from_keyword("auto");
					}
					else
					{
						return;
					}
				}
				else if (is_length(token))
				{
					StyleValue length;
					if (parse_length(token, length))
					{
						size_x = length;
					}
					else
					{
						return;
					}
				}
				else if (token.type == StyleTokenType::percentage)
				{
					size_x = StyleValue::from_percentage(StringHelp::text_to_float(token.value));
				}
				else
				{
					return;
				}

				if (pos == tokens.size())
				{
					background_sizes_x.push_back(size_x);
					background_sizes_y.push_back(size_y);
					break;
				}

				token = next_token(pos, tokens);
				if (token.type != StyleTokenType::delim || token.value != ",")
				{
					if (single_value)
					{
						return;
					}
					else if (token.type == StyleTokenType::ident && equals(token.value, "auto"))
					{
						size_y = StyleValue::from_keyword("auto");
					}
					else if (is_length(token))
					{
						StyleValue length;
						if (parse_length(token, length))
						{
							size_y = length;
						}
						else
						{
							return;
						}
					}
					else if (token.type == StyleTokenType::percentage)
					{
						size_y = StyleValue::from_percentage(StringHelp::text_to_float(token.value));
					}
					else
					{
						return;
					}

					if (pos == tokens.size())
					{
						background_sizes_x.push_back(size_x);
						background_sizes_y.push_back(size_y);
						break;
					}

					token = next_token(pos, tokens);
				}

				background_sizes_x.push_back(size_x);
				background_sizes_y.push_back(size_y);

				if (token.type != StyleTokenType::delim || token.value != ",")
					return;

				token = next_token(pos, tokens);
			}
		}

		setter->set_value("background-size", background_size);
		setter->set_value_array("background-sizes-x", background_sizes_x);
		setter->set_value_array("background-sizes-y", background_sizes_y);
	}
}
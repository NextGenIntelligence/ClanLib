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
**    Harry Storbacka
**    Magnus Norddahl
**    James Wynn
**    Emanuel Greisen
**    Mark Page
**    (if your name is missing here, please add it)
*/

#include "Display/precomp.h"
#include "API/Core/IOData/virtual_file_system.h"
#include "API/Core/IOData/virtual_directory.h"
#include "API/Core/IOData/path_help.h"
#include "API/Display/Collision/collision_outline.h"
#include "outline_provider_bitmap.h"
#include "outline_provider_file.h"
#include "API/Display/Collision/outline_circle.h"
#include "API/Display/Window/display_window.h"
#include "API/Display/Render/graphic_context.h"
#include "API/Display/ImageProviders/provider_factory.h"
#include "API/Display/Image/pixel_buffer.h"
#include "API/Core/Resources/resource_manager.h"
#include "API/Core/Resources/resource_data_session.h"
#include "API/Core/System/exception.h"
#include "API/Core/IOData/path_help.h"
#include "resourcedata_collisionoutline.h"
#include "collision_outline_generic.h"
#include "API/Core/Text/string_format.h"
#include "API/Display/2D/canvas.h"

namespace clan
{

/////////////////////////////////////////////////////////////////////////////
// CollisionOutline Construction:

CollisionOutline::CollisionOutline()
 : impl(new CollisionOutline_Impl())
{
}

CollisionOutline::CollisionOutline(const std::vector<Contour> &contours, const Size &size, OutlineAccuracy accuracy)
 : impl(new CollisionOutline_Impl(contours, size, accuracy))
{
}

CollisionOutline::CollisionOutline(const std::string &fullname, int alpha_limit, OutlineAccuracy accuracy, bool get_insides)
{
	std::string path = PathHelp::get_fullpath(fullname, PathHelp::path_type_file);
	std::string filename = PathHelp::get_filename(fullname, PathHelp::path_type_file);
	VirtualFileSystem vfs(path);
	VirtualDirectory dir = vfs.get_root_directory();
	*this = CollisionOutline(filename, dir, alpha_limit, accuracy, get_insides);
}

CollisionOutline::CollisionOutline(const std::string &filename, const VirtualDirectory &directory, int alpha_limit, OutlineAccuracy accuracy, bool get_insides)
{
	std::string file_extension = PathHelp::get_extension(filename);

	IODevice file = directory.open_file_read(filename);
	*this = CollisionOutline(file, file_extension, alpha_limit, accuracy, get_insides);
}

CollisionOutline::CollisionOutline(
	IODevice &file, const std::string &file_extension,
	int alpha_limit,
	OutlineAccuracy accuracy,
	bool get_insides)
{
	if( file_extension == "out" )
	{
		OutlineProviderFile outline_provider(file);
		*this = CollisionOutline(outline_provider.get_contours(), outline_provider.get_size(), accuracy_raw );
	}
	else
	{

		PixelBuffer pbuf = ImageProviderFactory::load(file, file_extension);
	
		if( pbuf.get_format() == tf_rgba8 )
		{
			OutlineProviderBitmap outline_provider(pbuf, alpha_limit, get_insides);
			*this = CollisionOutline(outline_provider.get_contours(), outline_provider.get_size(), accuracy );
		}
		else
		{
			OutlineProviderBitmap outline_provider(pbuf, alpha_limit, get_insides);
			*this = CollisionOutline(outline_provider.get_contours(), outline_provider.get_size(), accuracy_raw );
		}
	}

	set_rotation_hotspot(origin_center);
}

CollisionOutline::CollisionOutline(
	const std::string &resource_id,
	ResourceManager *manager)
{
	resource = manager->get_resource(resource_id);
	if (resource.get_type() != "collisionoutline")
		throw Exception(string_format("Resource '%1' is not of type 'collisionoutline'", resource_id));
	ResourceDataSession("collisionoutline", resource);
	std::shared_ptr<ResourceData_CollisionOutline> data = std::dynamic_pointer_cast<ResourceData_CollisionOutline>(resource.get_data("collisionoutline"));
	if (!data)
	{
		data = std::shared_ptr<ResourceData_CollisionOutline>(new ResourceData_CollisionOutline(resource));
		resource.set_data("collisionoutline", data);
	}
	*this = data->collision_outline;
}

CollisionOutline::CollisionOutline(
	const PixelBuffer &pbuf,
	int alpha_limit,
	OutlineAccuracy accuracy)
 : impl(new CollisionOutline_Impl())
{
	if( pbuf.get_format() == tf_rgba8 )
	{
		OutlineProviderBitmap outline_provider(pbuf, alpha_limit);
		*this = CollisionOutline(outline_provider.get_contours(), outline_provider.get_size(), accuracy );
	}
	else
	{
		OutlineProviderBitmap outline_provider(pbuf, alpha_limit);
		*this = CollisionOutline(outline_provider.get_contours(), outline_provider.get_size(), accuracy_raw );
	}
	
	set_rotation_hotspot(origin_center);
}

CollisionOutline::~CollisionOutline()
{
}

/////////////////////////////////////////////////////////////////////////////
// CollisionOutline Attributes:

std::vector<Contour> &CollisionOutline::get_contours()
{
	return impl->contours;
}

const std::vector<Contour> &CollisionOutline::get_contours() const
{
	return impl->contours;
}

Circlef CollisionOutline::get_minimum_enclosing_disc() const
{
	return impl->minimum_enclosing_disc;
}

Pointf CollisionOutline::get_translation() const
{
	return impl->position;
}

float CollisionOutline::get_angle() const
{
	return impl->angle;
}

Pointf CollisionOutline::get_scale() const
{
	return impl->scale_factor;
}

bool CollisionOutline::get_inside_test() const
{
	return impl->do_inside_test;
}

void CollisionOutline::get_alignment( Origin &origin, float &x, float &y ) const
{
	origin = impl->translation_origin;
	x = impl->translation_offset.x;
	y = impl->translation_offset.y;
}

void CollisionOutline::get_rotation_hotspot( Origin &origin, float &x, float &y) const
{
	origin = impl->rotation_origin;
	x = impl->rotation_hotspot.x;
	y = impl->rotation_hotspot.y;
}

unsigned int CollisionOutline::get_width() const
{
	return impl->width;
}

unsigned int CollisionOutline::get_height() const
{
	return impl->height;
}

const std::vector<CollidingContours> &CollisionOutline::get_collision_info() const
{
	return impl->collision_info;
}

void CollisionOutline::set_collision_info(const std::vector<CollidingContours> &colinfo)
{
	impl->collision_info = colinfo;
}

void CollisionOutline::clean_collision_info()
{
	impl->collision_info.clear();
}

void CollisionOutline::get_collision_info_state(bool &points, bool &normals, bool &metadata, bool &pendepth) const
{
	points = impl->collision_info_points;
	normals = impl->collision_info_normals;
	metadata = impl->collision_info_meta;
	pendepth = impl->collision_info_pen_depth;
}

/////////////////////////////////////////////////////////////////////////////
// CollisionOutline Operations:

CollisionOutline CollisionOutline::clone() const
{
	CollisionOutline copy;
	copy.impl->contours.clear();
	copy.impl->contours.reserve(impl->contours.size());
	for (size_t i = 0; i < impl->contours.size(); i++)
		copy.impl->contours.push_back(impl->contours[i].clone());
	copy.impl->do_inside_test = get_inside_test();
	copy.impl->width = get_width();
	copy.impl->height = get_height();
	copy.impl->position = get_translation();
	copy.impl->scale_factor = get_scale();
	copy.impl->angle = get_angle();
	copy.impl->minimum_enclosing_disc = get_minimum_enclosing_disc();

	bool points, normals, metadata, pendepths;
	get_collision_info_state(points,normals,metadata,pendepths);
	copy.enable_collision_info(points,normals,metadata,pendepths);

	Origin origin;
	float x, y;

	get_alignment(origin,x,y);
	copy.impl->translation_origin = origin;
	copy.impl->translation_offset.x = x;
	copy.impl->translation_offset.y = y;

	get_rotation_hotspot(origin,x,y);
	copy.impl->rotation_origin = origin;
	copy.impl->rotation_hotspot.x = x;
	copy.impl->rotation_hotspot.y = y;
	return copy;
}

void CollisionOutline::optimize(unsigned char check_distance, float corner_angle )
{
	impl->optimize(check_distance, corner_angle);
}

void CollisionOutline::draw(
	float x,
	float y,
	const Colorf &color,
	Canvas &canvas)
{
	GraphicContext &gc = canvas.get_gc();
	gc.set_point_size(2.0f);

	// Draw collision outline (Contours are assumed as closed polygons, hence we use line-loop)
	for(unsigned int i = 0; i < impl->contours.size(); i++)
	{
		// Draw the contour
		unsigned int numpoints = impl->contours[i].get_points().size();
		for(unsigned int s = 0; s < numpoints; s++)
		{
			const Pointf &p1 = impl->contours[i].get_points()[s];
			const Pointf &p2 = impl->contours[i].get_points()[(s+1) % numpoints];
			canvas.draw_line(x + p1.x + 0.5f, y + p1.y + 0.5f, x + p2.x + 0.5f, y + p2.y + 0.5f, color);
		}

	}

	Colorf colorinv(1.0f-color.get_red(),1.0f-color.get_green(),1.0f-color.get_blue());

	for(unsigned int i = 0; i < impl->contours.size(); i++)
	{
		unsigned int numpoints = impl->contours[i].get_points().size();

		// Add points (as opposite color)
		for(unsigned int s1 = 0; s1 < numpoints; s1++)
		{
			const Pointf &p1 = impl->contours[i].get_points()[s1];
			canvas.draw_point(x + p1.x + 0.5f, y + p1.y + 0.5f, colorinv);
		}
	}
}

void CollisionOutline::draw_sub_circles(
	float x,
	float y,
	const Colorf &color,
	Canvas &canvas)
{
	// Draw the circles
	for(unsigned int i = 0; i < impl->contours.size(); i++)
	{
		unsigned int numcircles = impl->contours[i].get_sub_circles().size();
		for(unsigned int s = 0; s < numcircles; s++)
		{
			Pointf center = impl->contours[i].get_sub_circles()[s].position;
			float radius     = impl->contours[i].get_sub_circles()[s].radius;
			float numsegments = 16;
			for(float e = 0; e < numsegments; e++)
			{
				float offx1 = float(cos(PI * 2.0 * (e / numsegments))*radius);
				float offy1 = float(sin(PI * 2.0 * (e / numsegments))*radius);
				float offx2 = float(cos(PI * 2.0 * ((e+1) / numsegments))*radius);
				float offy2 = float(sin(PI * 2.0 * ((e+1) / numsegments))*radius);
				Pointf p1(x + center.x + offx1, y + center.y + offy1);
				Pointf p2(x + center.x + offx2, y + center.y + offy2);
				canvas.draw_line(p1.x + 0.5f, p1.y + 0.5f, p2.x + 0.5f, p2.y + 0.5f, color);
			}
		}
	}
}

void CollisionOutline::draw_smallest_enclosing_disc(
	float x,
	float y,
	const Colorf &color,
	Canvas &canvas)
{
	// Draw the smallest enclosing disc
	Pointf center = impl->minimum_enclosing_disc.position;
	float radius     = impl->minimum_enclosing_disc.radius;
	float numsegments = 24; // To make it visible if the outline has only one contour, and the contour only has one sub-circle
	for(float e = 0; e < numsegments; e++)
	{
		float offx1 = float(cos(PI * 2.0 * (e / numsegments))*radius);
		float offy1 = float(sin(PI * 2.0 * (e / numsegments))*radius);
		float offx2 = float(cos(PI * 2.0 * ((e+1) / numsegments))*radius);
		float offy2 = float(sin(PI * 2.0 * ((e+1) / numsegments))*radius);
		Pointf p1(x + center.x + offx1, y + center.y + offy1);
		Pointf p2(x + center.x + offx2, y + center.y + offy2);
		canvas.draw_line(p1.x + 0.5f, p1.y + 0.5f, p2.x + 0.5f, p2.y + 0.5f, color);
	}
}

void CollisionOutline::set_translation(float x, float y)
{
	impl->set_translation(x,y);
}

void CollisionOutline::set_scale(float x, float y)
{
	impl->set_scale(x,y);
}

void CollisionOutline::set_angle(const Angle &angle)
{
	impl->set_angle(angle);
}

void CollisionOutline::rotate(const Angle &angle)
{
	impl->rotate(angle);
}

void CollisionOutline::set_inside_test(bool value)
{
	impl->do_inside_test = value;
}

void CollisionOutline::enable_collision_info(  bool points, bool normals, bool metadata, bool pen_depth)
{
	impl->collision_info_points = points;
	impl->collision_info_normals = normals;
	impl->collision_info_meta = metadata;
	impl->collision_info_pen_depth = pen_depth;
	impl->collision_info_collect = points || normals || metadata || pen_depth;
}

void CollisionOutline::calculate_radius()
{
	impl->calculate_radius();
}

void CollisionOutline::calculate_sub_circles(float radius_multiplier)
{
	impl->calculate_sub_circles(radius_multiplier);
}

void CollisionOutline::calculate_smallest_enclosing_discs()
{
	impl->calculate_smallest_enclosing_discs();
}

void CollisionOutline::calculate_convex_hulls()
{
	impl->calculate_convex_hulls();
}

void CollisionOutline::save(const std::string &fullname) const
{

	std::string path = PathHelp::get_fullpath(fullname, PathHelp::path_type_file);
	std::string filename = PathHelp::get_filename(fullname, PathHelp::path_type_file);
	VirtualFileSystem vfs(path);
	VirtualDirectory dir = vfs.get_root_directory();
	save(filename, dir);
}

void CollisionOutline::save(const std::string &filename, VirtualDirectory &directory) const
{
	IODevice file = directory.open_file(filename, File::create_always);
	impl->save(file);
}

void CollisionOutline::save(IODevice &file) const
{
	impl->save(file);
}

bool CollisionOutline::collide(const CollisionOutline &outline, bool remove_old_collision_info)
{
	return impl->collide(outline, remove_old_collision_info);
}

void CollisionOutline::calculate_penetration_depth(std::vector<CollidingContours> &collision_info)
{
	CollisionOutline_Impl::calculate_penetration_depth(collision_info);
}

bool CollisionOutline::point_inside(const Pointf &point) const
{
	return impl->point_inside(point);
}

void CollisionOutline::set_alignment( Origin origin, float x, float y )
{
	Origin old_origin = impl->translation_origin;

	// undo previous translation offset.
	impl->set_translation(-impl->translation_offset.x, -impl->translation_offset.y, true);
	
	// undo previous origin.
	if( origin != impl->translation_origin )
	{
		float fix_x = 0, fix_y = 0;

		float width = impl->width/2.0f;
		float height = impl->height/2.0f;

		switch( old_origin )
		{
		 case origin_top_left:
			fix_x = -width;
			fix_y = -height;
			break;
		 case origin_top_center:
			fix_x = 0;
			fix_y = -height;
			break;
		 case origin_top_right:
			fix_x = width;
			fix_y = -height;
			break;

		 case origin_center_left:
			fix_x = -width;
			fix_y = 0;
			break;
		 case origin_center:
			fix_x = 0;
			fix_y = 0;
			break;
		 case origin_center_right:
			fix_x = width;
			fix_y = 0;
			break;

		 case origin_bottom_left:
			fix_x = -width;
			fix_y = height;
			break;
		 case origin_bottom_center:
			fix_x = 0;
			fix_y = height;
			break;
		 case origin_bottom_right:
			fix_x = width;
			fix_y = height;
			break;
		}

		impl->set_translation(fix_x, fix_y, true); // offset outline points

		impl->rotation_hotspot.x += fix_x;
		impl->rotation_hotspot.y += fix_y;
	}

	
	impl->translation_origin = origin;
	impl->translation_offset.x = x;
	impl->translation_offset.y = y;
	
	impl->set_translation(x, y, true); // offset outline points

	// set new origin offset.
	if( origin != origin_center )
	{
		float fix_x = 0, fix_y = 0;

		float width = impl->width/2.0f;
		float height = impl->height/2.0f;

		switch( origin )
		{
		 case origin_top_left:
			fix_x = width;
			fix_y = height;
			break;
		 case origin_top_center:
			fix_x = 0;
			fix_y = height;
			break;
		 case origin_top_right:
			fix_x = -width;
			fix_y = height;
			break;

		 case origin_center_left:
			fix_x = width;
			fix_y = 0;
			break;
		 case origin_center:
			fix_x = 0;
			fix_y = 0;
			break;
		 case origin_center_right:
			fix_x = -width;
			fix_y = 0;
			break;

		 case origin_bottom_left:
			fix_x = width;
			fix_y = -height;
			break;
		 case origin_bottom_center:
			fix_x = 0;
			fix_y = -height;
			break;
		 case origin_bottom_right:
			fix_x = -width;
			fix_y = -height;
			break;
		}
		
		impl->set_translation(fix_x, fix_y, true); // offset outline points

		impl->rotation_hotspot.x += fix_x;
		impl->rotation_hotspot.y += fix_y;
	}

	impl->calculate_radius();
}

void CollisionOutline::set_rotation_hotspot( Origin origin, float x, float y )
{
	// undo translation origin
	Origin trans_origin = impl->translation_origin;	
	set_alignment(origin_center);

	// undo the rotation of the current rotation hotspot
	float angle = impl->angle;
	impl->set_angle(Angle(0.0f,angle_degrees));

	float fix_x = 0, fix_y = 0;

	// set new origin offset.
	if( origin != origin_center )
	{
		float width = impl->width/2.0f;
		float height = impl->height/2.0f;

		switch( origin )
		{
		 case origin_top_left:
			fix_x = -width;
			fix_y = -height;
			break;
		 case origin_top_center:
			fix_x = 0;
			fix_y = -height;
			break;
		 case origin_top_right:
			fix_x = width;
			fix_y = -height;
			break;

		 case origin_center_left:
			fix_x = -width;
			fix_y = 0;
			break;
		 case origin_center:
			fix_x = 0;
			fix_y = 0;
			break;
		 case origin_center_right:
			fix_x = width;
			fix_y = 0;
			break;

		 case origin_bottom_left:
			fix_x = -width;
			fix_y = height;
			break;
		 case origin_bottom_center:
			fix_x = 0;
			fix_y = height;
			break;
		 case origin_bottom_right:
			fix_x = width;
			fix_y = height;
			break;
		}
	}

	impl->rotation_origin = origin;

	impl->rotation_hotspot.x = fix_x + x;
	impl->rotation_hotspot.y = fix_y + y;

	// transform data using new rotation hotspot
	impl->set_angle(Angle(angle,angle_degrees));

	set_alignment(trans_origin);

	impl->calculate_radius();
}

}

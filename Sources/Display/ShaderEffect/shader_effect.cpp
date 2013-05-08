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
**    Magnus Norddahl
**    Kenneth Gangstoe
*/

#include "Display/precomp.h"
#include "API/Display/ShaderEffect/shader_effect.h"
#include "API/Display/ShaderEffect/shader_effect_description.h"
#include "API/Display/Render/program_object.h"
#include "API/Display/Render/frame_buffer.h"
#include "API/Display/Render/rasterizer_state.h"
#include "API/Display/Render/blend_state.h"
#include "API/Display/Render/depth_stencil_state.h"
#include "API/Display/Render/vertex_array_buffer.h"
#include "API/Display/Render/primitives_array.h"
#include "API/Display/Render/element_array_buffer.h"
#include "API/Display/Render/uniform_buffer.h"
#include "API/Display/Render/storage_buffer.h"
#include "API/Display/Render/texture.h"
#include "API/Display/Render/shader_object.h"
#include "API/Core/Text/string_format.h"
#include "shader_effect_description_impl.h"
#include <map>

namespace clan
{

class ShaderEffect_Impl
{
public:
	ShaderEffect_Impl(GraphicContext &gc) : elements_type(), num_vertices(0), program(gc) { }

	static std::string add_defines(GraphicContext &gc, const std::string * const code, const ShaderEffectDescription_Impl *description);

	void create_shaders(GraphicContext &gc, const ShaderEffectDescription_Impl *description);
	void create_primitives_array(GraphicContext &gc, const ShaderEffectDescription_Impl *description);

	ProgramObject program;

	FrameBuffer fb;

	RasterizerState rasterizer_state;
	BlendState blend_state;
	DepthStencilState depth_stencil_state;

	std::vector<VertexArrayBuffer> attributes;
	PrimitivesArray prim_array;

	ElementArrayBuffer elements;
	VertexAttributeDataType elements_type;

	std::map<int, UniformBuffer> uniform_bindings;

	std::map<int, StorageBuffer> storage_bindings;
	std::map<int, Texture> image_bindings;

	std::map<int, Texture> texture_bindings;

	int num_vertices;
};

ShaderEffect::ShaderEffect()
{
}

ShaderEffect::ShaderEffect(GraphicContext &gc, const ShaderEffectDescription &description)
: impl(new ShaderEffect_Impl(gc))
{
	impl->create_shaders(gc, description.impl.get());
	impl->create_primitives_array(gc, description.impl.get());
	impl->elements = description.impl->elements;
	impl->elements_type = description.impl->elements_type;
	impl->rasterizer_state = RasterizerState(gc, description.impl->rasterizer_state);
	impl->blend_state = BlendState(gc, description.impl->blend_state);
	impl->depth_stencil_state = DepthStencilState(gc, description.impl->depth_stencil_state);
}

bool ShaderEffect::is_null() const
{
	return !impl;
}

void ShaderEffect::dispatch(GraphicContext &gc, int x, int y, int z)
{
	gc.set_program_object(impl->program);

	for (auto it = impl->uniform_bindings.begin(); it != impl->uniform_bindings.end(); ++it)
	{
		gc.set_uniform_buffer(it->first, it->second);
	}

	for (auto it = impl->storage_bindings.begin(); it != impl->storage_bindings.end(); ++it)
	{
		gc.set_storage_buffer(it->first, it->second);
	}

	for (auto it = impl->image_bindings.begin(); it != impl->image_bindings.end(); ++it)
	{
		gc.set_image_texture(it->first, it->second);
	}

	for (auto it = impl->texture_bindings.begin(); it != impl->texture_bindings.end(); ++it)
	{
		gc.set_texture(it->first, it->second);
	}

	gc.dispatch(x, y, z);

	for (auto it = impl->uniform_bindings.begin(); it != impl->uniform_bindings.end(); ++it)
	{
		gc.reset_uniform_buffer(it->first);
	}

	for (auto it = impl->storage_bindings.begin(); it != impl->storage_bindings.end(); ++it)
	{
		gc.reset_storage_buffer(it->first);
	}

	for (auto it = impl->image_bindings.begin(); it != impl->image_bindings.end(); ++it)
	{
		gc.reset_image_texture(it->first);
	}

	for (auto it = impl->texture_bindings.begin(); it != impl->texture_bindings.end(); ++it)
	{
		gc.reset_texture(it->first);
	}

	gc.reset_program_object();
}

void ShaderEffect::draw(GraphicContext &gc)
{
	gc.set_program_object(impl->program);

	if (!impl->fb.is_null())
		gc.set_frame_buffer(impl->fb);

	if (!impl->rasterizer_state.is_null())
		gc.set_rasterizer_state(impl->rasterizer_state);

	if (!impl->blend_state.is_null())
		gc.set_blend_state(impl->blend_state);

	if (!impl->depth_stencil_state.is_null())
		gc.set_depth_stencil_state(impl->depth_stencil_state);

	for (auto it = impl->uniform_bindings.begin(); it != impl->uniform_bindings.end(); ++it)
	{
		gc.set_uniform_buffer(it->first, it->second);
	}

	for (auto it = impl->storage_bindings.begin(); it != impl->storage_bindings.end(); ++it)
	{
		gc.set_storage_buffer(it->first, it->second);
	}

	for (auto it = impl->image_bindings.begin(); it != impl->image_bindings.end(); ++it)
	{
		gc.set_image_texture(it->first, it->second);
	}

	for (auto it = impl->texture_bindings.begin(); it != impl->texture_bindings.end(); ++it)
	{
		gc.set_texture(it->first, it->second);
	}

	if (!impl->elements.is_null())
	{
		gc.set_primitives_elements(impl->elements);
		gc.set_primitives_array(impl->prim_array);
		gc.draw_primitives_elements(type_triangles, impl->num_vertices, impl->elements_type);
		gc.reset_primitives_array();
		gc.reset_primitives_elements();
	}
	else
	{
		gc.set_primitives_array(impl->prim_array);
		gc.draw_primitives_array(type_triangles, 0, impl->num_vertices);
		gc.reset_primitives_array();
	}

	for (auto it = impl->uniform_bindings.begin(); it != impl->uniform_bindings.end(); ++it)
	{
		gc.reset_uniform_buffer(it->first);
	}

	for (auto it = impl->storage_bindings.begin(); it != impl->storage_bindings.end(); ++it)
	{
		gc.reset_storage_buffer(it->first);
	}

	for (auto it = impl->image_bindings.begin(); it != impl->image_bindings.end(); ++it)
	{
		gc.reset_image_texture(it->first);
	}

	for (auto it = impl->texture_bindings.begin(); it != impl->texture_bindings.end(); ++it)
	{
		gc.reset_texture(it->first);
	}

	if (!impl->rasterizer_state.is_null())
		gc.reset_rasterizer_state();

	if (!impl->blend_state.is_null())
		gc.reset_blend_state();

	if (!impl->depth_stencil_state.is_null())
		gc.reset_depth_stencil_state();

	if (!impl->fb.is_null())
		gc.reset_frame_buffer();

	gc.reset_program_object();
}

/////////////////////////////////////////////////////////////////////////////

std::string ShaderEffect_Impl::add_defines(GraphicContext &gc, const std::string * const code, const ShaderEffectDescription_Impl *description)
{
	if (code[gc.get_shader_language()].empty())
		return std::string();

	std::string prefix;
	if (gc.get_shader_language() == shader_glsl && description->glsl_version != 0)
		prefix += string_format("#version %1\r\n", description->glsl_version);

	for (auto it = description->defines.begin(); it != description->defines.end(); ++it)
		prefix += string_format("#define %1 %2\r\n", it->first, it->second);
	prefix += "#line 0\r\n";

	return prefix + code[gc.get_shader_language()];
}

void ShaderEffect_Impl::create_shaders(GraphicContext &gc, const ShaderEffectDescription_Impl *description)
{
	std::string vertex_shader_code = add_defines(gc, description->vertex_shader_code, description);
	std::string fragment_shader_code = add_defines(gc, description->fragment_shader_code, description);
	std::string compute_shader_code = add_defines(gc, description->compute_shader_code, description);

	if (!vertex_shader_code.empty()) 
	{
		ShaderObject vertex_shader(gc, shadertype_vertex, vertex_shader_code);
		if(!vertex_shader.compile())
			throw Exception(string_format("Unable to compile vertex shader: %1", vertex_shader.get_info_log()));
		program.attach(vertex_shader);
	}

	if (!fragment_shader_code.empty()) 
	{
		ShaderObject fragment_shader(gc, shadertype_fragment, fragment_shader_code);
		if(!fragment_shader.compile())
			throw Exception(string_format("Unable to compile fragment shader: %1", fragment_shader.get_info_log()));
		program.attach(fragment_shader);
	}

	if (!compute_shader_code.empty()) 
	{
		ShaderObject compute_shader(gc, shadertype_compute, compute_shader_code);
		if(!compute_shader.compile())
			throw Exception(string_format("Unable to compile compute shader: %1", compute_shader.get_info_log()));
		program.attach(compute_shader);
	}

	int index = 0;
	for(auto it = description->attributes.begin(); it != description->attributes.end(); ++it)
	{
		program.bind_attribute_location(index++, it->first);
	}

	index = 0;
	for(auto it = description->frag_data.begin(); it != description->frag_data.end(); ++it)
	{
		program.bind_frag_data_location(index++, it->first);
	}

	index = 0;
	for(auto it = description->uniform_buffers.begin(); it != description->uniform_buffers.end(); ++it, index++)
	{
		program.set_uniform_buffer_index(it->first, index);
		uniform_bindings[index] = it->second;
	}

	index = 0;
	for(auto it = description->textures.begin(); it != description->textures.end(); ++it, index++)
	{
		program.set_uniform1i(it->first, index);
		texture_bindings[index] = it->second;
	}

	index = 0;
	for(auto it = description->images.begin(); it != description->images.end(); ++it, index++)
	{
		program.set_uniform1i(it->first, index);
		image_bindings[index] = it->second;
	}

	index = 0;
	for(auto it = description->storage_buffers.begin(); it != description->storage_buffers.end(); ++it, index++)
	{
		program.set_uniform1i(it->first, index);
		storage_bindings[index] = it->second;
	}

	if (!program.link())
		throw Exception(string_format("Link failed: %1", program.get_info_log()));
}

void ShaderEffect_Impl::create_primitives_array(GraphicContext &gc, const ShaderEffectDescription_Impl *description)
{
	prim_array = PrimitivesArray(gc);
	
	int index = 0;
	for (auto it = description->attributes.begin(); it != description->attributes.end(); ++it, index++)
	{
		if (it->second.attribute_type == ShaderEffectDescription_Impl::attribute_type_buffer)
		{
			VertexArrayBuffer buffer = it->second.buffer;
			prim_array.set_attributes(index, buffer, it->second.size, it->second.type, it->second.offset, it->second.stride, it->second.normalize);
			attributes.push_back(buffer);

			num_vertices = description->draw_count;
		}
		else if (it->second.attribute_type == ShaderEffectDescription_Impl::attribute_type_screen_quad)
		{
			Vec4f screen_quad[6] =
			{
				Vec4f(-1.0f,-1.0f, 0.0f, 1.0f),
				Vec4f( 1.0f,-1.0f, 0.0f, 1.0f),
				Vec4f(-1.0f, 1.0f, 0.0f, 1.0f),
				Vec4f( 1.0f, 1.0f, 0.0f, 1.0f),
				Vec4f( 1.0f,-1.0f, 0.0f, 1.0f),
				Vec4f(-1.0f, 1.0f, 0.0f, 1.0f)
			};
			VertexArrayVector<Vec4f> gpu_screen_quad(gc, screen_quad, 6);
			prim_array.set_attributes(index, gpu_screen_quad);
			attributes.push_back(gpu_screen_quad);

			num_vertices = 6;
		}
		else if (it->second.attribute_type == ShaderEffectDescription_Impl::attribute_type_uv_quad)
		{
			Vec4f uv_quad[6] =
			{
				Vec4f( 0.0f, 0.0f, 0.0f, 1.0f),
				Vec4f( 1.0f, 0.0f, 0.0f, 1.0f),
				Vec4f( 0.0f, 1.0f, 0.0f, 1.0f),
				Vec4f( 1.0f, 1.0f, 0.0f, 1.0f),
				Vec4f( 1.0f, 0.0f, 0.0f, 1.0f),
				Vec4f( 0.0f, 1.0f, 0.0f, 1.0f)
			};
			VertexArrayVector<Vec4f> gpu_uv_quad(gc, uv_quad, 6);
			prim_array.set_attributes(index, gpu_uv_quad);
			attributes.push_back(gpu_uv_quad);

			num_vertices = 6;
		}
	}
}

}

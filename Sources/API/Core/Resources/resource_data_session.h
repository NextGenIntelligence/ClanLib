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
*/

/// \addtogroup clanCore_Resources clanCore Resources
/// \{

#pragma once

#include "../api_core.h"
#include "resource.h"

namespace clan
{

/// \brief Resource data usage tracker / garbage collector.
class CL_API_CORE ResourceDataSession
{
/// \name Construction
/// \{

public:
	/// \brief Constructs a resource data session.
	ResourceDataSession();

	/// \brief Constructs a ResourceDataSession
	///
	/// \param data_name = String
	/// \param resource = Resource
	ResourceDataSession(const std::string &data_name, const Resource &resource);

	/// \brief Constructs a ResourceDataSession
	///
	/// \param copy = Resource Data Session
	ResourceDataSession(const ResourceDataSession &copy);

	~ResourceDataSession();

/// \}
/// \name Attributes
/// \{

public:

/// \}
/// \name Operations
/// \{

public:
	ResourceDataSession &operator =(const ResourceDataSession &copy);

/// \}
/// \name Implementation
/// \{

private:
	std::string name;

	Resource resource;
/// \}
};

}

/// \}

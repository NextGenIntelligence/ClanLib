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

/// \addtogroup clanCore_Signals clanCore Signals
/// \{

#pragma once

#include "../System/exception.h"
#include <memory>

namespace clan
{

/// (Internal ClanLib Class)
template <typename P1, typename P2, typename P3, typename P4, typename P5>
class Callback_Impl_v5
{
public:
	virtual ~Callback_Impl_v5()
	{
	}

	virtual void invoke(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const = 0;
};

/// (Internal ClanLib Class)
template <typename P1, typename P2, typename P3, typename P4, typename P5>
class Callback_Impl_v5_static : public Callback_Impl_v5<P1, P2, P3, P4, P5>
{
public:
	Callback_Impl_v5_static(void (*static_func)(P1, P2, P3, P4, P5))
	: static_func(static_func)
	{
	}

	void invoke(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
	{
		static_func(p1, p2, p3, p4, p5);
	}

	void (*static_func)(P1, P2, P3, P4, P5);
};

/// (Internal ClanLib Class)
template <typename P1, typename P2, typename P3, typename P4, typename P5, typename UserData>
class Callback_Impl_v5_static_user : public Callback_Impl_v5<P1, P2, P3, P4, P5>
{
public:
	Callback_Impl_v5_static_user(
		void (*static_func)(P1, P2, P3, P4, P5, UserData), const UserData &user_data)
	: static_func(static_func), user_data(user_data)
	{
	}

	void invoke(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
	{
		static_func(p1, p2, p3, p4, p5, user_data);
	}

	void (*static_func)(P1, P2, P3, P4, P5, UserData);

	UserData user_data;
};

/// (Internal ClanLib Class)
template <typename P1, typename P2, typename P3, typename P4, typename P5, typename InstanceClass>
class Callback_Impl_v5_member : public Callback_Impl_v5<P1, P2, P3, P4, P5>
{
public:
	Callback_Impl_v5_member(InstanceClass *instance,
		void (InstanceClass::*member_func)(P1, P2, P3, P4, P5))
	: instance(instance), member_func(member_func)
	{
	}

	void invoke(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
	{
		(instance->*member_func)(p1, p2, p3, p4, p5);
	}

	InstanceClass *instance;

	void (InstanceClass::*member_func)(P1, P2, P3, P4, P5);
};

/// (Internal ClanLib Class)
template <typename P1, typename P2, typename P3, typename P4, typename P5, typename InstanceClass, typename UserData>
class Callback_Impl_v5_member_user : public Callback_Impl_v5<P1, P2, P3, P4, P5>
{
public:
	Callback_Impl_v5_member_user(InstanceClass *instance,
		void (InstanceClass::*member_func)(P1, P2, P3, P4, P5, UserData), const UserData &user_data)
	: instance(instance), member_func(member_func), user_data(user_data)
	{
	}

	void invoke(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
	{
		(instance->*member_func)(p1, p2, p3, p4, p5, user_data);
	}

	InstanceClass *instance;

	void (InstanceClass::*member_func)(P1, P2, P3, P4, P5, UserData);

	UserData user_data;
};

/// (Internal ClanLib Class)
template <class P1, class P2, class P3, class P4, class P5, class Functor>
class Callback_Impl_v5_functor : public Callback_Impl_v5<P1, P2, P3, P4, P5>
{
public:
	Callback_Impl_v5_functor(Functor functor)
	: functor(functor)
	{
	}

	void invoke(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
	{
		functor(p1, p2, p3, p4, p5);
	}

	Functor functor;
};

/// \brief Callback_v5
///
template <typename P1, typename P2, typename P3, typename P4, typename P5>
class Callback_v5
{
public:
	Callback_v5()
	{
	}

	Callback_v5(const Callback_v5 &copy)
	: impl(copy.impl)
	{
	}

	Callback_v5(Callback_Impl_v5<P1, P2, P3, P4, P5> *impl)
	: impl(impl)
	{
	}

	Callback_v5(void (*function)(P1, P2, P3, P4, P5))
	: impl(new Callback_Impl_v5_static<P1, P2, P3, P4, P5>(function))
	{
	}

	template<typename UserData>
	Callback_v5(void (*function)(P1, P2, P3, P4, P5, UserData), const UserData &user_data)
	: impl(new Callback_Impl_v5_static_user<P1, P2, P3, P4, P5, UserData>(function, user_data))
	{
	}

	template<class InstanceClass>
	Callback_v5(InstanceClass *instance, void (InstanceClass::*function)(P1, P2, P3, P4, P5))
	: impl(new Callback_Impl_v5_member<P1, P2, P3, P4, P5, InstanceClass>(instance, function))
	{
	}

	template<class InstanceClass, typename UserData>
	Callback_v5(InstanceClass *instance, void (InstanceClass::*function)(P1, P2, P3, P4, P5, UserData), const UserData &user_data)
	: impl(new Callback_Impl_v5_member_user<P1, P2, P3, P4, P5, InstanceClass, UserData>(instance, function, user_data))
	{
	}

	void set(void (*function)(P1, P2, P3, P4, P5))
	{
		impl = std::shared_ptr< Callback_Impl_v5<P1, P2, P3, P4, P5> >(new Callback_Impl_v5_static<P1, P2, P3, P4, P5>(function));
	}

	template<typename UserData>
	void set(void (*function)(P1, P2, P3, P4, P5, UserData), const UserData &user_data)
	{
		impl = std::shared_ptr< Callback_Impl_v5<P1, P2, P3, P4, P5> >(new Callback_Impl_v5_static_user<P1, P2, P3, P4, P5, UserData>(function, user_data));
	}

	template<class InstanceClass>
	void set(InstanceClass *instance, void (InstanceClass::*function)(P1, P2, P3, P4, P5))
	{
		impl = std::shared_ptr< Callback_Impl_v5<P1, P2, P3, P4, P5> >(new Callback_Impl_v5_member<P1, P2, P3, P4, P5, InstanceClass>(instance, function));
	}

	template<class InstanceClass, typename UserData>
	void set(InstanceClass *instance, void (InstanceClass::*function)(P1, P2, P3, P4, P5, UserData), const UserData &user_data)
	{
		impl = std::shared_ptr< Callback_Impl_v5<P1, P2, P3, P4, P5> >(new Callback_Impl_v5_member_user<P1, P2, P3, P4, P5, InstanceClass, UserData>(instance, function, user_data));
	}

	void clear()
	{
		impl = std::shared_ptr< Callback_Impl_v5<P1, P2, P3, P4, P5> >();
	}

	void invoke(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
	{
		impl->invoke(p1, p2, p3, p4, p5);
	}

	bool is_null() const
	{
		return !impl;
	}

private:
	std::shared_ptr< Callback_Impl_v5<P1, P2, P3, P4, P5> > impl;
};

/// \brief Callback_v5_functor
///
template <typename P1, typename P2, typename P3, typename P4, typename P5>
class Callback_v5_functor : public Callback_v5<P1, P2, P3, P4, P5>
{
public:
	Callback_v5_functor()
	{
	}

	Callback_v5_functor(const Callback_v5_functor &copy)
	: Callback_v5<P1, P2, P3, P4, P5>(copy)
	{
	}

	template<class Functor>
	Callback_v5_functor(Functor functor)
	: Callback_v5<P1, P2, P3, P4, P5>(new Callback_Impl_v5_functor<P1, P2, P3, P4, P5, Functor>(functor))
	{
	}
};

}

/// \}

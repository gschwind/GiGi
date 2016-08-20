// -*- C++ -*-
/* GG is a GUI for SDL and OpenGL.
   Copyright (C) 2003-2008 T. Zachary Laine
   Copyright (C) 2016 Benoit Gschwind

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1
   of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA

   If you do not wish to comply with the terms of the LGPL please
   contact the author as other terms are available for a fee.

   Zach Laine
   whatwasthataddress@gmail.com */

/* Note that this file intentionally has no include guards.  This is because it is intended to be included multiple
   times, as in SignalsAndSlots.h. */

namespace GG {

namespace detail {
template <class C, class R ,
          typename ... Args>
struct Forwarder_
{
	Forwarder_(boost::signals2::signal<R (Args...), C>& sig_) : sig(sig_) {}
    R operator()(Args ... args) {sig(args...);}
    boost::signals2::signal<R (Args...), C>& sig;
};
} // namespace detail

/** connects a signal to a member function of a specific object that has the same function signature, putting \a R in
    slot group 0.  Slot call groups are called in ascending order.  Overloads exist for const- and non-const- versions
    with 0 to 8 arguments.  8 was picked as the max simply because boost::bind only supports up to 8 args as of this
    writing. */
template <class C, class R, class T1, class T2 ,
          typename ... Args> inline
boost::signals2::connection
Connect(boost::signals2::signal<R (Args...), C>& sig,
        R (T1::* fn) (Args...),
        T2 obj,
        boost::signals2::connect_position at = boost::signals2::at_back)
{
	auto xfn = [fn, obj](Args ... args) -> R { return (obj->*fn)(args...); };
    return sig.connect(xfn, at);
}

/** connects a signal to a const member function of a specific object that has the same function signature, putting \a R
    in slot group 0.  Slot call groups are called in ascending order.  Overloads exist for const- and non-const-
    versions with 0 to 8 arguments.  8 was picked as the max simply because boost::bind only supports up to 8 args as of
    this writing. */
template <class C, class R, class T1, class T2,
          typename ... Args> inline
boost::signals2::connection
Connect(boost::signals2::signal<R (Args...), C>& sig,
        R (T1::* fn) (Args...) const,
        T2 obj,
        boost::signals2::connect_position at = boost::signals2::at_back)
{
	auto xfn = [fn, obj](Args ... args) -> R { return (obj->*fn)(args...); };
    return sig.connect(xfn, at);
}

/** connects a signal to a member function of a specific object that has the same function signature, putting \a R in
    slot group \a grp.  Slot call groups are called in ascending order. Overloads exist for const- and non-const-
    versions with 0 to 8 arguments.  8 was picked as the max simply because boost::bind only supports up to 8 args as of
    this writing. */
template <class C, class R, class T1, class T2 ,
          typename ... Args> inline
boost::signals2::connection
Connect(boost::signals2::signal<R (Args...), C>& sig,
        R (T1::* fn) (Args...),
        T2 obj,
        int grp,
        boost::signals2::connect_position at = boost::signals2::at_back)
{
	auto xfn = [fn, obj](Args ... args) -> R { return (obj->*fn)(args...); };
    return sig.connect(grp, xfn, at);
}

/** connects a signal to a const member function of a specific object that has the same function signature, putting \a R
    in slot group \a grp.  Slot call groups are called in ascending order. Overloads exist for const- and non-const-
    versions with 0 to 8 arguments.  8 was picked as the max simply because boost::bind only supports up to 8 args as of
    this writing. */
template <class C, class R, class T1, class T2 ,
          typename ... Args> inline
boost::signals2::connection
Connect(boost::signals2::signal<R (Args...), C>& sig,
        R (T1::* fn) (Args...) const,
        T2 obj,
        int grp,
        boost::signals2::connect_position at = boost::signals2::at_back)
{
	auto xfn = [fn, obj](Args ... args) -> R { return (obj->*fn)(args...); };
    return sig.connect(grp, xfn, at);
}

/** connects a signal to another signal of the same signature, establishing signal-forwarding. \a sig1 places \a sig2 in
    its slot group 0. */
template <class C, class R ,
          typename ... Args> inline
boost::signals2::connection
Connect(boost::signals2::signal<R (Args...), C>& sig1,
        boost::signals2::signal<R (Args...), C>& sig2,
        boost::signals2::connect_position at = boost::signals2::at_back)
{
    typedef typename detail::Forwarder_<C, R , Args...> Forwarder;
    return sig1.connect(Forwarder(sig2), at);
}

/** connects a signal to another signal of the same signature, establishing signal-forwarding. \a sig1 places \a sig2 in
    its slot group \a grp. */
template <class C, class R ,
          typename ... Args> inline
boost::signals2::connection
Connect(boost::signals2::signal<R (Args...), C>& sig1,
        boost::signals2::signal<R (Args...), C>& sig2,
        int grp,
        boost::signals2::connect_position at = boost::signals2::at_back)
{
    typedef typename detail::Forwarder_<C, R , Args...> Forwarder;
    return sig1.connect(grp, Forwarder(sig2), at);
}

} //namespace GG

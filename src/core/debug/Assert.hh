/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2013 Wan Wai Ho

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation version 2
	of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	MA  02110-1301, USA.
*/

#pragma once

#define WB_ASSERT_VOID_CAST static_cast<void>

namespace wb { namespace debug
{
	void AssertFail( const char *expr, const char *file, int line, const char *func ) ;
}}

#ifdef NDEBUG
	#define WB_ASSERT(expr)		(WB_ASSERT_VOID_CAST(0))

#else
	#define WB_ASSERT(expr)	\
		((expr)								\
			? WB_ASSERT_VOID_CAST(0)		\
			: wb::debug::AssertFail(__STRING(expr), __FILE__, __LINE__, __PRETTY_FUNCTION__))

#endif

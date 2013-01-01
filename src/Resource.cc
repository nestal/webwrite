/*
	webwrite: an GPL wiki-like website with in-place editing
	Copyright (C) 2012  Wan Wai Ho

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Resource.hh"
#include "Config.hh"

#include "util/Exception.hh"

#include <iostream>
#include <cassert>

namespace wb {

Resource::Resource( const std::string& uri, const Config& cfg ) :
	m_cfg( &cfg )
{
	std::string wb_root = cfg.Str("wb-root") ;
	
	if ( uri.substr( 0, wb_root.size() ) != wb_root )
		throw Exception() ;
	
	m_path = uri.substr( wb_root.size() ) ;
}

const fs::path& Resource::Path() const
{
	return m_path ;
}

std::string Resource::Filename() const
{
	return m_path.filename().string() ;
}

bool Resource::IsDir() const
{
	return Filename().empty() || Filename() == "." || fs::is_directory(ContentPath()) ;
}

fs::path Resource::ContentPath() const
{
	assert( m_cfg != 0 ) ;
	return m_cfg->Base() / m_cfg->Str("data-path") / m_path ;
}

} // end of namespace

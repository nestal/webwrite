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

#include "Config.hh"

namespace wb {

Config::Config( const std::string& filename ) :
	m_cfg( Json::ParseFile( filename ) ),
	m_base( fs::current_path() )
{
}

std::string Config::Str( const std::string& key ) const
{
	return m_cfg[key].Str() ;
}

/*!	This configuration parameter denotes the base directory. This directory
	contains all the data files required by webwrite, e.g. "_lib" for the
	HTML/javascript files and "data" for storing the raw data.
*/
fs::path Config::Base() const
{
	return m_cfg.Has("base") ? fs::path(m_cfg["base"].Str()) : m_base ;
}

/*!	This configuration parameter contains the name of the default page in a
	directory. Default value: "main".
*/
std::string Config::MainPage() const
{
	return m_cfg.Has("main_page") ? m_cfg["main_page"].Str() : "main" ;
}

} // end of namespace

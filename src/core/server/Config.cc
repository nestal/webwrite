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

namespace wb { namespace cfg {

Json Init( const Json& json )
{
	Json copy( json ) ;
	if ( !copy.Has("base") )
		copy.Add( "base", Json(fs::current_path().string()) ) ;
	
	if ( !copy.Has("main_page") )
		copy.Add( "main_page", Json("main") ) ;
	
	return copy;
}

const Json& Inst( const Json& json )
{
	static const Json cfg = Init( json ) ;
	return cfg ;
}

fs::path Path( const std::string& cfg )
{
	return fs::path(cfg::Inst()[cfg].Str()) ;
}

std::string MimeType( const fs::path& file )
{
	const std::string ext = file.extension().string() ;

	Json r;
	return 
		fs::is_directory(file)	? "inode/directory" :
		(ext.empty()			? "text/html" :
		(cfg::Inst()["mime"].Get(ext, r) ? r.Str() : "application/octet-stream")) ;
}

} // end of namespace cfg

const Cfg& Cfg::Inst( const Json& json )
{
	static const Cfg inst =
	{
		json["name"].Str(),
		json["socket"].Str(),
		json["wb_root"].Str(),
		{
			json["lib"]["path"].Str(),
			json["lib"]["redir"].Str(),
		},
		{
			json["data"]["path"].Str(),
			json["data"]["redir"].Str(),
		},
		{
			json["meta"]["path"].Str(),
			json["meta"]["redir"].Str(),
		},
		{
			json["attic"]["path"].Str(),
			json["attic"]["redir"].Str(),
		},
	} ;
	return inst ;
}

} // end of namespace wb

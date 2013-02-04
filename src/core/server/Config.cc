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

namespace
{
	std::string FindMime( const fs::path& ext, const Cfg::MimeMap& mime )
	{
		Cfg::MimeMap::const_iterator i = mime.find( ext ) ;
		return i != mime.end() ? i->second : "application/octet-stream" ;
	}

	Cfg::MimeMap MakeMimeMap( const Json::Object& json )
	{
		Cfg::MimeMap result ;
		for ( Json::Object::const_iterator i = json.begin() ; i != json.end() ; ++i )
			result.insert( std::make_pair( fs::path( i->first ), i->second.Str() ) ) ;
		return result ;
	}
	
	std::string Optional(
		const Json&			json,
		const std::string&	key,
		const std::string&	def = "" )
	{
		Json result ;
		return json.Get(key, result) ? result.Str() : def ;
	}
} // end of local namespace

std::string Cfg::MimeType( const fs::path& file )
{
	const fs::path ext = file.extension() ;

	return 
		fs::is_directory(file)	? "inode/directory" :
		(ext.string().empty()	? "text/html" :
		(FindMime(ext, Cfg::Inst().mime))) ;
}

const Cfg& Cfg::Inst( const Json& json )
{
	static const Cfg inst =
	{
		Optional( json, "name", "WebWrite" ),
		Optional( json, "socket" ),
		json["wb_root"].Str(),
		Optional( json, "main_page", "main" ),
		
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
		{
			json["log"]["level"].Str(),
			json["log"]["file"].Str(),
		},
		MakeMimeMap( json["mime"].AsObject() ),
	} ;
	return inst ;
}

} // end of namespace wb

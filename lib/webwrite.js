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

function main()
{
	// once we know the URL to the root page, we can assign it to the logo link
	$.getJSON( "?var", function(data){
		var wb_base = document.location.protocol + "//" + document.location.host + data.wb_root + "/" ;

		// default is viewer
		LoadViewer( document.location, wb_base ) ;
		 
		$(".logo").click( function(){
			LoadViewer( data.wb_root + data.main, wb_base ) ;
		});
	}) ;
}

/*	this function transform the href from relative to absolute. It is called to transform
	the HTML data from server and then put it in wysihtml5.
*/
function ToAbsoluteHref( a, wb_base )
{
	var href = a.attr( "href" ) ;
	if ( href == null )
	{
	}
	else
	{
		var abs_re = /^(https?:\/\/|mailto:).*/ ;
		if ( !href.match(abs_re) )
		{
			console.log( "converting \"" + href + "\" to \"" + wb_base + href + "\"\n" ) ;
			a.attr( "href", wb_base + href ) ;
		}
	}
}

function LoadEditor( res_path, wb_base )
{
	$("#content").load( res_path + "?lib=editor.html", function()
	{
		var editor = new wysihtml5.Editor( "rich_editor",
		{
			stylesheets:	["?lib=webwrite.css"],
			toolbar:		"rich_toolbar",
			parserRules:	wysihtml5ParserRules
		});
		
		InitToolbar( editor, res_path, wb_base ) ;
		
		// load content from server
		$.get( res_path + "?load", function(data)
		{
			// do some transform before setting to editor
			var $dom = $( "<div>" + data + "<div>" );
			$dom.find( "a" ).each( function(){ ToAbsoluteHref( $(this), wb_base ); }) ;

			editor.setValue( $dom.html() ) ;
		}, "text") ;
	}) ;
}

function LoadViewer( res_path, wb_base )
{
	if ( wb_base == null )
		alert( "oops!" ) ;

	$("#content").load( res_path + "?lib=viewer.html", function()
	{
		$("#viewer").load( res_path + "?load", function()
		{
			$("a").attr( "target", "_blank" ) ;

			// use ajax instead of a full page load when clicks an internal link
			$("a").click( function(ev)
			{
				var raw_href = this.getAttribute("href") ;

				// relative links
				if ( raw_href.substring(0,2) == "./" || raw_href.substring(0,3) == "../" )
				{
					history.pushState( null, null, raw_href ) ;

					LoadViewer( raw_href, wb_base  ) ;
					ev.preventDefault() ;
				}
			}) ;
		} ) ;
		
		$("#toogle-edit").click( function()
		{
			LoadEditor( res_path, wb_base ) ;
		}) ;
	}) ;
}
	
/*	$("#drop").on( "dragover", function(event)
	{
		event.preventDefault() ;
		event.stopPropagation() ;
	});

	$("#drop").on( "dragenter", function(event)
	{
		event.preventDefault() ;
		event.stopPropagation() ;
	});

	// callback when user drops file
	$("#drop").on( "drop", function(event)
	{
		if ( event.preventDefault )
			event.preventDefault() ;

		var form_data = new FormData() ;
		$.each( event.originalEvent.dataTransfer.files, function(index, file)
		{
			form_data.append( file.name, file ) ;
		}) ;
		
		$.ajax({
			url:			res_path + "?upload",
			type:			"POST",
			data:			form_data,
			processData:	false,
			contentType:	false,
			success:		function(r)
			{
				alert( "OK! " + r ) ;
			}
		}) ;
	}) ;
*/

function InitToolbar( editor, res_path, wb_base )
{
	$("#save_button").click( function()
	{
		$.post(
			res_path + "?save",
			editor.getValue(),
			function(data, status)
			{
				// quit the editor
				LoadViewer( res_path, wb_base ) ;
			}) ;
	}) ;
	
	$("#cancel_button").click( function()
	{
		LoadViewer( res_path, wb_base ) ;
	});

	$("#switch_button").click( function()
	{
		editor.toolbar.execAction( "change_view" );
	});
	
	$("#bold_button").click( function()
	{
		editor.composer.commands.exec( "bold" );
	});
	
	$("#italic_button").click( function()
	{
		editor.composer.commands.exec( "italic" );
	});

	$("#h1_button").click( function()
	{
		editor.composer.commands.exec( "formatBlock", "h1" );
	});
	$("#h2_button").click( function()
	{
		editor.composer.commands.exec( "formatBlock", "h2" );
	});
	$("#h3_button").click( function()
	{
		editor.composer.commands.exec( "formatBlock", "h3" );
	});
	$("#h4_button").click( function()
	{
		editor.composer.commands.exec( "formatBlock", "h4" );
	});

	// lists
	$("#ordered_list_button").click( function()
	{
		editor.composer.commands.exec( "insertOrderedList" );
	});
	$("#unordered_list_button").click( function()
	{
		editor.composer.commands.exec( "insertUnorderedList" );
	});
}

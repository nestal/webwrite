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
	document.title = "WebWrite: " + document.location.pathname ;

	// default is viewer
	LoadViewer( document.location ) ;
}

function LoadEditor( res_path )
{
	$("#content").load( res_path + "?lib=editor.html", function()
	{
		var editor = new wysihtml5.Editor( "rich_editor",
		{
			stylesheets:	["?lib=webwrite.css"],
			toolbar:		"rich_toolbar",
			parserRules:	wysihtml5ParserRules
		});

		InitToolbar( editor, res_path ) ;
		
		// load content from server
		$.get( res_path + "?load", function(data)
		{
			editor.setValue( data ) ;
		}, "text") ;
	}) ;
}

function LoadViewer( res_path )
{
	$("#content").load( res_path + "?lib=viewer.html", function()
	{
		$("#viewer").load( res_path + "?load", function()
		{
			$("a").attr( "target", "_blank" ) ;

			// use ajax instead of a full page load when clicks an internal link
			$("a").click( function(ev)
			{
				var raw_href = this.getAttribute("href") ;

				var pat = /^https?:\/\//i;
				if ( !pat.test( raw_href ) )
				{
					history.pushState( null, null, raw_href ) ;

					LoadViewer( raw_href ) ;
					ev.preventDefault() ;
				}
			}) ;
		} ) ;
		
		$("#toogle-edit").click( function()
		{
			LoadEditor( res_path ) ;
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

function InitToolbar( editor, res_path )
{
	$("#save_button").click( function()
	{
		$.post(
			res_path + "?save",
			editor.getValue(),
			function(data, status)
			{
//				alert( "OK! " + status ) ;
				
				// quit the editor
				LoadViewer( res_path ) ;
			}) ;
	}) ;

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

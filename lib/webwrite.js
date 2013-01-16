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

		// inject the wb_base to the property of the "window" object to make it accessible
		Object.defineProperty( window, "wb_base",
		{
			value:		wb_base,
			writable:	false,
		}) ;
		
		// default is viewer
		LoadViewer() ;
		
		$(".logo").html( data.name ) ;

		// go to the main page if the logo is clicked
		$(".logo").click( function(){
			LoadViewer( data.wb_root + "/" + data.main ) ;
		});
	}) ;
	
	// handle back button for ajax
	$(window).bind( "popstate", function() {
		LoadViewer( document.location.href );
	});
	
	// close button of the dialog
	$("div#dialog a#dialog_xbutton").click(CloseDialog) ;
}

function IsValidRelativeHref( href )
{
	return href != null && (href.substring(0,2) == "./" || href.substring(0,3) == "../") ;
}

function IsInternalLink( href )
{
	return href.substring(0, window.wb_base.length) == window.wb_base ;
}

function BaseLocation( loc )
{
	if ( loc == null )
		loc = document.location.href ;
	
	return loc.substring( 0, loc.lastIndexOf("/") ) + "/" ;
}

/*	this function transform the href from relative to absolute. It is called to transform
	the HTML data from server and then put it in wysihtml5.
*/
function ToAbsoluteHref( a, attr_name )
{
	if ( attr_name == null )
		attr_name = "href" ;
	
	var href = a.attr( attr_name ) ;
	var abs_re = /^(https?:\/\/|mailto:).*/ ;
	if ( href != null && !href.match(abs_re) )
		a.attr( attr_name, BaseLocation() + href ) ;
}

function ToRelativeHref( a, attr_name )
{
	if ( attr_name == null )
		attr_name = "href" ;
	
	var href	= a.attr(attr_name) ;
	var abs_re	= /^(https?:\/\/|mailto:).*/ ;
	var base	= BaseLocation() ;
	
	if ( href != null && href.match(abs_re) && href.substring(0, base.length) == base )
	{
		href = href.substring(base.length) ;
	
		if ( !IsValidRelativeHref(href) )
			href = "./" + href ;
	}
console.log( "ToRelHref(): " + attr_name + " " + href ) ;
	a.attr( attr_name, href ) ;
}

// similar to LoadViewer(), the URL of the document to be loaded will be
// the one in the address bar. call history.pushState() to change it.
function LoadEditor( )
{
	$("#content").load( "?lib=editor.html", function()
	{
		// initialize the link input box to something familiar
		$("#link_input").attr( "value", window.wb_base ) ;
		
		var editor = new wysihtml5.Editor( "rich_editor",
		{
			stylesheets:	["?lib=webwrite.css"],
			toolbar:		"rich_toolbar",
			parserRules:	wysihtml5ParserRules
		});
		
		InitToolbar( editor ) ;
		
		// load content from server
		$.get( "?load", function(data)
		{
			// do some transform before setting to editor
			var $dom = $( "<div>" + data + "</div>" );
			$dom.find("a").each( function(){ ToAbsoluteHref( $(this) ); }) ;
			$dom.find("img").each( function(){ ToAbsoluteHref( $(this), "src" ); }) ;

			// remove boilerplate
			$dom.find("#boilerplate").remove() ;
			
			editor.setValue( $dom.html() ) ;
		}, "text") ;
	}) ;
}

// load the view directly using ajax if it is an internal link
function OnClickLinks( ev, a )
{
	var href	= a.getAttribute("href") ;
	if ( IsInternalLink( href ) )
	{
		console.log( "following internal link: \"" + href + "\"") ;
		ev.preventDefault() ;
		LoadViewer( href ) ;
	}

	// for external links, don't call ev.preventDefault() and the browser will follow the link
}

// view the page destinated by "href", which should be an absolute URI inside webwrite.
// if "href" is null, the same page will be reloaded. this is used to "quit" the editor.
function LoadViewer( href )
{
	// push the new href to history before calling loading the viewer.
	// history.pushState() will update the address bar to the new href.
	// LoadViewer() will load the page specified in the address bar.
	if ( href )
		history.pushState( null, null, href ) ;
	
	console.log( "base = " + BaseLocation() ) ;
	
	// the URL is always relative the the current document location
	$("#content").load( "?lib=viewer.html", function(){
		
		$("#viewer").load( "?load", function(){
			
			// convert all relative links to absolute links
			$("a", "#viewer").each( function(){ ToAbsoluteHref( $(this) ); }) ;

			// convert all image sources to absolute links
			$("img", "#viewer").each( function(){ ToAbsoluteHref( $(this), "src" ); }) ;
			
			// use ajax instead of a full page load when clicks an internal link
			$("a", "#viewer").click( function(ev){ OnClickLinks( ev, this ); }) ;
			
			$("span.edit_cmd", "#viewer").click(function(){LoadEditor();});
		} ) ;
		
		$("#edit_button").click(function(){LoadEditor();}) ;
		$("#upload_button").click( ShowDropDialog ) ;
	}) ;
	
	// load the directory index
	$("#menu").load( "?index", function(){
		
		// click links on the menu
		$("li.menu_idx > a").click( function(ev){
			console.log( "following index link: \"" + $(this).attr("href") + "\"") ;
			ev.preventDefault() ;

			LoadViewer( $(this).attr("href") );
		}) ;
	} );
}

function ShowDropDialog()
{
	$("#dialog h1").html("Upload file") ;
	$("#dialog_body").html("<div id=\"drop\">Please drop files here!</div>") ;
	ShowDialog() ;

	$("#drop").on( "dragover", function(event)
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
			url:			"?upload",
			type:			"POST",
			data:			form_data,
			processData:	false,
			contentType:	false,
			success:		function(r)
			{
				// notify user upload complete
				$("#dialog_body").html("<div>Upload complete!</div>") ;
				ShowDialog() ;
			}
		}) ;
	}) ;
}

function InitToolbar( editor )
{
	$("#save_button").click( function()
	{
		// do some transform before uploading to server
		var $dom = $( "<div>" + editor.getValue() + "</div>" );
		$dom.find("a").each( function(){ ToRelativeHref( $(this) ); }) ;
		$dom.find("img").each( function(){ ToRelativeHref( $(this), "src" ); }) ;

		// quit the editor by loading the viewer for the same page after the
		// server returns
		$.post(
			"?save",
			$dom.html(),
			function(data, status){LoadViewer();}
		);
	}) ;
	
	// quit the editor without saving, by loading the viewer for the same page
	$("#cancel_button").click( function(){LoadViewer();});

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
	$("#upload_button").click( ShowDropDialog ) ;

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

function ShowDialog()
{
	$("#overlay").show() ;
	$("#dialog").show() ;
}

function CloseDialog()
{
	$("#dialog").hide() ;
	$("#overlay").hide() ;
}

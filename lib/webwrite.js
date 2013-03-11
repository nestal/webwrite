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
	// don't create editors automatically because we want to do it when people press "edit"
	CKEDITOR.disableAutoInline = true;

	// once we know the URL to the root page, we can assign it to the logo link
	$.getJSON( "?var", function(cfg){
		var wb_base = document.location.protocol + "//" + document.location.host + cfg.wb_root + "/" ;

		// inject the wb_base to the "body" element to read it later
		$("body").data( "wb_base", wb_base ) ;
		
		// initialize logo
		$(".logo").html( cfg.name ) ;
		$(".logo").click( function(){Load(cfg.wb_root + "/" + cfg.main);});
		
		// show contents by default
		Load( ) ;
	}) ;
	
	// handle back button for ajax
	$(window).bind( "popstate", function() {
		Load() ;
		ReloadMenu() ;
	});
	
	// close button of the dialog
	$("div#dialog a#dialog_xbutton").click(CloseDialog) ;

	// standard buttons
	$("#edit_button").click( ToggleEdit ) ;
	$("#upload_button").click( ShowDropDialog ) ;
	$("#save_button").click( SaveDoc ) ;
}

function IsValidRelativeHref( href )
{
	return href != null && (href.substring(0,2) == "./" || href.substring(0,3) == "../") ;
}

function IsInternalLink( href )
{
	var wb_base = $("body").data("wb_base") ;
	return href.substring(0, wb_base.length) == wb_base ;
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
	
	// for fragments (i.e. links started with #) leave it as-is
	if ( href != null && href.substr(0,1) != "#" && href.substr(0,5) != "data:" )
	{
		var abs_re = /^(https?:\/\/|mailto:).*/ ;
		if ( !href.match(abs_re) )
			a.attr( attr_name, BaseLocation() + href ) ;
	}
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

// load the view directly using ajax if it is an internal link
function OnClickLinks( ev, a )
{
	var href	= a.getAttribute("href") ;
	if ( IsInternalLink( href ) )
	{
		console.log( "following internal link: \"" + href + "\"") ;
		ev.preventDefault() ;
		Load( href ) ;
	}

	// for external links, don't call ev.preventDefault() and the browser will follow the link
}

function ToggleEdit()
{
	var editable = $("#content").attr( "contenteditable" );
	if ( editable == "true" )
	{
		$("#content").attr( "contenteditable", "false" ) ;
		CKEDITOR.instances.content.destroy() ;
		
		InitViewer() ;
	}
	else
	{
		$("#boilerplate", "#content").remove() ;
		$("#content").attr( "contenteditable", "true" ) ;
		CKEDITOR.inline("content") ;
		CKEDITOR.instances.content.focus() ;
	}
}

function SaveDoc()
{
	var editor = CKEDITOR.instances.content ;
	if ( editor != null )
	{
		// do some transform before uploading to server
		var $dom = $( "<div>" + editor.getData() + "</div>" );
		$dom.find("a").each( function(){ ToRelativeHref( $(this) ); }) ;
		$dom.find("img").each( function(){ ToRelativeHref( $(this), "src" ); }) ;
		
		$.post(
			"?data",
			$dom.html(),
			function(data, status){
				ReloadMenu() ;
			}
		);
	}
}

function InitViewer( )
{
	// convert all relative links to absolute links
	$("a", "#content").each( function(){ ToAbsoluteHref( $(this) ); }) ;

	// use ajax instead of a full page load when clicks an internal link
	$("a", "#content").click( function(ev){ OnClickLinks( ev, this ); }) ;
	
	// convert all image sources to absolute links
	$("img", "#content").each( function(){ ToAbsoluteHref( $(this), "src" ); }) ;
		
	// "Do you want to create this page?"
	$("span.edit_cmd", "#content").click(ToggleEdit);

	// switch to viewer
	$("#content").show() ;
	$("#meta").hide() ;
	$("#prop_button").off() ;
	$("#prop_button").click( function(){ Load("?meta");} ) ;
}

function ReloadMenu()
{
	$("#menu").load( "?index", function(){
		
		$(this).removeClass("unitialized") ;

		// click links on the menu
		$("li.menu_idx > a").click( function(ev){
			console.log( "following index link: \"" + $(this).attr("href") + "\"") ;
			ev.preventDefault() ;
			
			// the mime type of the target is stored in the class of parent node
			var parent = $(this).parent() ;
			
			// only load if it is a HTML file or a directory
			Load( $(this).attr("href") );
		}) ;
	} );
}

// view the page destinated by "href", which should be an absolute URI inside webwrite.
// if "href" is null, the same page will be reloaded. this is used to "quit" the editor.
function Load( href )
{
	// if "href" is null, that means loading the same page
	// this check must be done before pushing history state because pushState() will
	// change document.location
	var same_dir = ( href == null
		? true
		: (BaseLocation(document.location.pathname) == BaseLocation(href)) ) ;

	// push the new href to history before calling loading the viewer.
	// history.pushState() will update the address bar to the new href.
	// Load() will load the page specified in the address bar.
	if ( href )
		history.pushState( null, null, href ) ;

	// the URL is always relative the the current document location
	var real_href	= document.location.toString() ;
	var find_qstr	= real_href.indexOf('?') ;
	var qstr		= real_href.substr(find_qstr) ;
	if ( find_qstr == -1 )
	{
		$("#content").load("?data", InitViewer) ;
	}
	else if ( qstr == "?meta" )
	{
		console.log( "hash = " + real_href.substr(0,find_qstr) ) ;
		ShowProperties(real_href.substr(0,find_qstr)) ;
	}
	
	// load the directory index only if the directory is changed
	if ( !same_dir || $("#menu").hasClass("unitialized") )
		ReloadMenu() ;
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

		console.log( "drop event" ) ;
		var form_data = new FormData() ;
		$.each( event.originalEvent.dataTransfer.files, function(index, file)
		{
			console.log( "adding " + file.name + " to form data" ) ;
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
				// reload menu to show the new file
				ReloadMenu() ;
				
				// notify user upload complete
				$("#dialog_body").html("<div>Upload complete!</div>") ;
				ShowDialog() ;
			}
		}) ;
	}) ;
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

function ShowProperties(href)
{
	$.getJSON( "?prop", function(data){
		
		// initialize name and the link
		$("#meta_name").html(data.name) ;
		$("#meta_name").attr( "href", href ) ;
		$("#meta_name").off("click") ;
		$("#meta_name").click(function(ev){
			if ( data.type == "text/html" )
				OnClickLinks(ev, this) ;
			else
			{
				window.open( href ) ;
				ev.preventDefault() ;
			}
		}) ;

		// show modified time		
		var modified = new Date() ;
		modified.setTime(data["last-modified"] * 1000) ;
		$("#meta_modified").html(modified.toString()) ;
		
		InitProperties(href) ;
	}) ;
}

function InitProperties( href )
{
	// show meta data
	$("#content").hide() ;
	$("#meta").show() ;
	$("#prop_button").off() ;
	$("#prop_button").click( function(){ Load(href);} ) ;
}

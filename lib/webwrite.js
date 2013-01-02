var g_alder = "haha" ;

function OnLoad()
{
	$("#tabs").tabs();

	var editor = new wysihtml5.Editor( "rich_editor",
	{
		stylesheets:	["_webwrite.css"],
		toolbar:		"rich_toolbar",
		parserRules:	wysihtml5ParserRules
	});

	InitToolbar( editor ) ;
	
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

	$("#drop").on( "drop", function(event)
	{
		if ( event.preventDefault )
			event.preventDefault() ;

		var file = event.originalEvent.dataTransfer.files[0] ;
		var reader = new FileReader() ;
		reader.onload = function(event)
		{
			$.post(
				document.location + file.name + "?save",
				event.target.result
			);
		} ;
		reader.readAsDataURL( file ) ;
	}) ;

	// load content from server
	$.get( document.location + "?load", function(data)
	{
		editor.setValue( data ) ;
	}, "text") ;
}

function InitToolbar( editor )
{
	$("#save_button").button().click( function()
	{
		$.post(
			document.location + "?save",
			editor.getValue(),
			function(data, status)
			{
				alert( "OK! " + status ) ;
			}) ;
	
	}) ;

	$("#switch_button").button().click( function()
	{
		editor.toolbar.execAction( "change_view" );
	});
	
	$("#bold_button").button().click( function()
	{
		editor.composer.commands.exec( "bold" );
	});
	
	$("#italic_button").button().click( function()
	{
		editor.composer.commands.exec( "italic" );
	});

	$("#h1_button").button().click( function()
	{
		editor.composer.commands.exec( "formatBlock", "h1" );
	});
	$("#h2_button").button().click( function()
	{
		editor.composer.commands.exec( "formatBlock", "h2" );
	});
	$("#h3_button").button().click( function()
	{
		editor.composer.commands.exec( "formatBlock", "h3" );
	});
	$("#h4_button").button().click( function()
	{
		editor.composer.commands.exec( "formatBlock", "h4" );
	});

	// lists
	$("#ordered_list_button").button().click( function()
	{
		editor.composer.commands.exec( "insertOrderedList" );
	});
	$("#unordered_list_button").button().click( function()
	{
		editor.composer.commands.exec( "insertUnorderedList" );
	});
	
	
	// everything marked as "make_button" will be made as buttons.
	$(".make_button").button() ;
}

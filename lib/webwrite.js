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
	
	// load content from server
	$.get( document.location + "?load", function(data)
	{
		editor.setValue( data ) ;
	}, "text") ;
}

function InitToolbar( editor )
{
	$("#save_button").button().click( function(){
		alert( editor.getValue() ) ;
		
		$.post(
			document.location + "?save",
			editor.getValue(),
			function(data, status)
			{
				alert( "OK! " + data ) ;
			}) ;
	
	}) ;

	$("#switch_button").button().click( function(){
		editor.toolbar.execAction( "change_view" );
	});
	
	$("#bold_button").button().click( function(){
		editor.composer.commands.exec( "bold" );
	});
	
	$("#italic_button").button().click( function(){
		editor.composer.commands.exec( "italic" );
	});
	
	$(".make_button").button() ;
}

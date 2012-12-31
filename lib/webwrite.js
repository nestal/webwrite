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

	// buttons in toolbar
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
	
	$("#bold_button").button({
	});
	$("#italic_button").button({
	});
	
	// load content from server
	$.get( document.location + "?load", function(data)
	{
		editor.setValue( data ) ;
	}, "text") ;
}

function OnSave()
{
/*	var json =
	{
		"alder":	g_alder,
		"content":	$("#content").tinymce().getContent()
	} ;

	$.post(
		document.location + "?save",
		JSON.stringify( json ),
		function(data, status)
		{
			alert( "OK! " + data ) ;
		}) ;
*/
	alert( $("#rich_editor").getValue() ) ;
}

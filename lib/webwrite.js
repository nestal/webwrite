var g_alder = "haha" ;

function OnLoad()
{
	$("#tabs").tabs();

	var editor = new wysihtml5.Editor("rich_editor",
	{
		stylesheets:	["_webwrite.css"],
		toolbar:		"rich_toolbar",
		parserRules:	wysihtml5ParserRules
	});
/*
	$.get( document.location + "?load", function(data)
	{
		// extract the "body" of the response and convert to string
		var json = JSON.parse( data ) ;
		
		$("#content").html( json.content ) ;
		
		// use browser to parse the string into HTML again
		$("#editor").html( json.content ) ;
	}, "text") ;*/
}

function OnSave()
{
	var json =
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
}

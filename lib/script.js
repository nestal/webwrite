function OnLoad()
{
	// script the save button
	$( "#save" ).click( function()
	{
		$.post( document.location + "?save", $("#content").html(), function(data, status)
		{
			alert( "OK! " + data ) ;
		}) ;
	}) ;
	
	// load document content
	$("#content").load( document.location + "?load" ) ;
}

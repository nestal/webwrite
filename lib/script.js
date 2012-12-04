function OnLoad()
{
	$( "#save" ).click( function()
	{
		$.post( document.location + "?save", $("#the_text").html(), function(data, status)
		{
			alert( "OK! " + data ) ;
		}) ;
	}) ;
}

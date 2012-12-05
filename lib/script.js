function OnLoad()
{
	$( "#save" ).click( function()
	{
		$.post( document.location + "?save", $("#content").html(), function(data, status)
		{
			alert( "OK! " + data ) ;
		}) ;
	}) ;
}

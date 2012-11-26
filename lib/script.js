function OnLoad()
{
	alert( "hi" ) ;
	$( "#save" ).click( function()
	{
		$.post( "/fcgi" + $("#path").val(), $("#the_text").html(), function(data, status)
		{
			alert( "OK! " + data ) ;
		}) ;
	}) ;
}

function OnLoad()
{
	alert( "hi" ) ;
	$( "#save" ).click( function()
	{
		$.post( "/webwrite/" + $("#path").val(), $("#the_text").html(), function(data, status)
		{
			alert( "OK! " + data ) ;
		}) ;
	}) ;
}

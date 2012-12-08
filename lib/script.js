function OnLoad()
{
	var editor = new wysihtml5.Editor(
		"content",	// id of textarea element
		{
			toolbar:      "toolbar",  // id of toolbar element
			parserRules:  wysihtml5ParserRules, // defined in parser rules set 
			stylesheets:  "_stylesheet.css",
		});

/*	bkLib.onDomLoaded(function(){
		var editor = new nicEditor({
			fullPanel : true,
			onSave : function(content, id, instance)
			{
				$.post( document.location + "?save", content, function(data, status)
				{
					alert( "OK! " + data ) ;
				}) ;
			
			}}) ;
		
		editor.panelInstance('content');
		
		$.get( document.location + "?load", function(data)
		{
			nicEditors.findEditor("content").setContent( data ) ;
		}, "text" ) ;
	});
*/
	// load document content
// 	$("#content").load( document.location + "?load" ) ;
}

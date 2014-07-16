
//this function used for inserting of TAB in <textarea>
function insertTab(o, e) {
	var kC = e.keyCode ? e.keyCode : e.charCode ? e.charCode : e.which;
	if (kC == 9 && !e.shiftKey && !e.ctrlKey && !e.altKey) { // tab was pressed
	    //get vertical scrollbar position (old versions of mozilla)
	    var oS = o.scrollTop;
	    if (o.setSelectionRange)
	    {	//for Mozilla
		// get caret position/selection
		var val = o.value,
		start = o.selectionStart,
		end = o.selectionEnd;

		// set textarea value to: text before caret + tab + text after caret
		o.value = val.substring(0, start) + '\t' + val.substring(end);

		// put caret at right position again
		o.selectionStart = o.selectionEnd = start + 1;
		//o.focus();
	    }
	    else if (o.createTextRange)
	    {	//for IE
		document.selection.createRange().text = "\t";
		e.returnValue = false;
	    }

	    //set vertical scrollbar position
	    o.scrollTop = oS;
	    //stop the default action of an element from happening
	    if (e.preventDefault)
	    {
		e.preventDefault();
	    }
            // prevent the focus lose
            return false;

        }

}

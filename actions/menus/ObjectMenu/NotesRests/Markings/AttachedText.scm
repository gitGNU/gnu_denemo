;;;AttachedText
(let ((text #f) (tag "AttachedText")(markup #f)(current #f)
			(position (d-PopupMenu (list (cons (_ "Above") "^")  (cons (_ "Below") "_") (cons (_ "Auto Position") "-")))))
	(set! current (d-DirectiveGet-chord-display tag))
	(if (not current)
		(set! current ""))
	(set! text (d-GetUserInputWithSnippets (_ "Text") (_ "Give text to appear with note/chord: ") current));;cannot popup menu after this, it runs gtk_main
	(if text 
	  (begin 
			(if position
			   (begin
						(set! markup (cdr text))
						(set! text (car text))
						(d-DirectivePut-chord-display tag  text )
						(d-DirectivePut-chord-postfix tag  (string-append position "\\markup { \\override  #'(line-width . 40) " markup "}"))
						(d-DirectivePut-chord-minpixels  tag 20)
						(d-SetSaved #f))))
		(begin
	  	(let ((confirm (d-GetUserInput (d-DirectiveGet-chord-display tag) (_ "Delete this text?") (_ "y"))))
	  	 (if (and confirm (equal? confirm (_ "y")))
	  		(begin
	  			(d-DirectiveDelete-chord tag)
	  			(d-SetSaved #f))
			(d-InfoDialog (_ "Cancelled")))))))
			
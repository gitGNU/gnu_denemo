;;; CriticalComment
(let ((tag  "CriticalComment") (lilypond ""))
(define current (d-DirectiveGet-standalone-display tag))
(let script ((answer (d-GetUserInputWithSnippets "Critical Comment" "Give Comment" (if current current ""))))
	(set! lilypond (cdr answer))
	(set! answer (car answer))
	(if (and answer (not (string=? answer "")))
		(begin
			(d-Directive-standalone tag)
			(d-DirectivePut-standalone-minpixels tag 30)
			(d-DirectivePut-standalone-override tag
				(logior DENEMO_OVERRIDE_HIDDEN DENEMO_OVERRIDE_EDITOR))
			(d-DirectivePut-standalone-display tag  answer)
			(d-DirectivePut-standalone-postfix tag lilypond)
			(d-RefreshDisplay)
			(d-SetSaved #f))
		#f)))
;;; MovementPageBreak
(if (d-Directive-movementcontrol? "TitledPiece")
	(let ((title (d-DirectiveGet-movementcontrol-display "TitledPiece")))
		 (d-DirectiveDelete-header "MovementPageBreak")
		 (d-DirectiveDelete-movementcontrol "TitledPiece") ;; to put page break before title
		 (d-InfoDialog (string-append "Movement Page Break " (if (d-Directive-movementcontrol? "MovementPageBreak") "Removed" "Inserted") " before Movement Title"))
		 (if (d-Directive-movementcontrol? "MovementPageBreak")
		 	(d-DirectiveDelete-movementcontrol "MovementPageBreak")
			(begin
			  (d-DirectivePut-movementcontrol-override "MovementPageBreak" (logior DENEMO_OVERRIDE_GRAPHIC DENEMO_OVERRIDE_TAGEDIT))
			  (d-DirectivePut-movementcontrol-display "MovementPageBreak" "Page Break before Movement")
			  (d-DirectivePut-movementcontrol-prefix  "MovementPageBreak" "\n\\pageBreak\n")))
		(d-DirectivePut-movementcontrol-display "TitledPiece" title)
		(d-DirectivePut-movementcontrol-prefix  "TitledPiece" (string-append "\\titledPiece \\markup \"" title "\"\n"))
		(d-DirectivePut-movementcontrol-override "TitledPiece" (logior DENEMO_OVERRIDE_TAGEDIT DENEMO_OVERRIDE_GRAPHIC)))
	(let ()
		(if (and (not MovementPageBreak::params) (d-Directive-header? "MovementPageBreak"))
   			 (d-DirectiveDelete-header "MovementPageBreak")
    			(begin
    			(disp "Putting old style page break")
				(d-DirectivePut-header-postfix "MovementPageBreak" "breakbefore = ##t\n")
				(d-DirectivePut-header-display "MovementPageBreak" "(Page Break)")
				(d-DirectivePut-header-override "MovementPageBreak" (logior DENEMO_OVERRIDE_GRAPHIC DENEMO_OVERRIDE_TAGEDIT))))))
(d-SetSaved #f)
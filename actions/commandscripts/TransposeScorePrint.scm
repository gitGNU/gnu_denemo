;;; Warning!!! This file is derived from those in actions/menus/... do not edit here
 ;;;;;;;;;;; TransposeScorePrint
(if (not (defined? 'Transpose::Interval))
    (begin
	  (define Transpose::Interval "c ees")
   ;;;(d-LoadCommand "/MainMenu/EditMenu/Transpose/SetTransposeIntervalFromSelection")
   ;; (d-InitializeScript "SetTransposeIntervalFromSelection")
	  ))
(let ((lily #f) (text #f))
  (set! Transpose::Interval (d-GetUserInput (_ "Set Transpose Interval") (_ "Give Interval to transpose by as two note names, 
 for example \"c g\" means transpose 5th up.
Note names are in Dutch!!! a,b,c ... are the same but
\"es\" = flat, so e.g. bes means b-flat
\"is\" = sharp so e.g fis means f-sharp
Use commas for octave(s) down, 
single-quotes for octave(s) up
e.g. c c' means octave up.
You do not have to start with c
e.g. d e means a tone higher.
") Transpose::Interval))
(if Transpose::Interval
  (begin
  (set! lily (string-append  "\\transpose " Transpose::Interval " "))
  (set! text (string-append  "Print transposed:  " Transpose::Interval " "))
  (d-DirectivePut-score-postfix  "TransposeScorePrint" lily)
  (d-DirectivePut-score-display  "TransposeScorePrint" text)
  (d-DirectivePut-score-override  "TransposeScorePrint" DENEMO_OVERRIDE_GRAPHIC)
  (d-SetSaved #f))))

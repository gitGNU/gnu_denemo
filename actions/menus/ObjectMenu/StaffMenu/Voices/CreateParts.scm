;;;CreateParts
(let ()

(define (delete-lowest-notes)
  ;delete lowest note of each chord
  (d-MoveToBeginning)
  (let loop () 
    (if (d-NextChord) 
	(begin 
	  (if (d-CursorToNote (d-GetNote)) (d-StagedDelete))
	  (loop)))))

(define (delete-upper-notes)
  (d-MoveToBeginning)
  (let loop ((continue #f))
    (if (d-NextChord)
	(begin 
	  (if (d-CursorToNote (d-GetNote 2))
	      (begin 
		(d-RemoveNoteFromChord)
		(set! continue #t)
		(loop continue))
	      (loop continue)))
	(if continue
	    (begin
	      (d-MoveToBeginning)
	      (loop  #f))))))

(define (pause) (disp "Pausing..." "Press any key" " "))


(d-MoveToBeginning)
(d-GoToEnd)
(d-Copy)
(d-AddAfter)
(d-Paste)
(d-MoveToBeginning)
(d-VoicePreset1)
(d-MoveCursorLeft)
(delete-lowest-notes);;delete part 3
(delete-lowest-notes);;delete part 2

(d-AddAfter)
(d-Paste)
(pause)
(d-MoveToBeginning)
(d-VoicePreset2)
(d-MoveCursorLeft)
(delete-lowest-notes);;delete part 3
(delete-upper-notes);;delete part 1 (and anything extra)

(d-AddAfter)
(d-Paste)
(pause)
(d-MoveToBeginning)
(d-VoicePreset3)
(d-MoveCursorLeft)
(delete-upper-notes);;delete parts 2 and 1 (and anything extra)
(d-SimplifyTies)
(d-MoveToStaffUp)
(d-SimplifyTies)
(d-MoveToStaffUp)
(d-SimplifyTies)
)

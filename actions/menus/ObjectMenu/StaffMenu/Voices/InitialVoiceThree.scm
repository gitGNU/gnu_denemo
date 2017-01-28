;;;InitialVoiceThree
(let ((tag "InitialVoice") (move-right #f))
(d-PushPosition)
(d-DirectivePut-voice-display tag (_ "Voice Three"))
(d-DirectivePut-voice-postfix tag "\\voiceThree")
(d-DirectivePut-voice-override tag DENEMO_OVERRIDE_GRAPHIC)
(d-MoveToBeginning)
(if (StemDirective?) (d-DeleteObject) (set! move-right #t))
(d-StartUpStems)
(d-MoveCursorLeft)
(d-PopPosition)
(if move-right
  (d-MoveCursorRight));;to compensate for the insertion of the stem directive
(d-SetSaved #f))

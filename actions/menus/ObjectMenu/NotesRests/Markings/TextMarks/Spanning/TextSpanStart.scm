;;;TextSpanStart
(if (Music?)
    (let ((tag "TextSpanStart")(text #f))
    (if (d-Directive-chord? tag)
        (d-DirectiveDelete-chord tag)
        (begin
            (if (and (string? TextSpanStart::params) (not (equal? TextSpanStart::params "edit")))
                    (set! text TextSpanStart::params)
                    (set! text (d-GetUserInput (_ "Text Spanner") (_ "Give text ") "rall")))
            (if text
                (begin
                    (d-DirectivePut-chord-prefix tag  (string-append
                        "\\override TextSpanner.bound-details.left.text = \"" text "\"  "))

                    (d-DirectivePut-chord-postfix tag "\\startTextSpan")
                    (d-DirectivePut-chord-override tag DENEMO_OVERRIDE_AFFIX)
                    (d-DirectivePut-chord-display tag  text)))))
    (d-SetSaved #f)))
;;;End of scheme script
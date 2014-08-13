;;;ScoreVersion
(let* ((tag "ScoreVersion")(current (d-DirectiveGet-score-data tag )))
  (if (not current)
      (set! current (_ "v 1.0")))
  (set! current (d-GetUserInput (_ "Version") (_ "Give a version for this edition:") current))
  (if current
    (begin 
        (d-DirectivePut-score-display tag current)
        (d-DirectivePut-score-data tag current)
        (case (RadioBoxMenu 
            (cons (_ "Version number only on first page") 'first)
            (cons (_ "Version number on every page") 'all)
            (cons (_ "Delete Version Numbering") 'delete))
         ((first)
            (d-DirectivePut-score-override tag  DENEMO_OVERRIDE_AFFIX)
            (d-DirectivePut-score-prefix tag  (string-append "\\markup \\teeny \"" current "\"")))
        ((all)
           (d-DirectivePut-score-override tag  DENEMO_OVERRIDE_AFFIX)
            (d-DirectivePut-score-prefix tag (string-append "\\paper {
            evenHeaderMarkup = \\markup \\fill-line {\\line{\\teeny {"
             current
             "}}\\line {
        \\on-the-fly \\print-page-number-check-first
        \\fromproperty #'page:page-number-string
      }}
            oddHeaderMarkup = \\markup \\fill-line {
            \\line {
        \\on-the-fly \\print-page-number-check-first
        \\fromproperty #'page:page-number-string
      }\\line{\\teeny {"
             current
             "}}}\n}")))
        ((delete)
            (d-DirectiveDelete-score tag)))         
        (d-SetSaved #f))
    (d-InfoDialog (_ "Cancelled"))))
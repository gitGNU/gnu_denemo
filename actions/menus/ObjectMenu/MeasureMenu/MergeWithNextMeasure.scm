(let ()
	(define position (d-GetHorizontalPosition))
	(define measure (d-GetMeasure))
	(if (d-MoveToMeasureRight)
		(let ()
		(define count -1)
		(d-PushClipboard)
		(d-UnsetMark)
		(d-SetMark)
		(GoToMeasureEnd)
		(d-SetPoint)
		(d-Cut)
		(d-DeleteMeasure)
		(d-GoToPosition #f #f  measure #f)
		(GoToMeasureEnd)
		(RepeatUntilFail (lambda () (set! count (1+ count)) (d-PutClipObj 0 count))) ; paste
		(d-GoToPosition #f #f measure position)
		(d-PopClipboard))))
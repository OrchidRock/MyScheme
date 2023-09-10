;:
;:
;:
;: constructor
(define (make-queue) (cons '() '()))
(define (front-ptr queue) (car queue))
(define (rear-ptr queue) (cdr queue))
(define (set-front-ptr! queue item) (set-car! queue item))
(define (set-rear-ptr! queue item) (set-cdr! queue item))

;:queue's operations

(define (empty-queue? queue) (null? (front-ptr queue)))
(define (front-queue queue)
    (if (empty-queue? queue)
        (error "FRONT called with an empty queue" queue)
        (car (front-ptr queue))))
(define (insert-queue! queue item)
    (let ((new-pair (cons item '())))
        (cond ((empty-queue? queue)
                    (set-front-ptr! queue new-pair)
                    (set-rear-ptr! queue new-pair)
                    queue)
              (else
                    (set-cdr! (rear-ptr queue) new-pair)
                    (set-rear-ptr! queue new-pair)
                    queue))))
(define (delete-queue! queue)
    (cond ((empty-queue? queue)
                (error "DELETE! called with empty queue" queue))
          (else (set-front-ptr! queue (cdr (front-ptr queue)))
                queue)))

;: exercise 3.21
(define (print-queue queue)
    (display (front-ptr queue))
    'ok)

;: exercise 3.22
(define (make-queue-2)
    (let ((front-ptr '())
          (rear-ptr '()))
        (define (dispatch m)
            (cond ((eq? m 'empty?) (null? front-ptr))
                  ((eq? m 'front) (if (null? front-ptr)
                                      (error "")
                                      (car front-ptr)))
                  ((eq? m 'insert!)
                    (lambda (item) (let ((new-pair (cons item '())))
                                        (cond ((null? front-ptr)
                                                (set! front-ptr new-pair)
                                                (set! rear-ptr new-pair)
                                                'ok)
                                              (else
                                                (set-cdr! rear-ptr new-pair)
                                                (set! rear-ptr new-pair)
                                                'ok)))))
                  ((eq? m 'delete!)
                    (if (null? front-ptr)
                        (error "")
                        (set! front-ptr (cdr front-ptr))))
                  (else (error ""))))
        dispatch))
;:
(define q1 (make-queue))
(define q2 (make-queue-2))

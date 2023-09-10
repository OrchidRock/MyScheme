;:
;: exercise 2.28
;: Write a procedure that takes as argument a tree and returns a list whose elements are all
;: the leaves of the tree arranged in left-to-right order.
;:

(define (fringe tree)
    (define (iter tree result)
        (cond ((null? tree) '())
              ((not (pair? tree)) (cons tree result))
              (else (append (iter (car tree) '()) 
                            (iter (cdr tree) '())))))
    (iter tree '()))


(define (fringe tree)
    (cond ((null? tree) '())
          ((not (pair? tree)) (list tree))
          (else (append (fringe (car tree))
                        (fringe (cdr tree))))))
;(define ())
;: Redefine fringe
;(define (fringe tree)
;   (map (lambda (sub-tree)
;          (if (pair? sub-tree)
;                (fringe sub-tree)
;                sub-tree))
;        tree))

;: test

(define x (list (list 1 2) (list 3 4)))
(define x2 (list 1 (list 2 (list 3 4)) 5))


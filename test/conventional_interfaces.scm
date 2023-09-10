;;
;: Sequences as Conventional Interfaces
;:


;; mapper
(define (map-s proc items)
    (if (null? items)
        '()
        (cons (proc (car items))
              (map-s proc (cdr items)))))

;; more general map
;;(define map map)


;: filter
;(define filter filter)
(define (filter predicate sequence)
    (cond ((null? sequence) '())
          ((predicate (car sequence)) (cons (car sequence)
                                            (filter predicate (cdr sequence))))
          (else (filter predicate (cdr sequence)))))

;: accumulater
(define (accumulate op initial sequence)
    (if (null? sequence)
        initial
        (op (car sequence) (accumulate op initial (cdr sequence)))))

;:
;: exercise 2.36
;: accumulate-n  similar to accumulate except that it takas as its third argument
;: a sequence of sequence, which are all assumed to hava the same number of elements.
;: It applies the designated accumulation procedure to combine all the first elements
;: of the sequence, all the second elements of the sequence, and so on, and returns a
;: sequence of the results.
;:

(define (select-car s)
    (if (null? s)
        '()
        (cons (car (car s))
              (select-car (cdr s)))))

(define (select-cdr s)
    (if (null? s)
        '()
        (cons (cdr (car s))
              (select-cdr (cdr s)))))

(define (accumulate-n op init seqs)

    (if (null? (car seqs))
        '()
        (cons (accumulate op init (select-car seqs))
              (accumulate-n op init (select-cdr seqs)))))


(define ss (list (list 1 2 3) (list 4 5 6) (list 7 8 9) (list 10 11 12)))

;: enumerater
(define (enumerate-interval low high)
    (if (> low high)
        '()
        (cons low (enumerate-interval (+ low 1) high))))


;: flatmap
(define (flatmap proc seq)
    (accumulate append '() (map-s proc seq)))



;: examples


(load "fringe.scm")
(define enumerate-tree fringe)

(define (sum-odd-squares tree)
    (accumulate +
                0
                (map-s square
                     (filter odd?
                             (enumerate-tree tree)))))

(define x (list 1 2 3 4 5))
(define x2 (list 1 (list 2 (list 3 4)) 5))
(define x3 (list (list 1 2 3) (list 2 3 4) (list 3 4 5)))
(define T1 (map-s square x))
(define T2 (filter odd? x))
(define T3 (accumulate + 0 x))
(define T4 (accumulate * 1 x))
(define T5 (accumulate cons '() x))
(define T6 (enumerate-interval 2 7))
(define T7 (enumerate-tree x2))

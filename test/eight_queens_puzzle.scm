;:
;: exercise 2.42
;: The "eight-quenns puzzle" asks how to place eight queens on a chessboard so that
;: no queen is in check from aany other (i.e., no two queens are in the same row, column, or diagonl.)
;:

(load "conventional_interfaces.scm")
(define (adjoin-position new-row k rest-of-queens)
    (append (list new-row) rest-of-queens))

(define (safe? k p)
    (define (iter new rest c)
        (cond ((null? rest) #t)
              ((= c k) #t)
              ((or (= (car rest) new)
                   (= (car rest) (- new c))
                   (= (car rest) (+ new c))) #f)
              (else (iter new (cdr rest) (+ c 1)))))
    (iter (car p) (cdr p) 1))

;: Test
(define safe-test (safe? 8 (list 6 4 1 5 8 2 7 3)))
(define safe-test2 (safe? 8 (list 6 1 4 5 8 2 7 3)))

(define empty-board (list '()))

(define (queens board-size)
    (define (queen-cols k)
        (if (= k 0)
            empty-board
            ( let ((result
            (filter (lambda (positions) (safe? k positions))
                    (flatmap (lambda (rest-of-queens)
                                     (map (lambda (new-row)
                                          (adjoin-position new-row k rest-of-queens))
                                          (enumerate-interval 1 board-size)))
                             (queen-cols (- k 1))))))
                             ;:(display result)
                             ;:(newline)
                             result)))
    (queen-cols board-size))

;: exercise 2.43
;: queen-cols is called board-size times in queens-slow when it is called one time in queens.
(define (queens-slow board-size)
    (define (queen-cols k)
        (if (= k 0)
            empty-board
            ( let ((result
            (filter (lambda (positions) (safe? k positions))
                    (flatmap (lambda (new-row)
                                     (map (lambda (rest-of-queens)
                                          (adjoin-position new-row k rest-of-queens))
                                          (queen-cols (- k 1))))
                             (enumerate-interval 1 board-size)))))
                             ;:(display result)
                             ;:(newline)
                             result)))
    (queen-cols board-size))
;: Test
(define T1 (queens 2))
(define T2 (queens 3))
(define T3 (queens 4))
(define T4 (queens 5))
(define T5 (queens 6))
(define T6 (queens 7))
(define T7 (queens 8))

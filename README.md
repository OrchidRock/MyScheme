# MyScheme
An experimental Scheme interpreter implemented using C++.

## Build & Run
`$ cmake -B build`

`$ cd build`

`$ make`

`$ ./MyScheme`

## Simple Example

```shell
> Welcome to MyScheme. Use Ctrl-C to exit.
> 123
123
> -123
-123
> "sdf"
"sdf"
> #t
#t
> #f
#f
> #\a
#\a
> abc
abc
> (1 . 2)
(1 . 2)
> (1 . (2 . 3))
(1 2 . 3)
> (1 2)
(1 2)
> (1 (2 3))
(1 2 3)

```

## complex emaple

```shell
> (define count
        ((lambda (total)
            (lambda (increment)
                (set! total (+ total increment))
                total))
        0))
ok
> (count 3)
3
> (count 5)
5
>(define (factorial n)
    (define (iter product counter max-count)
      (if (> counter max-count)
          product
          (iter (* counter product)
                (+ counter 1)
                max-count)))
    (iter 1 1 n))
ok
> (factorial 4)
24
> (define Y
    (lambda (f)
      ((lambda (x) (f (lambda (y) ((x x) y))))
       (lambda (x) (f (lambda (y) ((x x) y)))))))
ok
> (define factorial
    (Y (lambda (fact) 
         (lambda (n) 
           (if (= n 0)
               1
               (* n (fact (- n 1))))))))
ok
> (factorial 5)
120
```

## test
`$ cd test`

`$ ../build/MyScheme`

```shell
> Welcome to MyScheme. Use Ctrl-C to exit.
> (load "stdlib.scm") ; library 
ok
> (load "conventional_interfaces.scm") ; library
ok
> x
(1 2 3 4 5)
> x2
(1 (2 (3 4)) 5)
> x3
((1 2 3) (2 3 4) (3 4 5))
> T1
(1 4 9 16 25)
> T2
(1 3 5)
> T3
15
> T4
120
> T5
(1 2 3 4 5)
> T6
(2 3 4 5 6 7)
> T7
(1 2 3 4 5)
> (load "eight_queens_puzzle.scm") ; eight queens puzzle problem.
ok
> T7
```

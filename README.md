# MyScheme
An experimental Scheme compiler implemented  using C++.

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
> (load "stdlib.scm")

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

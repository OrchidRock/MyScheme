# MyScheme
An experimental Scheme compiler implemented  using C++.

## Build & Run
`$ cmake -B build`

`$ cd build`

`$ make`

`$ ./MyScheme`

## Example

"""shell
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

"""

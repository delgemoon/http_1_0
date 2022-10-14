# Simple Http server

## BUILD:

```shell
mkdir build
cd build
cmake ..
make 
```

## Sample Run


```shell
cd build
./dtl 5555 101 false ../doc/
```

## params;

5555: port
101: number of thread
false: detach or not
../doc/: root html directory


## Features

1. Support HTTP GET/1.0 Method
2. 700 concurrent connection per second

## NOTE: you might want to change

```shell
ulimit -n 10000
```
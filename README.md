# MDICT CPP

mdict *.mdx/*.mdd interpreter c++ implements

## Clone the project

```
git clone git@github.com:dictlab/mdict-cpp.git
git submodule init
git submodule update --recursive # pull the gtest library
```

## Executable

### Cmake

```
mkdir build
cd build && cmake .. && make mydict
target/bin/mydict your_mdx_file.mdx yourword
```
the executable binary will generate at `bin/mdict`


## Library

### Library

```
mkdir build 
cd buid && cmake .. && make mdict
ls target/lib/libmdict.a
```


## format
ref: [mdx format](https://www.zhihu.com/question/22143768)

![mdx_format](https://tva1.sinaimg.cn/large/008eGmZEly1go066lnewfj30u01bdb29.jpg)

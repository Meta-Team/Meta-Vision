Meta-Vision
===========

编译步骤
--------

1. `git clone` 代码
2. `cd Meta-Vision`
3. `git submodule update` 如果不行可能？（大雾）可以尝试一下`git submodule update --init --recursive`
4. `mkdir build`
5. `cd build`
6. `cmake ..`
7. `make Vision`或者其他target

8. 运行：
--------
8. `./Vision ../meta.yml`

或者

8.1 需要修改下 `run.sh` 中 `cd` 的目录到当前 `Meta-Vision` 中的 `build` 目录

8.2 （大雾）把 `run.sh` 放在 `build` 目录中竟然能够执行？

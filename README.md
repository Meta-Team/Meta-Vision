Meta-Vision
===========

see myHeader.hpp for new vision code
see Demo.jpg for a demo output of the vision part
change BLUE_OFFSET and RED_OFFSET
with FLAG DEBUG, you see three pop-up windows, frame.jpg for the raw input image, thredshold.jpg for bitmap that denote possible area that may be the LED light bar, detect.jpg for visualization of possible armor target.

编译步骤
--------

1. `git clone` 代码
2. `cd Meta-Vision`
3. `mkdir build`
4. `cd build`
5. `cmake ..`
6. `make`
7. 运行 `./robomaster_meta_vision ../meta.yml`
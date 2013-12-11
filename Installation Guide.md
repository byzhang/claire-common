# Installation Guide

claire-common is a collection of utility for build modern c++ application. 

It is designed for used in x86-64 Linux. Beacause claire-common use some new kernel api(timerfd, eventfd...), kernel version require at least more than 2.6.32.

Currently claire-common use [CMake][1] build the library, require at least 2.8 version.

It depends serveral library:
 
- [boost][2]: claire-common use boost everywhere. The new, the better.

- [gflags][3]: claire-common use gflags to configure it, such as logging module. 

- [gtest][4]: claire-common unittest use it. see more informatio [here][5]

- [protobuf][6]: claire-common include a moudule to support json <--> protobuf conversion, so need it, please use version bigger than 2.5.

- [rapidjson][7]:  As before, we use rapidjson to support json <--> protobuf conversion.

- libdwarf: Symbolizer module need it，some distribution already install it. 

After install all before, run ./build.sh and ./build.sh install.

Done.

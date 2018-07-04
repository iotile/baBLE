# baBLE - Linux bridge

This is the baBLE bridge for Linux, written in C++.

## Building

### Manually

```bash
# Install tools
$ sudo apt install cmake make g++

$ cd platforms/linux
$ mkdir build && cd build

# Use cmake to generate Makefiles
$ cmake -G "Unix Makefiles" ..

# Compile using make (replace <num_jobs> by the number of CPU cores you have to speed up the compilation, else just use make)
$ make -j <num_jobs>
```

**Important**: you'll need `cmake>=3.5`, and `g++>=4.9`.

To build a release, simply add `-DCMAKE_BUILD_TYPE=Release ` to the cmake command.

### Using the Docker image

You can use the Docker image [here](https://github.com/iotile/baBLE/tree/master/tools/cpp-compilation) to have a pre-configured
Ubuntu 16.04 with all the tools you need to build the Linux bridge, for all supported architectures
(x86_64 natively, i686 and armv7l thanks to cross-compilation). Then you just have to use `cmake` and `make` like before.

# baBLE

[![Build Status](https://travis-ci.com/iotile/baBLE.svg?branch=master)](https://travis-ci.com/iotile/baBLE)

baBLE is a universal translator from native Bluetooth to a high-level interface,
which can be used by any language, on any platform.

The goal is to (finally) make it simple to use Bluetooth Low Energy, with your favorite language, without worrying about
cross-platform issues on Linux, Mac or Windows.

To do this, baBLE is composed of two parts:

 - **bridges**: written in OS favorite language, a bridge is dependent of the OS, using its API to interact with the
 native Bluetooth system at a low-level. Its goal is to provide a high-level, *universal interface* (formatted with
 [flatbuffers](https://github.com/google/flatbuffers)) through its standard input/output.
 - **interfaces**: written in any language, an interface simply wraps the baBLE commands into functions, to communicate
 with the bridges in a transparent way. *Interfaces are platform-independent.*

![baBLE general schema](./doc/baBLE.jpg)
*<p align="center">Schema of baBLE principle</p>*

## Progress

The project is still in development.

#### Bridges

|        Platform       |  Status  |
|-----------------------|----------|
|   Linux (using Bluez) |    👍    |
|   Mac                 |    👎    |
|   Windows             |    👎    |

#### Interfaces

|        Language       |  Status  |
|-----------------------|----------|
|   Python              |    👍    |

Feel free to create a wrapper for your favorite language.


### License

This project is distributed under the terms of the [MIT license](./LICENSE).

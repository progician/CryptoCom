# CryptoCom

Library and application of Private Set Intersection algorithms. These primitives can be used for distributed computing.

## Getting started

The following section is set you up with building and running the software.

### Prerequisites 

You will need CMake 3.15+ and optionally conan 1.20 (for installing dependencies) installed for your platform, as well as CMake supported C++ development toolchain with C++14 capable compiler. The development work was done on MacOS and Linux and so the Windows, for the moment is not supported.

### Build and test

The build system follows idiomatic CMake practices. First you need to configure the project:

```
$ cmake -Bbuild .
```

On Linux and MacOS this will produce a Makefile build script. However I do strongly recommend for all C++ developers out to use Ninja instead.

```
$ cmake -GNinja -Bbuild .
```

Once configured, one can cut to the chase: build and install it.

```
$ cmake --build build && cmake --build build --target install
```


But the whole point of this project is to demonstrate Continuous Integration and Delivery practices and I expect developers would be interested how to do testing. The following is advised:

```
$ cmake --build build --target check-unit
$ cmake --build build --target check-functional
$ cmake --build build --target check-perf
$ cmake --build build --target install
```

This are the same stages the Jenkinsfile declares for the CI pipeline.


## Authors

* **Gyula Gubacsi** - *author* <gyula.gubacsi@gmail.com>
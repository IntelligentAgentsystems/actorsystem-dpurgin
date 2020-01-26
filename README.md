# actorsystem-dpurgin
actorsystem-dpurgin created by GitHub Classroom

## Requirements 

1. C++ Actor Framework 0.18 (https://www.actor-framework.org/)
   * You can use the prebuilt CAF for Windows for MSVC 2017 from https://dpurgin.github.io
1. If you intend to use the prebuilt CAF for Windows for MSVC 2017: 
   * Install MS Visual Studio 2017 with C++ desktop development workload
   * Install [open-source Qt Framework](https://www.qt.io/download-qt-installer) version 5.12.x for MSVC 2017 (choose LTS and archive versions in the online installer)
2. If you are on Linux or intend to build CAF yourself:
   * Install any version of [open-source Qt Framework](https://www.qt.io/download-qt-installer), use the online installer even on Linux (don't use the OS's package manager, the Qt builds there are almost always outdated)
   * C++17-compliant compiler required
3. Open Qt Creator installed with the Qt framework
4. Open `project.qbs` in Qt Creator as project
5. If on Windows, you might need to set paths to the CAF build in lines 51-55 of `project.qbs`

## Building CAF on Windows

cmake (http://cmake.org) and Visual Studio 2017 (at least) required. 

Dynamic libraries are not supported on Windows.

Passing `-DCMAKE_INSTALL_PREFIX=<path>` to cmake would change the default installation path of the libraries and
header files (standard path would be `C:\Program Files (x86)\caf`, and it requires privilege elevation). 

By default, a 32-bit version will be built. A 64-bit build can be obtained by passing `-G"Visual Studio 15 2017 Win64"` to cmake. 
Note that in order to use CAF with Qt, the 64-bit build is required.

Unless you have OpenSSL SDK you should also pass `-DCAF_NO_OPENSSL=yes` to cmake.

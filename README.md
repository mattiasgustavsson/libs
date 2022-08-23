
# libs
Single-file public domain libraries for C/C++ (dual licensed under MIT).

* [app.h](docs/app.md) - Small cross-platform base framework for graphical apps, for C/C++.
* [assetsys.h](docs/assetsys.md) - File system abstraction to read from zip-files, for C/C++.
* [hashtable.h](docs/hashtable.md) - Cache efficient hash table implementation for C/C++.
* [http.h](docs/http.md) - Basic HTTP protocol implementation over sockets (no https).
* [ini.h](docs/ini.md) - Simple ini-file reader for C/C++.
* [rnd.h](docs/rnd.md) - Pseudo-random number generators for C/C++.
* [strpool.h](docs/strpool.md) - Highly efficient string pool for C/C++.
* [thread.h](docs/thread.md) - Cross platform threading functions for C/C++.

# Installing mgnlibs(vcpkg)

You can download and install mgnlibs using the [vcpkg](https://github.com/Microsoft/vcpkg) dependency manager:

git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
vcpkg install mgnlibs

The mgnlibs port in vcpkg is kept up to date by Microsoft team members and community contributors. If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.

![build](https://github.com/mattiasgustavsson/libs/actions/workflows/main.yml/badge.svg)

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


# wip libs
More libs, work-in-progress, some are unfinished, some are complete but lacking documentation.

* array.h - Dynamic array library for C/C++.
* buffer.h - Memory buffer with read/write operations, for C/C++.
* crtemu.h - Cathode ray tube emulation shader for C/C++.
* cstr.h - String interning and manipulation library for C/C++.
* dialog.h - Loading and management of dialogs for a custom game dialog system.
* dir.h - Directory listing functions for C/C++.
* frametimer.h - Framerate timer functionality, for C/C++.
* id3tag.h - Read/write ID3 tags from/to mp3 files in C/C++.
* img.h - Image processing functions for C/C++.
* mus.h - Parsing library for MUS music files (as used in DOS games).
* paldither.h - Convert true-color image to custom palette, with dither.
* palettize.h - Median-cut palette generation and remapping for C/C++.
* palrle.h - Run-length encoding of palettized bitmaps, for C/C++.
* pixelfont.h - Custom pixel font format builder and renderer.
* sysfont.h - Simple debug text renderer for C/C++.
* testfw.h - Basic test framework for C/C++.


# repackaged libs
Single-file header-only versions of libs written by other people, released under the same license as the original lib.
I recommend using the latest version of these libs - I only repackage them like this to fit my single-file-libs centered
dev paradigm, and if you don't absolutely need that, you are better off using the original multi-file versions.

* ftplib.h - FTP client lib for C/C++. *By Thomas Pfau.*
* hoedown.h - Markdown to HTML renderer for C/C++. *By Porte/Marti/Mendez/Torres.*
* libxdiff.h - File Differential Library. *By Davide Libenzi.*
* lzma.h - LZMA data compression/decompression library. *By Igor Pavlov.*
* opl.h - OPL3/SB16 emulation with MIDI interface. *Based on code by Aaron Giles and Mateusz Viste* 
* samplerate.h - Sample-rate converter (libsamplerate) for C/C++. *By Erik de Castro Lopo*
* speech.h - Basic text-to-speech synthesizer for C/C++. *By Jari Komppa / Nick Ing-Simmons (et al)*

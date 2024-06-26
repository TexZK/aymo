# AYMO - Accelerated YaMaha Operator


## Work In Progress

**This package is still at a very early stage of development.**

I would mark it as *pre-alpha* right now.


## Overview

The main target of *AYMO* is to provide a software library to accelerate emulation of old sound chips, in particular those of the '80s and '90s, like the mighty *Yamaha OPL3*.

Software emulation of multi-channel sound chips can have a rather heavy impact on the CPU, especially when programmed with a general purpose programming language, despite nice heavy optimizations by the compiler.
Most modern CPUs provide some interesting *parallelization* opportunities via their special *SIMD* operators, such as those of *x86 SSE/AVX* and *ARM NEON*.
This comes handy because sound operations are often parallel in nature, using the same subset of operations and information flows.

The *AYMO* project tries to take advantage of these *SIMD* operations to reduce CPU time of the emulation, hopefully by some meaningful margin.
This allows reuse of cheap or old *embedded hardware* platforms to create fun toys like arcade cabinet emulators, creative synthsizers, or simple music jukeboxes, replicating the sound of old hardware.


## Building


### Requirements

* The `meson` build system + `ninja` (default) or `make`.
* A modern *C99* compiler, such as `gcc` or `clang` (multi-platform) or `msvc` (for *Windows* platforms), with support for the target *SIMD intrinsics*.
* A compatible target machine; currently `x86` (`SSE`-`AVX2`) or `ARM` (`NEON`).
* This repository uses *git submodules*. To initialize them:

```sh
cd PATH_TO_PROJECT_ROOT
git submodule init
git submodule update
```


#### Windows

The simplest generic way to compile under *Windows* is via a *MSYS2* environment:

1. [Install *MSYS2*](https://www.msys2.org/wiki/MSYS2-installation/).
2. Run the *MSYS2 MSYS* application from *Windows Start*.
3. Install common packages:

```sh
pacman -S meson ninja gcc
```

The *MSYS2 MSYS* should be used for any further command line interactions.

Alternatively, install [*Visual Studio Community*](https://visualstudio.microsoft.com/vs/community/) (currently *VS 2022*).
This requires some dedicated setup (see below).


### Setup

A generic setup for the host machine itself is very simple with *Meson*:

```sh
cd PATH_TO_PROJECT_ROOT
meson setup builddir
```


#### Visual Studio Community

From the *Windows Start*, launch *x86_x64 Cross Tools Compiler for VS 2022* (or similar).

Project setup for *Visual Studio Community* requires defining `vs` as the *back-end*:

```sh
cd PATH_TO_PROJECT_ROOT
meson setup builddir --backend vs
```

This generates the `aymo.sln` *Visual Studio Solution* under the `builddir` folder.


### Compiling

The `meson compile` command can auto-detect the appropriate build system (although *Ninja* is the default):

```sh
cd PATH_TO_PROJECT_ROOT/builddir
meson compile
```


## Testing

A basic test suite is run via the `meson test` command:

```sh
cd PATH_TO_PROJECT_ROOT/builddir
meson test
```

These tests usually compare the implementations of *AYMO* with those of other existing libraries. The latter are considered *ground thruth*, so any deviations from their outputs might be marked as errors.


## Benchmarking

A basic benchmark suite is run via the following commands:

```sh
cd PATH_TO_PROJECT_ROOT/builddir
meson test --benchmark
meson compile benchmark-report-tda8425
meson compile benchmark-report-ym7128
meson compile benchmark-report-ymf262
```


### Benchmark Results

Some preliminary benchmarks were run against some very different CPUs:

| System | OS | CPU | SIMD | Notes
|:-|:-|:-|:-|:-|
| PC | Windows 10 | i7 6700k | x86 SSE4.1 + AVX2 | 2016 gaming PC |
| BeagleBone Black | Debian 11 | ARM Cortex-A8 | ARMv7 NEON | Headless |
| Raspberry Pi 5 | Debian 12 | ARM Cortex-A76 | ARMv7 NEON | Headless + Heatsink Fan |

All the systems were updated to their latest software and OS releases.
The compiler was *GCC* for all these machines.

All the systems run `--cpu-ext dummy`, which mimics the overhead of the test harness itself (mostly the score decoder), to subtract it from the actual benchmarks.

All the benchmarks results are normalized against the plain *C* implementation, run as `--cpu-ext none`.


#### TDA8425

A basic *TDA8425* can be emulated with simple DSP techniques (mostly IIR filters), so the implementation can be rather straightforward.

Surprisingly, the *BBB* shows a much higher speedup compared to the other SIMD I tested.
Perhaps the plain C implementation cannot be optimized by the CPU core itself, as done with higher grade CPUs.
This somehow shows the benefits of *AYMO* for older embedded systems.

![Benchmark Results](./doc/benchmarks-tda8425.png)


#### YM7128

The *YM7128* is a simple fixed-point delay unit, with lots of parallel computations.
The results are indeed very interesting for all the SIMD architectures under test, consistently showing some nice speedup.

![Benchmark Results](./doc/benchmarks-ym7128.png)


#### YMF262

The reference *OPL3* implementation is *NukedOPL3*.

All the *OPL3* scores were played via `aymo_ymf262_play --benchmark --loops 3`, except for the *BBB* which did not loop (too slow!).

![YMF262 Benchmark Results](./doc/benchmarks-ymf262.png)


## Integration

Currently *AYMO* should compile as both for *static* and *dynamic* library natures.

The *static* library is already used by the standalone executables of this repository, like *apps* and *unit tests*.

The *dynamic* library is still untested.


## License

AYMO - Accelerated YaMaha Operator.
Copyright (c) 2023-2024 Andrea Zoppi.

AYMO is free software: you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 2.1 of the License, or (at your option)
any later version.

AYMO is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with AYMO. If not, see <https://www.gnu.org/licenses/>.

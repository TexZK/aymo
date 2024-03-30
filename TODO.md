# TO-DO List

Here are some ideas that came to my mind.

* Make as _shared library_.
    * *AYMO_API* for *__stdcall*.  &rarr;  **DROPPED:** no need for *__stdcall*!
    * *AYMO_PUBLIC* for *declspec* / *attribute*.  &rarr;  **DONE!**

* Add score replayer.
    * _IMF_ score format.  &rarr;  **DONE!**
    * _DRO_ score format.  &rarr;  **DONE!**
    * _RAW_ score format.  &rarr;  **DONE!**
    * _VGM_ score format.
    * _Benchmark_ mode, to be integrated with _Meson_.  &rarr; currently only via `aymo_ymf262_play`

* Add _YMF262_.
    * All CPU architectures.
        * _x86 SSE4.1_  &rarr;  **DONE!**
        * _x86 AVX_ (_VEX_ encoded _SSE4.1_)
        * _x86 AVX2_  &rarr;  **DONE!**
        * _ARM NEON_  &rarr;  **TBV: RPi5 & BBB**
    * 4-slot operators.  &rarr;  **TBV**
    * rhythm mode.  &rarr;  **TBV**

* Add most of the possible *YMF262* static operator configurations for unit tests.
    * Single-note scores.
    * 4-slot operators.
    * Rhythm mode.
    * Leaving complex and dynamic parameters to actual music scores.

* Make _YMF262_ queue delay dynamically configurable.
    * Shrinking requires phase counter clamping.

* Take queuing away from _YMF262_ instances.
    * Make as CPU-independent utility class.
    * Make default length 64.
    * Make length and pointers configurable.
    * Queue status accessors.

* Allow _YMF262_ output multiple formats.
    * _int16_ / _float_.  &rarr;  **DONE!**
    * 2 / 4 channels.  &rarr;  **DONE!**
    * _planar_ / _interleaved_ buffers.  &rarr;  **DROPPED:** _planar_ converted externally.

* Add simple linear resampler.  &rarr;  **DEFERRED:** use existing resampler libraries
    * Stereo.
    * _int16_ / _float_.
    * Fixed 2x/4x/8x up-samplers, with dedicated FIR windows.
    * Piece-wise down-sampling to custom rates.

* Add _YM7128B_.
    * All CPU architectures.
        * _x86 SSE4.1_  &rarr;  **DONE!**
        * _x86 AVX_
        * _x86 AVX2_
        * _ARM NEON_  &rarr;  **TBV: RPi5 & BBB**
    * Stick to the closest fixed point models.  &rarr;  **DONE!**
    * C++ wrappers.

* Add _TDA8425_.
    * All CPU architectures.
        * _x86 SSE4.1_  &rarr;  **DONE!**
        * _x86 AVX_
        * _x86 AVX2_  &rarr;  **DONE!**
        * _ARM NEON_  &rarr;  **TBV: RPi5 & BBB**
    * _float32_ model.  &rarr;  **DONE!**
    * _int16_ mode.
    * C++ wrappers.

* Add _YMF262_ superset.
    * Rough emulation of similar _Yamaha operators_.
    * For accelerated music plugins (not for actual emulation).
    * C++ wrappers.

* Add _YM3812_ subset.
    * Remove unecessary features from _YMF262_.
    * Interleave *2-op* code to benefit from multiple dispatch SIMD.
    * Add [*Composite Sine Mode*](https://retrocomputing.stackexchange.com/a/23751).

* Add built-in plugin APIs to all emulated models.
    * [_LADSPA_](https://www.ladspa.org/)
    * [_FST_](https://git.iem.at/zmoelnig/FST) ?
    * [_LV2_](http://lv2plug.in/)

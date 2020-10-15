# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set (PROJECT_NAME zipper)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

set (minizip minizip/minizip.c minizip/mz_compat.c minizip/mz_crypt.c minizip/mz_crypt_brg.c minizip/mz_os.c minizip/mz_os_posix.c minizip/mz_strm.c minizip/mz_strm_buf.c minizip/mz_strm_bzip.c minizip/mz_strm_lzma.c minizip/mz_strm_mem.c minizip/mz_strm_os_posix.c minizip/mz_strm_pkcrypt.c minizip/mz_strm_split.c minizip/mz_strm_wzaes.c minizip/mz_strm_zlib.c minizip/mz_zip.c minizip/mz_zip_rw.c)
set (brg minizip/lib/brg/aescrypt.c minizip/lib/brg/aeskey.c minizip/lib/brg/aestab.c minizip/lib/brg/hmac.c minizip/lib/brg/sha1.c minizip/lib/brg/sha2.c)
set (bzip2 minizip/lib/bzip2/blocksort.c minizip/lib/bzip2/bzlib.c minizip/lib/bzip2/compress.c minizip/lib/bzip2/crctable.c minizip/lib/bzip2/decompress.c minizip/lib/bzip2/huffman.c minizip/lib/bzip2/randtable.c)
set (liblzma minizip/lib/liblzma/check/check.c minizip/lib/liblzma/check/crc32_fast.c minizip/lib/liblzma/check/crc32_table.c
        minizip/lib/liblzma/common/alone_decoder.c minizip/lib/liblzma/common/alone_encoder.c minizip/lib/liblzma/common/common.c minizip/lib/liblzma/common/filter_encoder.c
        minizip/lib/liblzma/lz/lz_decoder.c minizip/lib/liblzma/lz/lz_encoder.c minizip/lib/liblzma/lz/lz_encoder_mf.c
        minizip/lib/liblzma/lzma/fastpos_table.c minizip/lib/liblzma/lzma/lzma_decoder.c minizip/lib/liblzma/lzma/lzma_encoder.c minizip/lib/liblzma/lzma/lzma_encoder_optimum_fast.c minizip/lib/liblzma/lzma/lzma_encoder_optimum_normal.c minizip/lib/liblzma/lzma/lzma_encoder_presets.c
        minizip/lib/liblzma/rangecoder/price_table.c)

add_library( # Sets the name of the library.
        ${PROJECT_NAME}

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).

        ${minizip}
        ${brg}
        ${bzip2}
        ${liblzma}

        zipper.cpp)

include_directories(
        src/main/cpp
        src/main/cpp/minizip/
        src/main/cpp/minizip/lib/brg/
        src/main/cpp/minizip/lib/bzip2/
        src/main/cpp/minizip/lib/liblzma/api/
        src/main/cpp/minizip/lib/liblzma/api/lzma/
        src/main/cpp/minizip/lib/liblzma/check/
        src/main/cpp/minizip/lib/liblzma/common/
        src/main/cpp/minizip/lib/liblzma/lz/
        src/main/cpp/minizip/lib/liblzma/lzma/
        src/main/cpp/minizip/lib/liblzma/rangecoder/
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log
        android)
find_package(ZLIB REQUIRED)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        ${PROJECT_NAME}

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${ZLIB_LIBRARIES}
        android)
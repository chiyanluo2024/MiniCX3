This repository contains a set of software that I tested with while working on the Boost Spirit X3 library.  It mainly serves as an example for a few lightly-documented language features (e.g. keywords, skippers).  All comments and suggestions are welcome.

As of Apr 2 2025 I followed the following steps to complete a successfull cross-compilation of this library on an Ubuntu environment for windows.  These steps as as follows:

1) Install Clang and LLVM tools:
sudo apt install clang lld clang-tools

2) Install XWin:
cargo install xwin --locked

3) Use XWin to install a version of Windows SDK on Linux:
export WINSDK_PATH="$HOME/winsdk" && xwin --accept-license splat --preserve-ms-arch-notation --output "$WINSDK_PATH"

4) Edit CMakeLists.txt as well as CMake scripts to allow for clang cross compiling.  Notes: (a) I had to set gtest_disable_pthreads to be ON in order to compile googletest.  (b) Many of the CMake commands seem to stop working for cross-compiling and I had to use a long CMAKE_CXX_FLAGS.  (c) Some of the vectorization intrinsics symbols seem to be missing and I had to suppress the linker errors with "-Xlinker /force" flags.


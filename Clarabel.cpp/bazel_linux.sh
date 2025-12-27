#!/bin/bash
if [ $# > 0 ] && [ "$1" == "noavx" ]; then
	if [ $# > 1 ] && [ "$2" == "RHEL8" ]; then
		echo 'avx = off, c++2a !'
		../bazel-6.5.0-linux-x86_64 build --compilation_mode=opt --cxxopt="-std=c++2a" --linkopt="-lstdc++fs" --cxxopt="-fvisibility=hidden" --cxxopt="-Wno-ignored-attributes" --cxxopt="-march=native" --cxxopt="-fPIC" --cpu=k8 --sandbox_base=/run/shm/ --local_ram_resources=HOST_RAM*0.3 --//Script:avx="off" --//Model:avx="off" --//Tests:avx="off" --experimental_cc_shared_library //Script //Model //CsCML //Tests:CMLTest //PyCML
	else
		echo 'avx = off, c++20 !'
		../bazel-6.5.0-linux-x86_64 build --compilation_mode=opt --cxxopt="-std=c++20" --cxxopt="-fvisibility=hidden" --cxxopt="-Wno-ignored-attributes" --cxxopt="-march=native" --cxxopt="-fPIC" --cpu=k8 --sandbox_base=/run/shm/ --local_ram_resources=HOST_RAM*0.3 --//Script:avx="off" --//Model:avx="off" --//Tests:avx="off" --experimental_cc_shared_library //Script //Model //CsCML //Tests:CMLTest //PyCML
	fi
else
	if [ $# > 1 ] && [ "$2" == "RHEL8" ]; then
		echo 'avx = on, c++2a !'
		../bazel-6.5.0-linux-x86_64 build --compilation_mode=opt --cxxopt="-std=c++2a" --linkopt="-lstdc++fs" --cxxopt="-fvisibility=hidden" --cxxopt="-Wno-ignored-attributes" --cxxopt="-march=native" --cxxopt="-fPIC" --cpu=k8 --sandbox_base=/run/shm/ --local_ram_resources=HOST_RAM*0.3 --//Script:avx="off" --//Model:avx="off" --//Tests:avx="off" --experimental_cc_shared_library //Script //Model //CsCML //Tests:CMLTest //PyCML
	else
		echo 'avx = on, c++20 !'
		../bazel-6.5.0-linux-x86_64 build --compilation_mode=opt --cxxopt="-std=c++20" --cxxopt="-fvisibility=hidden" --cxxopt="-Wno-ignored-attributes" --cxxopt="-march=haswell" --cxxopt="-mfma" --cxxopt="-ftree-vectorize" --cxxopt="-fPIC" --cpu=k8 --sandbox_base=/run/shm/ --local_ram_resources=HOST_RAM*0.3 --//Script:avx="on" --//Model:avx="on" --//Tests:avx="on" --experimental_cc_shared_library //Script //Model //CsCML //Tests:CMLTest //PyCML
	fi
fi
./bazel-bin/Tests/CMLTest
./buildCS_linux.sh bazel
./testPython_linux.sh bazel

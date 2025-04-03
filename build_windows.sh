#!/bin/bash
cmake -DCMAKE_BUILD_TYPE=Release -S . -B windows_build && cmake --build windows_build --config Release && windows_build/ScriptTest/Release/ScriptTest.exe

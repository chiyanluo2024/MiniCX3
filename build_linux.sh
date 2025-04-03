#!/bin/bash
cmake -DCMAKE_BUILD_TYPE=Release -S . -B linux_build && cmake --build linux_build --config Release && linux_build/ScriptTest/ScriptTest

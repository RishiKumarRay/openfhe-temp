#!/bin/bash

# Example Usage:
#   `cd` into src directory
#   run this script from there, ./../tools/scripts/apply-code-format
# =====================================================================================================================

find ./ -type f -and \( -name '*.cpp' -or -name '*.c' -or -name '*.h' \) | xargs clang-format -i --assume-filename=.clang-format

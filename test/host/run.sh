#!/bin/sh
set -eu

repo_dir=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
out_dir=$(mktemp -d)
trap 'rm -rf "$out_dir"' EXIT

c++ -std=c++11 -Wall -Wextra \
  -I"$repo_dir/test/host" -I"$repo_dir/src" \
  "$repo_dir/test/host/test_movement.cpp" \
  "$repo_dir/src/RoombaMovement.cpp" \
  "$repo_dir/src/RoombaActuators.cpp" \
  -o "$out_dir/test_movement"

"$out_dir/test_movement"
echo "host protocol tests passed"

#!/bin/sh
set -e

DRAGONRUBY_DIR=${DRAGONRUBY_DIR:-$HOME/DR}

echo "Compiling DragonRuby extension (Lua, no JIT)"
clang -isystem $DRAGONRUBY_DIR/include -fPIC -shared src/dr.c src/lua.c liblua.a -o lua.so
echo "Done"


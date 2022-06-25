#!/bin/sh
set -e

DRAGONRUBY_DIR=${DRAGONRUBY_DIR:-$HOME/DR}

echo "Compiling DragonRuby extension (LuaJIT)"
clang -isystem $DRAGONRUBY_DIR/include -I./include -fPIC -shared -DUSE_LUAJIT src/dr.c src/lua.c libluajit.a -o luajit.so
echo "Done"


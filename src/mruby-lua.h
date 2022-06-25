#include <mruby.h>
#include <mruby/class.h>
#include <mruby/variable.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/data.h>
#include <mruby/variable.h>
#ifdef USE_LUAJIT
#include <luajit/lua.h>
#include <luajit/lauxlib.h>
#else
#include <lua5.2/lua.h>
#include <lua5.2/lauxlib.h>
#endif

typedef struct {
  lua_State* L;
  int closed;
} mrb_lua_data;

void mrb_lua_final(mrb_state* mrb, void* p);
mrb_value mrb_lua_init(mrb_state* mrb, mrb_value self);
mrb_value lua_to_mrb(mrb_state* mrb, lua_State* L, int index);
void mrb_to_lua(mrb_state* mrb, lua_State* L, mrb_value val);
void mrb_lua_raise_closed(mrb_state* mrb);
mrb_value mrb_lua_dostring(mrb_state* mrb, mrb_value self);
mrb_value mrb_lua_dofile(mrb_state* mrb, mrb_value self);
mrb_value mrb_lua_getglobal(mrb_state* mrb, mrb_value self);
mrb_value mrb_lua_setglobal(mrb_state* mrb, mrb_value self);
mrb_value mrb_lua_callglobal(mrb_state* mrb, mrb_value self);
mrb_value mrb_lua_close(mrb_state* mrb, mrb_value self);
mrb_value mrb_lua_closed(mrb_state* mrb, mrb_value self);
void mrb_mruby_lua_gem_init(mrb_state* mrb);
void mrb_mruby_lua_gem_final(mrb_state* mrb);

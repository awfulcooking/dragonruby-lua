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

void luaL_openlibs(lua_State*);

void mrb_lua_final(mrb_state* mrb, void* p)
{
  lua_State* L = p;
  lua_close(L);
  // mrb_free(mrb, p);
}

static struct mrb_data_type class_lua_type = { "Lua", mrb_lua_final };

mrb_value mrb_lua_init(mrb_state* mrb, mrb_value self)
{
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);
  DATA_PTR(self) = L;
  DATA_TYPE(self) = &class_lua_type;
  return self;
}

mrb_value lua_to_mrb(mrb_state* mrb, lua_State* L, int index)
{
  mrb_value result;
  switch (lua_type(L, index)) {
    case LUA_TNIL:
      result = mrb_nil_value();
      break;
    case LUA_TNUMBER: {
        lua_Integer n = lua_tointeger(L, index);
        lua_Number  f = lua_tonumber(L, index);
        if (n != f) {
          result = mrb_float_value(mrb, f);
        }
        else {
          result = mrb_fixnum_value(n);
        }
      }
      break;
    case LUA_TBOOLEAN:
      result = lua_toboolean(L, index) ? mrb_true_value() : mrb_false_value(); 
      break;
    case LUA_TSTRING:
      result = mrb_str_new_cstr(mrb, lua_tostring(L, index));
      break;
    case LUA_TFUNCTION:
      if (lua_iscfunction(L, index)) {
        lua_CFunction cf = lua_tocfunction(L, index);
        result = mrb_cptr_value(mrb, cf);
      }
      else if (lua_isfunction(L, index)) {
        mrb_raise(mrb, E_NOTIMP_ERROR, "Not implemented.");
      }
      else {
        mrb_raise(mrb, E_NOTIMP_ERROR, "Not implemented.");
      }
      break;
    case LUA_TTABLE: ;
      result = mrb_hash_new(mrb);
      index = lua_gettop(L);
      lua_pushnil(L);
      while (lua_next(L, index) != 0) {
        mrb_value key = lua_to_mrb(mrb, L, -2);
        mrb_value val = lua_to_mrb(mrb, L, -1);
        mrb_hash_set(mrb, result, key, val);
        lua_pop(L, 1);
      }
      break;
    case LUA_TUSERDATA:
    case LUA_TLIGHTUSERDATA:
    case LUA_TTHREAD:
    default:
      mrb_raise(mrb, E_NOTIMP_ERROR, "Not implemented.");
      break;
  }
  return result;
}

static inline int __mrb_bool_p(mrb_value o)
{
  return mrb_type(o) == MRB_TT_FALSE || mrb_type(o) == MRB_TT_TRUE;
}

void mrb_to_lua(mrb_state* mrb, lua_State* L, mrb_value val)
{
  if (mrb_nil_p(val)) {
    lua_pushnil(L);
  }
  else if (mrb_fixnum_p(val)) {
    lua_pushinteger(L, mrb_fixnum(val));
  }
  else if (mrb_float_p(val)) {
    lua_pushnumber(L, mrb_float(val));
  }
  else if (mrb_string_p(val)) {
    lua_pushstring(L, RSTRING_PTR(val));
  }
  else if (mrb_array_p(val)) {
    mrb_raise(mrb, E_NOTIMP_ERROR, "Not implemented.");
  }
  else if (mrb_hash_p(val)) {
    mrb_raise(mrb, E_NOTIMP_ERROR, "Not implemented.");
  }
  else if (mrb_cptr_p(val)) {
    lua_pushcfunction(L, mrb_cptr(val));
  }
  else if (mrb_exception_p(val)) {
    mrb_raise(mrb, E_TYPE_ERROR, "Not support type specified.");
  }
  else if (__mrb_bool_p(val)) {
    lua_pushboolean(L, mrb_bool(val));
  }
  else {
    mrb_raise(mrb, E_TYPE_ERROR, "Not support type specified.");
  }
  return;
}

mrb_value mrb_lua_dostring(mrb_state* mrb, mrb_value self)
{
  lua_State* L = DATA_PTR(self);
  mrb_value str;
  mrb_get_args(mrb, "S", &str);
  if (luaL_dostring(L, RSTRING_PTR(str))) {
    mrb_raise(mrb, E_SCRIPT_ERROR, lua_tostring(L, -1));
  }
  return lua_to_mrb(mrb, L, -1);
}

mrb_value mrb_lua_dofile(mrb_state* mrb, mrb_value self)
{
  lua_State* L = DATA_PTR(self);
  mrb_value path;
  mrb_get_args(mrb, "S", &path);
  if (luaL_dofile(L, RSTRING_PTR(path))) {
    mrb_raise(mrb, E_SCRIPT_ERROR, lua_tostring(L, -1));
  }
  return lua_to_mrb(mrb, L, -1);
}

mrb_value mrb_lua_getglobal(mrb_state* mrb, mrb_value self)
{
  lua_State* L = DATA_PTR(self);
  mrb_value key;
  mrb_get_args(mrb, "S", &key);
  lua_getglobal(L, RSTRING_PTR(key));
  return lua_to_mrb(mrb, L, -1);
}

mrb_value mrb_lua_setglobal(mrb_state* mrb, mrb_value self)
{
  lua_State* L = DATA_PTR(self);
  mrb_value key, val;
  mrb_get_args(mrb, "So", &key, &val);
  mrb_to_lua(mrb, L, val);
  lua_setglobal(L, RSTRING_PTR(key));
  return mrb_nil_value();
}

void mrb_mruby_lua_gem_init(mrb_state* mrb)
{
  struct RClass* rclass = mrb_define_class(mrb, "Lua", mrb->object_class);
  MRB_SET_INSTANCE_TT(rclass, MRB_TT_DATA);
  mrb_define_method(mrb, rclass, "initialize", mrb_lua_init,      MRB_ARGS_NONE());
  mrb_define_method(mrb, rclass, "dostring",   mrb_lua_dostring,  MRB_ARGS_REQ(1));
  mrb_define_method(mrb, rclass, "dofile",     mrb_lua_dofile,    MRB_ARGS_REQ(1));
  mrb_define_method(mrb, rclass, "[]",         mrb_lua_getglobal, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, rclass, "[]=",        mrb_lua_setglobal, MRB_ARGS_REQ(2));
  return;
}

void mrb_mruby_lua_gem_final(mrb_state* mrb)
{
  return;
}


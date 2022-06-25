# dragonruby-lua

**dragonruby-lua** adds a native Lua VM to DragonRuby.

It lets DragonRuby call Lua functions, evaluate Lua strings, and read values from Lua.

You can even call DragonRuby C APIs with LuaJIT's ffi:

```lua
dragon = require('dragon') -- load dragon.lua from your project directory
ffi = require('ffi')

dragon.drb_log_write("Lua", 1, "Hello from Lua!")

local pixels = ffi.new('uint32_t[?]', 256*256)
for i=0,#pixels do
  pixels[i] = 0xff00ff00 -- green, full alpha
end

dragon.drb_upload_pixel_array('my_lua_pixels', 256, 256, pixels)
```

It is a fork of [mruby-lua](https://github.com/dyama/mruby-lua). Thank you to the author 🥳.

## Dependencies

* libluajit (recommended) - headers, static library (-fPIC), or
* liblua5.2 (or newer) - headers, static library (-fPIC)

Tested with DragonRuby 3.18 on Linux.

The original author could compile with MinGW32 on Windows 7.

Hopefully it compiles easily elsewhere.

### Compile Lua/LuaJIT with -fPIC

To combine with this code into a shared library, the compiler needs a static build of liblua or libluajit, with **Position Independent Code**.

The official releases are not built that way, so you must compile Lua/LuaJIT yourself.

Luckily, it is relatively quick and painless.

For Linux, I cloned [lua/lua](https://github.com/lua/lua) / [LuaJIT](https://github.com/LuaJIT/LuaJIT), added '-shared -fPIC' to CFLAGS in their Makefiles, then ran `make -j4`.

A copy of the resulting liblua.a and libluajit.a for Linux x86_64 is included in this repo.

## Ruby API

### Lua

#### Class methods

* `new -> Lua`
  - Create instance of the Lua script engine.

#### Instance methods

* `dostring(script_string) -> Hash|NilClass|TrueClass|FalseClass|Numeric|String`
  - Run Lua script from string object. Return value converted to a Ruby object if possible.

* `run(script_string)`
  - Alias for dostring.

* `<< script_string`
  - Alias for dostring.
    ```ruby
      lua << 'io.write("Hello from Ruby")'
      lua << <<~LUA
        function hello_world(name)
          io.write("Hello " .. (name or "World"))
        end
      LUA
    ```

* `dofile(script_file_path) -> nil`
  - Run Lua script from file.

* `[key] -> object, [key] = object`
  - Get or set global variable.

* `call_global(name, arg1=nil, .., arg9=nil) -> Hash|NilClass|TrueClass|FalseClass|Numeric|String`
  - Calls a global Lua function by name, with up to 9 arguments.
    * Faster than `dostring("foo(1, 2, 3)")`.
    * Arguments converted safely.

## Note about VS Code

If you use heredocs with LUA as the token, then VS Code will highlight the contents as Lua ;-)

```ruby
lua.run(<<~LUA)
  function beautiful_highlighted_lua(code)
    yes:please()
  end
LUA

lua << <<~LUA
  look:ma("it's some", "highlighted", "Lua")
END
```

## Supported data convertion

|MRuby type|To Lua|From Lua|Lua type     |
|:--------:|:----:|:------:|:-----------:|
|Nil       | o    | o      |NIL          |
|Fixnum    | o    | o      |NUMBER       |
|Float     | o    | o      |NUMBER       |
|String    | o    | o      |STRING       |
|array     |      |        |TABLE        |
|hash      |      | o      |TABLE        |
|C pointer |      | o      |FUNCTION(c)  |
| -        |      |        |FUNCTION     |
|Exception |      |        | -           |
|Bool      | o    | o      |BOOLEAN      |
| -        |      |        |USERDATA     |
| -        |      |        |LIGHTUSERDATA|
| -        |      |        |THREAD       |

## Examples from mruby-lua

### Lua#dostring method

    lua = Lua.new
    
    script =<<"EOF"
      function twice(x)
        return x * 2
      end
      a = 10
      b = 20
      print(a * twice(b))
    EOF
    
    lua.dostring script
      => 400

### Lua#dofile method

    lua = Lua.new
    lua.dofile "test/fib.lua"

### Lua#[] method

Get global variable from Lua.

    lua = Lua.new

    lua.dostring "a = 10; b = 20; c = a * b"

    p lua["a"] # => 10
    p lua["b"] # => 20
    p lua["c"] # => 200

    lua.dostring "vec1 = { x = 0.0, y = 1.0, z = 3.0 }"
    p lua["vec1"]
    # => {"x"=>0, "y"=>1, "z"=>3}

### Lua#[]= method

Set global variable to Lua.

    lua = Lua.new

    lua["a"] = 123
    lua["b"] = 23.45
    lua["c"] = true
    lua["d"] = nil

    lua.dostring "print(a)" # => 123
    lua.dostring "print(b)" # => 23.45
    lua.dostring "print(c)" # => true
    lua.dostring "print(d)" # => nil


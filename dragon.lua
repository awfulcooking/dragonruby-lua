local ffi = require('ffi')

-- mruby
ffi.cdef([[
  typedef uint32_t mrb_sym;
  typedef double mrb_float;
  typedef uint8_t mrb_bool;
  typedef int64_t mrb_int;
  typedef uint64_t mrb_uint;

  typedef uint32_t mrb_aspec;
  typedef const char *mrb_args_format;

  union mrb_value_union {
    void *p;
    mrb_int i;
    mrb_sym sym;
  };

  enum mrb_vtype {
    MRB_TT_FALSE = 0,
    MRB_TT_TRUE,
    MRB_TT_FLOAT,
    MRB_TT_INTEGER,
    MRB_TT_SYMBOL,
    MRB_TT_UNDEF,
    MRB_TT_CPTR,
    MRB_TT_FREE,
    MRB_TT_OBJECT,
    MRB_TT_CLASS,
    MRB_TT_MODULE,
    MRB_TT_ICLASS,
    MRB_TT_SCLASS,
    MRB_TT_PROC,
    MRB_TT_ARRAY,
    MRB_TT_HASH,
    MRB_TT_STRING,
    MRB_TT_RANGE,
    MRB_TT_EXCEPTION,
    MRB_TT_ENV,
    MRB_TT_DATA,
    MRB_TT_FIBER,
    MRB_TT_ISTRUCT,
    MRB_TT_BREAK,
    MRB_TT_MAXDEFINE
  };
  
  typedef struct mrb_value {
    union mrb_value_union value;
    enum mrb_vtype tt;
  } mrb_value;

  typedef mrb_value (*mrb_func_t)(struct mrb_state *mrb, mrb_value self);
]])

-- dragonruby
ffi.cdef([[
  typedef enum drb_foreign_object_kind {
    drb_foreign_object_kind_struct,
    drb_foreign_object_kind_pointer
  } drb_foreign_object_kind;

  typedef struct drb_foreign_object {
    drb_foreign_object_kind kind;
  } drb_foreign_object;

  struct mrb_state;
  struct RClass;
  struct RData;
  struct mrb_data_type;

  typedef struct drb_api_t {
    void (*mrb_raise)(struct mrb_state *mrb, struct RClass *c, const char *msg);
    void (*mrb_raisef)(struct mrb_state *mrb, struct RClass *c, const char *fmt, ...);
    struct RClass *(*mrb_module_get)(struct mrb_state *mrb, const char *name);
    struct RClass *(*mrb_module_get_under)(struct mrb_state *mrb, struct RClass *outer,
                                          const char *name);
    struct RClass *(*mrb_define_module_under)(struct mrb_state *mrb, struct RClass *outer,
                                              const char *name);
    struct RClass *(*mrb_class_get_under)(struct mrb_state *mrb, struct RClass *outer,
                                          const char *name);
    struct RClass *(*mrb_define_class_under)(struct mrb_state *mrb, struct RClass *outer,
                                            const char *name, struct RClass *super);
    void (*mrb_define_module_function)(struct mrb_state *mrb, struct RClass *cla, const char *name,
                                      mrb_func_t fun, mrb_aspec aspec);
    void (*mrb_define_method)(struct mrb_state *mrb, struct RClass *cla, const char *name,
                              mrb_func_t func, mrb_aspec aspec);
    void (*mrb_define_class_method)(struct mrb_state *mrb, struct RClass *cla, const char *name,
                                    mrb_func_t fun, mrb_aspec aspec);
    int64_t (*mrb_get_args)(struct mrb_state *mrb, mrb_args_format format, ...);
    mrb_value (*mrb_str_new_cstr)(struct mrb_state *, const char *);
    struct RData *(*mrb_data_object_alloc)(struct mrb_state *mrb, struct RClass *klass, void *ptr,
                                          const struct mrb_data_type *type);

    void (*drb_free_foreign_object)(struct mrb_state *mrb, void *ptr);
    void (*drb_typecheck_float)(struct mrb_state *mrb, mrb_value self);
    void (*drb_typecheck_int)(struct mrb_state *mrb, mrb_value self);
    void (*drb_typecheck_bool)(struct mrb_state *mrb, mrb_value self);
    mrb_value (*drb_float_value)(struct mrb_state *mrb, mrb_float f);
    struct RClass *(*drb_getruntime_error)(struct mrb_state *mrb);
    struct RClass *(*drb_getargument_error)(struct mrb_state *mrb);
    void (*drb_typecheck_aggregate)(struct mrb_state *mrb, mrb_value self, struct RClass *expected,
                                    struct mrb_data_type *data_type);
    void (*drb_upload_pixel_array)(const char *name, const int w, const int h, const uint32_t *pixels);
    void *(*drb_load_image)(const char *fname, int *w, int *h);
    void (*drb_free_image)(void *pixels);
    void (*drb_log_write)(const char *subsystem, const int level, const char *str);
  } drb_api_t;
]])

ffi.cdef([[
  drb_api_t* dr_lua_get_drb_api();
]])

return ffi.C.dr_lua_get_drb_api()


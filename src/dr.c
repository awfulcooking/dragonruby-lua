#include <dragonruby.h>
#include "mruby-lua.h"

static drb_api_t *drb_api;

drb_api_t* dr_lua_get_drb_api() {
  return drb_api;
}

DRB_FFI_EXPORT
void drb_register_c_extensions_with_api(mrb_state *state, struct drb_api_t *api) {
	drb_api = api;
	mrb_mruby_lua_gem_init(state);
}

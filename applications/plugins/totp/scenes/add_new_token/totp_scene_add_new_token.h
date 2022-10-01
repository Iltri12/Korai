#ifndef _TOTP_SCENE_ADD_NEW_TOKEN_H_
#define _TOTP_SCENE_ADD_NEW_TOKEN_H_

#include <gui/gui.h>
#include <furi.h>
#include <furi_hal.h>
#include "../../types/plugin_state.h"
#include "../../types/plugin_event.h"

typedef struct {
    uint8_t current_token_index;
} TokenAddEditSceneContext;

void totp_scene_add_new_token_init(PluginState* plugin_state);
void totp_scene_add_new_token_activate(PluginState* plugin_state, const TokenAddEditSceneContext* context);
void totp_scene_add_new_token_render(Canvas* const canvas, PluginState* plugin_state);
bool totp_scene_add_new_token_handle_event(PluginEvent* const event, PluginState* plugin_state);
void totp_scene_add_new_token_deactivate(PluginState* plugin_state);
void totp_scene_add_new_token_free(PluginState* plugin_state);

#endif

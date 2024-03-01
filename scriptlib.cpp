/*
 * Copyright (c) 2010-2015, Argon Sun (Fluorohydride)
 * Copyright (c) 2018-2024, Edoardo Lolletti (edo9300) <edoardo762@gmail.com>
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
#include "duel.h"
#include "field.h"
#include "lua_obj.h"
#include "scriptlib.h"

namespace scriptlib {

const char* get_lua_type_name(lua_State* L, int32_t index) {
	if(lua_isfunction(L, index))
		return "Function";
	if(lua_isstring(L, index))
		return "String";
	if(lua_isinteger(L, index) || lua_isnumber(L, index))
		return "Int";
	if(lua_isboolean(L, index))
		return "boolean";
	if(lua_isnil(L, index) || lua_isnone(L, index))
		return "nil";
	if(auto* obj = lua_get<lua_obj*>(L, index); obj != nullptr) {
		switch(obj->lua_type) {
		case LuaParam::CARD:
			return "Card";
		case LuaParam::GROUP:
			return "Group";
		case LuaParam::EFFECT:
			return "Effect";
		case LuaParam::DELETED:
			return "Deleted";
		default:
			unreachable();
		}
	}
	return "unknown";
}

void check_action_permission(lua_State* L) {
	if(lua_get<duel*>(L)->lua->no_action)
		lua_error(L, "Action is not allowed here.");
}
int32_t push_return_cards(lua_State* L, int32_t/* status*/, lua_KContext ctx) {
	const auto pduel = lua_get<duel*>(L);
	bool cancelable = (bool)ctx;
	if(pduel->game_field->return_cards.canceled) {
		if(cancelable) {
			lua_pushnil(L);
		} else {
			group* pgroup = pduel->new_group();
			interpreter::pushobject(L, pgroup);
		}
	} else {
		group* pgroup = pduel->new_group(pduel->game_field->return_cards.list);
		interpreter::pushobject(L, pgroup);
	}
	return 1;
}
int32_t is_deleted_object(lua_State* L) {
	if(auto obj = lua_touserdata(L, 1)) {
		auto* ret = *static_cast<lua_obj**>(obj);
		lua_pushboolean(L, ret->lua_type == LuaParam::DELETED);
	} else {
		lua_pushboolean(L, false);
	}
	return 1;
}
}

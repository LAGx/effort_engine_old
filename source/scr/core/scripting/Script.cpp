
#include "core/scripting/Script.h"


void Script::Create(){
	lua_state = luaL_newstate();
	
	static const luaL_Reg lualibs[] = //can include new lua libs
	{
		{"base", luaopen_base},
		{"io", luaopen_io},
		{"string", luaopen_string},
		{NULL, NULL}
	};

	for(const luaL_Reg *lib = lualibs; lib->func != NULL; lib++)
	{
		luaL_requiref(lua_state, lib->name, lib->func, 1);
		lua_settop(lua_state, 0);
	}
}


void Script::Close(){
	lua_close(lua_state);
}


int Script::DoFile(char* ScriptFileName){
	luaL_dofile(lua_state,ScriptFileName);

	return lua_tointeger(lua_state, lua_gettop(lua_state));
}

template<>
void Script::RegisterConstant<int>(int value, char* constantname){
	lua_pushinteger(lua_state, value);
	lua_setglobal(lua_state,constantname);
}

template<>
void Script::RegisterConstant<double>(double value, char* constantname){
	lua_pushnumber(lua_state, value);
	lua_setglobal(lua_state,constantname);
}

template<>
void Script::RegisterConstant<char*>(char* value, char* constantname){
	lua_pushstring(lua_state, value);
	lua_setglobal(lua_state,constantname);
}

template<>
void Script::RegisterConstant<bool>(bool value, char* constantname){
	lua_pushboolean(lua_state, value);
	lua_setglobal(lua_state,constantname);
}

template<>
void Script::RegisterConstant<lua_CFunction>(lua_CFunction value, char* constantname){
	lua_pushcfunction(lua_state, value);
	lua_setglobal(lua_state,constantname);
}

void Script::Array(){
    lua_createtable(lua_state, 2, 0);
}

template<>
void Script::RegisterConstantArray<int>(int value, int index){
	lua_pushnumber(lua_state, index);
	lua_pushinteger(lua_state, value);
	lua_settable(lua_state, -3);
}

template<>
void Script::RegisterConstantArray<double>(double value, int index){
	lua_pushnumber(lua_state, index);
	lua_pushnumber(lua_state, value);
	lua_settable(lua_state, -3);
}

template<>
void Script::RegisterConstantArray<char*>(char* value, int index){
	lua_pushnumber(lua_state, index);
	lua_pushstring(lua_state, value);
	lua_settable(lua_state, -3);
}

template<>
void Script::RegisterConstantArray<bool>(bool value, int index){
	lua_pushnumber(lua_state, index);
	lua_pushboolean(lua_state, value);
	lua_settable(lua_state, -3);
}

template<>
void Script::RegisterConstantArray<lua_CFunction>(lua_CFunction value, int index){
	lua_pushnumber(lua_state, index);
	lua_pushcfunction(lua_state, value);
	lua_settable(lua_state, -3);
}

void Script::RegisterArray(char* arrayname){
	lua_setglobal(lua_state, arrayname);
}

int Script::GetArgumentCount(){
	return lua_gettop(lua_state);
}

template<>
int Script::GetArgument<int>(int index){
	return lua_tointeger(lua_state,index);
}

template<>
double Script::GetArgument<double>(int index){
	return lua_tonumber(lua_state,index);
}

template<>
char* Script::GetArgument<char*>(int index){
	return (char*)lua_tostring(lua_state,index);
}

template<>
bool Script::GetArgument<bool>(int index){
	return lua_toboolean(lua_state,index);
}

template<>
void Script::Return<int>(int value)
{
	lua_pushinteger(lua_state,value);
}

template<>
void Script::Return<double>(double value)
{
	lua_pushnumber(lua_state,value);
}

template<>
void Script::Return<char*>(char* value)
{
	lua_pushstring(lua_state,value);
}

template<>
void Script::Return<bool>(bool value)
{
	lua_pushboolean(lua_state,value);
}

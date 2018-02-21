//DON`T USE. IT`S JUST TEST CODE
#pragma once
#pragma comment(lib, "lua.lib")
extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}
#include <stdio.h>
#include <iostream>
#include <sstream>

using namespace std;

class Script
{
private:
    lua_State *lua_state;

public:

	void Create();

	void Close();

	int DoFile(char* ScriptFileName);

	template<class T>
	void RegisterConstant(T value, char* constantname);

	void Array();

	template<class T>
	void RegisterConstantArray(T value, int index);

	void RegisterArray(char* arrayname);

	int GetArgumentCount();

	template<class T>
	T GetArgument(int index);

	template<class T>
    void Return(T value);
};
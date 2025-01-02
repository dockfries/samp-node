#pragma once

#include <unordered_map>
#include <string>
#include "amx/amx.h"
#include "node.h"
#include "v8.h"

namespace sampnode
{
	namespace native
	{
		void call_real(const v8::FunctionCallbackInfo<v8::Value>& args);
		void call_float_real(const v8::FunctionCallbackInfo<v8::Value>& args);
		void call_fake(const v8::FunctionCallbackInfo<v8::Value>& args);
		void call_float_fake(const v8::FunctionCallbackInfo<v8::Value>& args);
		void call(AMX* amx, const v8::FunctionCallbackInfo<v8::Value>& args);
		void call_float(AMX* amx, const v8::FunctionCallbackInfo<v8::Value>& args);
		AMX_NATIVE get_address(const std::string& name);
	}
}
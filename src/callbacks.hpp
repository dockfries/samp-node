#pragma once
#include "node.h"
#include "v8.h"
#include "amx/amx.h"

namespace sampnode
{
	class callback
	{
	public:
		struct data
		{
			std::string name;
			std::string param_types;
		};

		static void call(const v8::FunctionCallbackInfo<v8::Value> &info);
		static void call_float(const v8::FunctionCallbackInfo<v8::Value> &info);

	private:
		static int execute_amx_call(
				v8::Isolate *isolate,
				const std::string &name,
				const std::string &format,
				const v8::FunctionCallbackInfo<v8::Value> &info,
				v8::Local<v8::Context> &context);
	};

	extern bool js_calling_public;
}
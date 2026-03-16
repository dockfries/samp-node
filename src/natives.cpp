#include <string>
#include <unordered_map>
#include "amx/amx2.h"
#include "common.hpp"
#include "natives.hpp"
#include "sampgdk.h"

namespace sampnode
{
	std::unordered_map<std::string, AMX_NATIVE> pawn_natives_cache;

	AMX_NATIVE native::get_address(const std::string &name)
	{
		if (auto iter = pawn_natives_cache.find(name); iter != pawn_natives_cache.end())
		{
			return iter->second;
		}

		AMX_NATIVE native = sampgdk::FindNative(name.c_str());
		if (native)
		{
			pawn_natives_cache[name] = native;
		}
		return native;
	}

	void native::call(const v8::FunctionCallbackInfo<v8::Value> &args)
	{
		v8::Isolate *isolate = args.GetIsolate();
		v8::Locker locker(isolate);
		v8::Isolate::Scope isolateScope(isolate);

		auto _context = isolate->GetCurrentContext();
		v8::Context::Scope contextScope(_context);

		v8::TryCatch eh(isolate);

		v8::String::Utf8Value str(isolate, args[0]);
		std::string name(*str);

		v8::String::Utf8Value str2(isolate, args[1]);
		std::string_view format(*str2);

		if (format == "undefined")
		{
			format = "";
		}

		AMX_NATIVE native = get_address(name);
		if (!native)
		{
			L_ERROR << "[callNative] native function: " << name << " not found.";
			return;
		}

		void *params[32];
		cell param_value[20];
		int param_size[32];
		int j = 0;
		int k = 2;
		int vars = 0;
		int strs = 0;
		int strv = 0;

		std::string str_format;
		for (char c : format)
		{
			switch (c)
			{
			case 'i':
			{
				param_value[j] = args[k]->Int32Value(_context).ToChecked();
				params[j] = static_cast<void *>(&param_value[j]);
				j++;
				k++;
				str_format += 'i';
			}
			break;

			case 'f':
			{
				float val = 0.0f;
				if (!args[k]->IsUndefined())
					val = static_cast<float>(args[k]->NumberValue(_context).ToChecked());

				param_value[j] = amx_ftoc(val);
				params[j] = static_cast<void *>(&param_value[j]);
				j++;
				k++;
				str_format += 'f';
			}
			break;

			case 's':
			{
				v8::String::Utf8Value _str(isolate, args[k]);
				std::string_view str(*_str);
				char *mystr = new char[str.length() + 1];
				std::copy(str.begin(), str.end(), mystr);
				mystr[str.length()] = '\0';
				params[j] = static_cast<void *>(mystr);
				j++;
				k++;
				str_format += 's';
				strs++;
			}
			break;

			case 'a':
			{
				if (!args[k]->IsArray())
				{
					args.GetReturnValue().Set(false);
					L_ERROR << "callNative: '" << name << "', parameter " << k << "must be an array";
					return;
				}

				v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(args[k++]);
				size_t size = a->Length();

				cell *value = new cell[size];
				for (size_t b = 0; b < size; b++)
				{
					value[b] = a->Get(_context, b).ToLocalChecked()->Int32Value(_context).ToChecked();
				}

				str_format += "a[" + std::to_string(size) + "]";
				params[j++] = static_cast<void *>(value);
				strs++;
			}
			break;

			case 'v':
			{
				if (!args[k]->IsArray())
				{
					args.GetReturnValue().Set(false);
					L_ERROR << "callNative: '" << name << "', parameter " << k << "must be an array";
					return;
				}

				v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(args[k++]);
				size_t size = a->Length();

				cell *value = new cell[size];
				for (size_t b = 0; b < size; b++)
				{
					float val = static_cast<float>(a->Get(_context, b).ToLocalChecked()->NumberValue(_context).ToChecked());
					value[b] = amx_ftoc(val);
				}

				str_format += "a[" + std::to_string(size) + "]";
				params[j++] = static_cast<void *>(value);
				strs++;
			}
			break;

			case 'F':
			case 'I':
			{
				param_value[j] = 0;
				params[j] = static_cast<void *>(&param_value[j]);
				j++;
				vars++;
				str_format += 'R';
			}
			break;

			case 'A':
			{
				const int size = args[k]->Int32Value(_context).ToChecked();
				param_size[j] = size;
				cell *value = new cell[size]();
				params[j] = static_cast<void *>(value);
				j++;
				str_format += "A[" + std::to_string(size) + "]";
				vars++;
			}
			break;

			case 'V':
			{
				const int size = args[k]->Int32Value(_context).ToChecked();
				param_size[j] = size;
				cell *value = new cell[size];
				float val = 0.0f;
				for (int c = 0; c < size; c++)
				{
					value[c] = amx_ftoc(val);
				}
				params[j] = static_cast<void *>(value);
				j++;
				str_format += "A[" + std::to_string(size) + "]";
				vars++;
			}
			break;

			case 'S':
			{
				unsigned int strl = args[k]->Int32Value(_context).ToChecked();
				if (strl < 1)
				{
					L_ERROR << "callNative: '" << name << "' - String length can't be 0";
					return;
				}

				param_size[j] = static_cast<cell>(strl);
				str_format += "S[" + std::to_string(strl) + "]";
				params[j] = new char[strl]();
				j++;
				vars++;
			}
			break;
			}
		}

		int32_t retval = sampgdk::InvokeNativeArray(native, str_format.c_str(), params);

		if (vars > 0 || strs > 0)
		{
			v8::Local<v8::Array> arr = v8::Array::New(isolate, vars);
			int var_index = 0;
			j = 0;

			for (char c : format)
			{
				switch (c)
				{
				case 'i':
				case 'f':
				{
					j++;
				}
				break;

				case 's':
				case 'a':
				case 'v':
				{
					delete[] static_cast<char *>(params[j++]);
				}
				break;

				case 'A':
				{
					int size = param_size[j];
					v8::Local<v8::Array> rArr = v8::Array::New(isolate, size);
					cell *prams = static_cast<cell *>(params[j]);
					for (int c = 0; c < size; c++)
					{
						rArr->Set(_context, c, v8::Integer::New(isolate, prams[c]));
					}
					arr->Set(_context, var_index++, rArr);
					delete[] static_cast<char *>(params[j++]);
				}
				break;

				case 'V':
				{
					cell *param_array = static_cast<cell *>(params[j]);
					int size = param_size[j];
					v8::Local<v8::Array> rArr = v8::Array::New(isolate, size);
					for (int c = 0; c < size; c++)
					{
						rArr->Set(_context, c, v8::Number::New(isolate, amx_ctof(param_array[c])));
					}
					arr->Set(_context, var_index++, rArr);
					delete[] static_cast<char *>(params[j++]);
				}
				break;

				case 'I':
				{
					int val = *static_cast<cell *>(params[j++]);
					arr->Set(_context, var_index++, v8::Integer::New(isolate, val));
				}
				break;

				case 'F':
				{
					float val = amx_ctof(*static_cast<cell *>(params[j++]));
					arr->Set(_context, var_index++, v8::Number::New(isolate, val));
				}
				break;

				case 'S':
				{
					size_t s_len = param_size[j];
					char *s_str = static_cast<char *>(params[j]);
					s_str[s_len - 1] = '\0';
					arr->Set(_context, var_index++, v8::String::NewFromUtf8(isolate, s_str).ToLocalChecked());
					delete[] static_cast<char *>(params[j++]);
				}
				break;
				}
			}

			if (var_index >= 1)
			{
				arr->Set(_context, var_index, v8::Integer::New(isolate, retval));
				args.GetReturnValue().Set(arr);
			}
			else
			{
				args.GetReturnValue().Set(retval);
			}
		}
		else
		{
			args.GetReturnValue().Set(retval);
		}

		if (eh.HasCaught())
		{
			v8::String::Utf8Value error(isolate, eh.Exception());
			v8::String::Utf8Value stack(isolate, eh.StackTrace(_context).ToLocalChecked());
			L_ERROR << "[samp-node] event handling function in resource: " << *error << "\nstack:\n"
							<< *stack << "\n";
		}
	}

	void native::call_float(const v8::FunctionCallbackInfo<v8::Value> &args)
	{
		call(args);

		v8::Isolate *isolate = args.GetIsolate();
		v8::HandleScope handleScope(isolate);
		v8::Local<v8::Context> context = isolate->GetCurrentContext();

		v8::Local<v8::Value> retVal = args.GetReturnValue().Get();

		if (!retVal->IsUndefined())
		{
			int32_t retval = retVal->Int32Value(context).ToChecked();
			args.GetReturnValue().Set(amx_ctof(retval));
		}
	}
}
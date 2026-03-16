#include <string>
#include <vector>
#include "logger.hpp"
#include "resource.hpp"
#include "events.hpp"
#include "callbacks.hpp"
#include "amx/amx.h"
#include "amxhandler.hpp"
#include "sampgdk.h"

namespace sampnode
{
	bool js_calling_public = false;

	int callback::execute_amx_call(
			v8::Isolate *isolate,
			const std::string &name,
			const std::string &format,
			const v8::FunctionCallbackInfo<v8::Value> &info,
			v8::Local<v8::Context> &context)
	{
		int k = 2;
		cell _params[32];
		std::vector<void *> params;
		int numberOfStrings = 0;
		int arraySizes[32];

		for (int i = 0; i < static_cast<int>(format.length()); i++)
		{
			switch (format[i])
			{
			case 'i':
			case 'd':
			{
				_params[i] = info[k]->Int32Value(context).ToChecked();
				params.push_back(static_cast<void *>(&_params[i]));
				k++;
			}
			break;
			case 'f':
			{
				float val = 0.0f;
				if (!info[k]->IsUndefined())
					val = static_cast<float>(info[k]->NumberValue(context).ToChecked());
				_params[i] = amx_ftoc(val);
				params.push_back(static_cast<void *>(&_params[i]));
				k++;
			}
			break;
			case 's':
			{
				v8::String::Utf8Value _str(isolate, info[k]);
				const char *str(*_str);
				size_t slen = std::char_traits<char>::length(str);
				char *mystr = new char[slen + 1];
				std::copy(str, str + slen, mystr);
				mystr[slen] = '\0';
				params.push_back(static_cast<void *>(mystr));
				numberOfStrings++;
				k++;
			}
			break;
			case 'a':
			{
				if (!info[k]->IsArray())
				{
					L_ERROR << "callPublic: '" << name << "', parameter " << k << "must be an array";
					return 0;
				}
				v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(info[k]);
				size_t size = a->Length();
				cell *value = new cell[size];
				for (size_t b = 0; b < size; b++)
				{
					value[b] = a->Get(context, b).ToLocalChecked()->Int32Value(context).ToChecked();
				}
				params.push_back(static_cast<void *>(value));
				arraySizes[i] = size;
				numberOfStrings++;
				k++;
			}
			break;
			case 'v':
			{
				if (!info[k]->IsArray())
				{
					L_ERROR << "callPublic: '" << name << "', parameter " << k << "must be an array";
					return 0;
				}
				v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(info[k]);
				size_t size = a->Length();
				cell *value = new cell[size];
				for (size_t b = 0; b < size; b++)
				{
					float val = static_cast<float>(a->Get(context, b).ToLocalChecked()->NumberValue(context).ToChecked());
					value[b] = amx_ftoc(val);
				}
				params.push_back(static_cast<void *>(value));
				arraySizes[i] = size;
				numberOfStrings++;
				k++;
			}
			break;
			default:
				break;
			}
		}

		std::vector<cell> amx_addr;
		for (int i = 0; i < numberOfStrings; i++)
		{
			amx_addr.push_back(0);
		}
		numberOfStrings = 0;

		int returnValue = 0;
		for (auto &amx : amx::amx_list)
		{
			int callback = 0;
			if (amx_FindPublic(amx.second->get(), name.c_str(), &callback))
			{
				continue;
			}

			if (callback < -10000)
			{
				continue;
			}

			for (int i = format.length() - 1; i >= 0; i--)
			{
				switch (format[i])
				{
				case 'd':
				case 'i':
				{
					amx_Push(amx.second->get(), *reinterpret_cast<cell *>(params[i]));
					break;
				}
				case 'f':
				{
					amx_Push(amx.second->get(), *reinterpret_cast<cell *>(params[i]));
					break;
				}
				case 's':
				{
					char *string = static_cast<char *>(params[i]);
					if (string != nullptr && std::char_traits<char>::length(string) > 0)
					{
						amx_PushString(amx.second->get(), &amx_addr[numberOfStrings], nullptr, string, 0, 0);
					}
					else
					{
						*string = 1;
						*(string + 1) = '\0';
						amx_PushString(amx.second->get(), &amx_addr[numberOfStrings], nullptr, string, 0, 0);
					}
					numberOfStrings++;
					break;
				}
				case 'a':
				case 'v':
				{
					cell *amxArray = static_cast<cell *>(params[i]);
					amx_PushArray(amx.second->get(), &amx_addr[numberOfStrings], nullptr, amxArray, arraySizes[i]);
					numberOfStrings++;
				}
				break;
				default:
					break;
				}
			}

			js_calling_public = true;
			amx_Exec(amx.second->get(), reinterpret_cast<cell *>(&returnValue), callback);
			js_calling_public = false;

			while (numberOfStrings)
			{
				numberOfStrings--;
				amx_Release(amx.second->get(), amx_addr[numberOfStrings]);
			}
		}

		for (auto &param : params)
		{
			if (format[&param - &params[0]] == 's')
			{
				delete[] static_cast<char *>(param);
			}
			else if (format[&param - &params[0]] == 'a' || format[&param - &params[0]] == 'v')
			{
				delete[] static_cast<cell *>(param);
			}
		}

		return returnValue;
	}

	void callback::call(const v8::FunctionCallbackInfo<v8::Value> &info)
	{
		v8::Isolate *isolate = info.GetIsolate();
		v8::Locker locker(isolate);
		v8::Isolate::Scope isolateScope(isolate);
		auto context = isolate->GetCurrentContext();
		v8::Context::Scope contextScope(context);
		v8::TryCatch eh(isolate);

		if (info.Length() > 0)
		{
			v8::String::Utf8Value str(isolate, info[0]);
			std::string name(*str);

			v8::String::Utf8Value str2(isolate, info[1]);
			std::string format(*str2);

			int returnValue = execute_amx_call(isolate, name, format, info, context);
			info.GetReturnValue().Set(returnValue);
		}
		else
		{
			info.GetReturnValue().Set(0);
		}
	}

	void callback::call_float(const v8::FunctionCallbackInfo<v8::Value> &info)
	{
		v8::Isolate *isolate = info.GetIsolate();
		v8::Locker locker(isolate);
		v8::Isolate::Scope isolateScope(isolate);
		auto context = isolate->GetCurrentContext();
		v8::Context::Scope contextScope(context);
		v8::TryCatch eh(isolate);

		if (info.Length() > 0)
		{
			v8::String::Utf8Value str(isolate, info[0]);
			std::string name(*str);

			v8::String::Utf8Value str2(isolate, info[1]);
			std::string format(*str2);

			int returnValue = execute_amx_call(isolate, name, format, info, context);
			info.GetReturnValue().Set(amx_ctof(returnValue));
		}
		else
		{
			info.GetReturnValue().Set(0.0f);
		}
	}
}
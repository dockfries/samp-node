
#include <algorithm>
#include <vector>
#include "v8.h"
#include "uv.h"
#include "node.h"
#include "plugincommon.h"
#include "amx/amx.h"
#include "events.hpp"
#include "utils.hpp"

namespace sampnode
{

	eventsContainer events = eventsContainer();

	bool event::register_event(const std::string& eventName, const std::string& param_types)
	{
		if (events.find(eventName) != events.end()) return false;
		events.insert({ eventName, new event(eventName, param_types) });
		return true;
	}

	void event::register_event(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		if (info.Length() > 1)
		{
			v8::HandleScope scope(info.GetIsolate());
			if (!info[0]->IsString() || !info[1]->IsString())
			{
				info.GetReturnValue().Set(false);
				return;
			}
			else
			{
				std::string& eventName = utils::js_to_string(info[0]);
				std::string& paramTypes = utils::js_to_string(info[1]);
				if (events.find(eventName) != events.end())
				{
					info.GetReturnValue().Set(false);
					return;
				}
				events.insert({ eventName, new event(eventName, paramTypes) });
				info.GetReturnValue().Set(true);
			}
		}
	}

	void event::on(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		if (info.Length() > 0)
		{
			v8::HandleScope scope(info.GetIsolate());
			if (!info[0]->IsString())
				return;

			int funcArgIndex = info.Length() - 1;
			std::string& eventName = utils::js_to_string(info[0]);

			if (events.find(eventName) == events.end()) return;
			event* _event = events[eventName];

			if ((funcArgIndex >= 0) && (info[funcArgIndex]->IsFunction()))
			{
				v8::Local<v8::Function> function = info[funcArgIndex].As<v8::Function>();
				_event->append(function);
			}
		}
	}

	void event::remove_listener(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		if (info.Length() > 0)
		{
			v8::HandleScope scope(info.GetIsolate());
			if (!info[0]->IsString())
				return;

			std::string& eventName = utils::js_to_string(info[0]);

			if (events.find(eventName) == events.end()) return;

			event* _event = events[eventName];

			if (info.Length() > 1)
			{
				if (info[1]->IsArray())
				{
					v8::Local<v8::Array> funcArray = v8::Local<v8::Array>::Cast(info[1]);
					for (unsigned int i = 0; i < funcArray->Length(); i++)
					{
						v8::Local<v8::Value>& element = funcArray->Get(i);
						v8::Local<v8::Function>& function = element.As<v8::Function>();
						for (auto& element : _event->functionList) {
							if (element.function.Get(info.GetIsolate()) == function)
							{
								v8::Isolate* isolate = info.GetIsolate();
								_event->remove(element.function.Get(info.GetIsolate()));
								break;
							}
						}

					}
				}
				else
				{
					v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(info[1]);
					for (auto& element : _event->functionList) {
						if (element.function.Get(info.GetIsolate()) == function)
						{
							_event->remove(element.function.Get(info.GetIsolate()));
							break;
						}
					}
				}
			}
			else if (info.Length() == 1)
			{
				_event->remove_all();
			}
		}
	}

	event::event(const std::string& eventName, const std::string& param_types)
	{
		name = eventName;
		paramTypes = param_types;
		functionList = std::vector<EventListener_t>();
	}

	event::event()
	{

	}

	event::~event()
	{

	}

	void event::append(const v8::Local<v8::Function>& function)
	{
		v8::Isolate* isolate = function->GetIsolate();
		for (auto& element : functionList)
		{
			if (element.function.Get(isolate) == function)
				return;
		}

		functionList.push_back(
			EventListener_t(
				isolate,
				isolate->GetCurrentContext(),
				function
			)
		);

		//listener.Get(isolate)->Call(isolate->GetCurrentContext()->Global(), 0, NULL);
	}

	void event::remove(const v8::Local<v8::Function>& function)
	{
		v8::Isolate* isolate = function->GetIsolate();
		EventListener_t eventListener = EventListener_t(
			isolate,
			isolate->GetCurrentContext(),
			function
		);
		functionList.erase(std::remove(functionList.begin(), functionList.end(), eventListener));
		functionList.shrink_to_fit();
	}

	void event::remove_all()
	{
		functionList.clear();
	}

	void event::call(AMX* amx, cell* params, cell* retval)
	{
		for (auto& listener : functionList)
		{
			v8::Isolate* isolate = listener.isolate;
			v8::Locker v8Locker(listener.isolate);
			v8::HandleScope hs(listener.isolate);
			v8::Local<v8::Context> ctx = v8::Local<v8::Context>::New(listener.isolate, listener.context);
			v8::Context::Scope cs(ctx);
			v8::TryCatch eh(listener.isolate);

			v8::Local<v8::Value>* argv = NULL;
			unsigned int argc = paramTypes.length();
			argv = new v8::Local<v8::Value>[argc];;
			size_t param_count = params[0] / sizeof(cell);
			for (unsigned int i = 0; i < argc; i++)
			{
				switch (paramTypes[i])
				{
				case 's':
				{
					cell* maddr = NULL;
					int len = 0;
					char* sval;
					if (amx_GetAddr(amx, params[i + 1], &maddr) != AMX_ERR_NONE)
					{
						printf("Can't get string address: %s", name.c_str());
						return;
					}
					amx_StrLen(maddr, &len);
					sval = new char[len + 1];
					if (amx_GetString(sval, maddr, 0, len + 1) != AMX_ERR_NONE)
					{
						printf("Can't get string: %s", name.c_str());
						return;
					}
					argv[i] = v8::String::NewFromUtf8(isolate, sval);
					break;
				}
				case 'd':
				{
					argv[i] = v8::Integer::New(isolate, static_cast<int32_t>(params[i + 1]));
					break;
				}
				case 'i':
				{
					argv[i] = v8::Integer::New(isolate, static_cast<uint16_t>(params[i + 1]));
					break;
				}
				case 'f':
				{
					argv[i] = v8::Number::New(isolate, amx_ctof(params[i + 1]));
					break;
				}
				}
			}

			v8::Local<v8::Function> function = listener.function.Get(listener.isolate);
			function->Call(listener.context.Get(listener.isolate)->Global(), argc, argv);

			if (argc > 0) delete[] argv;
			int retvalue = 1;
			if (retval != nullptr) *retval = static_cast<cell>(retvalue);
			if (eh.HasCaught())
			{
				v8::String::Utf8Value str(listener.isolate, eh.Exception());
				v8::String::Utf8Value stack(listener.isolate, eh.StackTrace(listener.context.Get(listener.isolate)).ToLocalChecked());

				printf("[samp-node] event handling function in resource: %s\nstack:\n%s\n", *str, *stack);
			}
		}
	}
}
#include <algorithm>
#include <vector>
#include "plugincommon.h"
#include "amx/amx.h"
#include "events.hpp"
#include "utils.hpp"
#include "logger.hpp"
#include "nodeimpl.hpp"
#include "uv.h"
#include "node.h"

namespace sampnode
{
	eventsContainer events = eventsContainer();

	int handlePromiseReturnValue(v8::Local<v8::Value> returnValue, v8::Isolate *isolate)
	{
		if (returnValue->IsPromise())
		{
			v8::Local<v8::Promise> promise = returnValue.As<v8::Promise>();
			while (true)
			{
				v8::Promise::PromiseState state = promise->State();
				if (state == v8::Promise::PromiseState::kPending)
				{
					sampnode::nodeImpl.Tick();
				}
				else if (state == v8::Promise::PromiseState::kFulfilled)
				{
					v8::Local<v8::Value> result = promise->Result();
					return +(result->BooleanValue(isolate));
				}
				else if (state == v8::Promise::PromiseState::kRejected)
				{
					return 0;
				}
			}
		}
		else
		{
			return +(returnValue->BooleanValue(isolate));
		}
	}

	v8::Local<v8::Value> *convertAmxParamsToV8(AMX *amx, cell *params, v8::Isolate *isolate, v8::Local<v8::Context> ctx, const std::string &paramTypes, unsigned int &argc, int &paramOffset, bool isFromPawnNative)
	{
		argc = paramTypes.length();
		v8::Local<v8::Value> *argv = new v8::Local<v8::Value>[argc];
		paramOffset = 0;

		for (unsigned int i = 0; i < argc; i++)
		{
			switch (paramTypes[i])
			{
			case 's':
			{
				cell *maddr = NULL;
				int len = 0;
				char *sval;
				if (amx_GetAddr(amx, params[i + paramOffset + 1], &maddr) != AMX_ERR_NONE)
				{
					L_ERROR << "Can't get string address";
					delete[] argv;
					return nullptr;
				}
				amx_StrLen(maddr, &len);
				sval = new char[len + 1];
				if (amx_GetString(sval, maddr, 0, len + 1) != AMX_ERR_NONE)
				{
					L_ERROR << "Can't get string address";
					delete[] argv;
					delete[] sval;
					return nullptr;
				}
				argv[i] = v8::String::NewFromUtf8(isolate, sval).ToLocalChecked();
				delete[] sval;
				break;
			}
			case 'a':
			{
				cell *array = NULL;
				if (amx_GetAddr(amx, params[i + paramOffset + 1], &array) != AMX_ERR_NONE)
				{
					L_ERROR << "Can't get array address";
					delete[] argv;
					return nullptr;
				}
				int size;
				if (isFromPawnNative)
				{
					size = static_cast<int>(*utils::get_amxaddr(amx, params[i + paramOffset + 2]));
				}
				else
				{
					size = params[i + 2];
					L_INFO << "Array size: " << size;
				}
				v8::Local<v8::Array> jsArray = v8::Array::New(isolate, size);
				for (int j = 0; j < size; j++)
				{
					jsArray->Set(ctx, j, v8::Integer::New(isolate, static_cast<uint32_t>(array[j])));
				}
				argv[i] = jsArray;
				paramOffset++;
				break;
			}
			case 'v':
			{
				cell *array = NULL;
				if (amx_GetAddr(amx, params[i + paramOffset + 1], &array) != AMX_ERR_NONE)
				{
					L_ERROR << "Can't get float array address";
					delete[] argv;
					return nullptr;
				}

				int size;
				if (isFromPawnNative)
				{
					size = static_cast<int>(*utils::get_amxaddr(amx, params[i + paramOffset + 2]));
				}
				else
				{
					size = params[i + 2];
				}
				v8::Local<v8::Array> jsArray = v8::Array::New(isolate, size);
				for (int j = 0; j < size; j++)
				{
					jsArray->Set(ctx, j, v8::Integer::New(isolate, amx_ctof(array[j])));
				}
				argv[i] = jsArray;
				paramOffset++;
				break;
			}
			case 'd':
			{
				int32_t value;
				if (isFromPawnNative)
				{
					value = static_cast<int32_t>(*utils::get_amxaddr(amx, params[i + paramOffset + 1]));
				}
				else
				{
					value = static_cast<int32_t>(params[i + 1]);
				}
				argv[i] = v8::Integer::New(isolate, value);
				break;
			}
			case 'i':
			{
				uint32_t value;
				if (isFromPawnNative)
				{
					value = static_cast<uint32_t>(*utils::get_amxaddr(amx, params[i + paramOffset + 1]));
				}
				else
				{
					value = static_cast<uint32_t>(params[i + 1]);
				}
				argv[i] = v8::Integer::New(isolate, value);
				break;
			}
			case 'f':
			{
				float value;
				if (isFromPawnNative)
				{
					value = amx_ctof(*utils::get_amxaddr(amx, params[i + paramOffset + 1]));
				}
				else
				{
					value = amx_ctof(params[i + 1]);
				}
				argv[i] = v8::Number::New(isolate, value);
				break;
			}
			}
		}

		return argv;
	}

	bool event::register_event(const std::string &eventName, const std::string &param_types)
	{
		if (events.find(eventName) != events.end())
			return false;
		events.insert({eventName, new event(eventName, param_types)});
		return true;
	}

	void event::register_event(const v8::FunctionCallbackInfo<v8::Value> &info)
	{
		if (info.Length() > 1)
		{
			auto isolate = info.GetIsolate();
			v8::Locker locker(isolate);
			v8::Isolate::Scope isolateScope(isolate);
			v8::HandleScope scope(isolate);

			v8::Local<v8::Context> context = isolate->GetCurrentContext();
			v8::Context::Scope contextScope(context);

			if (!info[0]->IsString() || !info[1]->IsString())
			{
				info.GetReturnValue().Set(false);
				return;
			}
			else
			{
				std::string eventName = utils::js_to_string(isolate, info[0]);
				std::string paramTypes = utils::js_to_string(isolate, info[1]);
				if (events.find(eventName) != events.end())
				{
					info.GetReturnValue().Set(false);
					return;
				}
				events.insert({eventName, new event(eventName, paramTypes)});
				info.GetReturnValue().Set(true);
			}
		}
	}

	void event::on(const v8::FunctionCallbackInfo<v8::Value> &info)
	{
		if (info.Length() > 0)
		{
			auto isolate = info.GetIsolate();
			v8::Locker locker(isolate);
			v8::Isolate::Scope isolateScope(isolate);
			v8::HandleScope scope(isolate);

			v8::Local<v8::Context> context = isolate->GetCurrentContext();
			v8::Context::Scope contextScope(context);

			if (!info[0]->IsString())
				return;

			int funcArgIndex = info.Length() - 1;
			std::string eventName = utils::js_to_string(isolate, info[0]);

			if (events.find(eventName) == events.end())
				return;
			event *_event = events[eventName];

			if ((funcArgIndex >= 0) && (info[funcArgIndex]->IsFunction()))
			{
				v8::Local<v8::Function> function = info[funcArgIndex].As<v8::Function>();
				_event->append(context, function);
			}
		}
	}

	void event::remove_listener(const v8::FunctionCallbackInfo<v8::Value> &info)
	{
		if (info.Length() > 0)
		{
			auto isolate = info.GetIsolate();
			v8::Locker locker(isolate);
			v8::Isolate::Scope isolateScope(isolate);
			v8::HandleScope scope(isolate);

			v8::Local<v8::Context> _context = isolate->GetCurrentContext();

			v8::Context::Scope contextScope(_context);

			if (!info[0]->IsString())
				return;

			std::string eventName = utils::js_to_string(isolate, info[0]);

			if (events.find(eventName) == events.end())
				return;

			event *_event = events[eventName];

			if (info.Length() > 1)
			{
				if (info[1]->IsArray())
				{
					v8::Local<v8::Array> funcArray = v8::Local<v8::Array>::Cast(info[1]);
					for (unsigned int i = 0; i < funcArray->Length(); i++)
					{
						const v8::Local<v8::Function> &function = funcArray->Get(_context, i).ToLocalChecked().As<v8::Function>();
						for (auto &element : _event->functionList)
						{
							if (element.context == _context && element.function.Get(isolate) == function)
							{
								_event->remove(element);
								break;
							}
						}
					}
				}
				else
				{
					v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(info[1]);
					for (auto &element : _event->functionList)
					{
						if (element.context == _context && element.function.Get(isolate) == function)
						{
							_event->remove(element);
							break;
						}
					}
				}
			}
			else if (info.Length() == 1)
			{
				auto copiedFunctionList = _event->functionList;

				for (auto &element : copiedFunctionList)
				{
					if (element.context == _context)
					{
						_event->remove(element);
					}
				}
			}
		}
	}

	cell event::pawn_call_event(AMX *amx, cell *params)
	{
		char *eventName_c;
		amx_StrParam(amx, params[1], eventName_c);
		const std::string eventName(eventName_c);

		const auto &_event = events.find(eventName);
		if (_event == events.end())
			return 0;

		cell retVal = 0;
		_event->second->call(amx, params + 1, &retVal, true);
		return retVal;
	}

	event::event(const std::string &eventName, const std::string &param_types)
			: name(eventName),
				paramTypes(param_types),
				functionList(std::vector<EventListener_t>())
	{
	}

	event::event()
	{
	}

	event::~event()
	{
	}

	void event::append(const v8::Local<v8::Context> &context, const v8::Local<v8::Function> &function)
	{
		v8::Isolate *isolate = function->GetIsolate();

		bool result = std::any_of(functionList.cbegin(), functionList.cend(),
															[&function, &isolate](const EventListener_t &listener)
															{
																return listener.function.Get(isolate) == function;
															});

		if (result)
		{
			return;
		}

		functionList.push_back(
				EventListener_t(
						isolate,
						context,
						function));
	}

	void event::remove(const EventListener_t &eventListener)
	{
		functionList.erase(std::remove(functionList.begin(), functionList.end(), eventListener), functionList.end());
	}

	void event::remove_all()
	{
		functionList.clear();
	}

	void event::call(v8::Local<v8::Value> *args, int argCount)
	{
		std::vector<EventListener_t> copiedFunctionList = functionList;

		for (auto &listener : copiedFunctionList)
		{
			if (std::find(functionList.begin(), functionList.end(), listener) == functionList.end())
			{
				continue;
			}

			v8::Isolate *isolate = listener.isolate;
			v8::Locker v8Locker(isolate);
			v8::Isolate::Scope isolateScope(isolate);
			v8::HandleScope hs(isolate);
			v8::Local<v8::Context> ctx = listener.context.Get(isolate);
			v8::Context::Scope cs(ctx);

			isolate->CancelTerminateExecution();

			v8::TryCatch eh(isolate);

			v8::Local<v8::Function> function = listener.function.Get(isolate);
			function->Call(ctx, ctx->Global(), argCount, args);

			if (eh.HasCaught())
			{
				v8::String::Utf8Value str(isolate, eh.Exception());
				v8::String::Utf8Value stack(isolate, eh.StackTrace(listener.context.Get(isolate)).ToLocalChecked());

				L_ERROR << "Event handling function in resource: " << *str << "\nstack:\n"
								<< *stack << "\n";
			}
		}

		if (argCount > 0)
			delete[] args;
	}

	void event::call(AMX *amx, cell *params, cell *retval, bool isFromPawnNative)
	{
		std::vector<EventListener_t> copiedFunctionList = functionList;

		for (auto &listener : copiedFunctionList)
		{
			if (std::find(functionList.begin(), functionList.end(), listener) == functionList.end())
			{
				continue;
			}

			v8::Isolate *isolate = listener.isolate;
			v8::Locker v8Locker(isolate);
			v8::Isolate::Scope isolateScope(isolate);
			v8::HandleScope hs(isolate);
			v8::Local<v8::Context> ctx = listener.context.Get(isolate);
			v8::Context::Scope cs(ctx);

			isolate->CancelTerminateExecution();

			v8::TryCatch eh(isolate);

			unsigned int argc;
			int paramOffset;
			v8::Local<v8::Value> *argv = convertAmxParamsToV8(amx, params, isolate, ctx, paramTypes, argc, paramOffset, isFromPawnNative);
			if (argv == nullptr)
			{
				L_ERROR << "Failed to convert AMX parameters to V8 values: " << name.c_str();
				return;
			}

			v8::Local<v8::Function> function = listener.function.Get(isolate);
			v8::MaybeLocal<v8::Value> returnValue = function->Call(ctx, ctx->Global(), argc, argv);

			if (argc > 0)
				delete[] argv;

			if (eh.HasCaught())
			{
				v8::String::Utf8Value str(isolate, eh.Exception());
				v8::String::Utf8Value stack(isolate, eh.StackTrace(listener.context.Get(isolate)).ToLocalChecked());

				L_ERROR << "Exception thrown: " << *str << "\nstack:\n"
								<< *stack << (isFromPawnNative ? "\n" : "");
			}
			else
			{
				v8::Local<v8::Value> returnValueLocal = returnValue.ToLocalChecked();
				int cppIntReturnValue = handlePromiseReturnValue(returnValueLocal, isolate);
				if (retval != nullptr)
					*retval = static_cast<cell>(cppIntReturnValue);
			}
		}
	}
}

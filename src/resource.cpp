#include "resource.hpp"
#include "functions.hpp"
#include "nodeimpl.hpp"
#include "bootstrap.hpp"
#include "logger.hpp"

namespace sampnode
{

	namespace
	{
		void LogV8Error(v8::Isolate *isolate, const v8::TryCatch &try_catch)
		{
			v8::String::Utf8Value exception(isolate, try_catch.Exception());
			v8::Local<v8::Message> message = try_catch.Message();

			if (message.IsEmpty())
			{
				L_ERROR << *exception;
			}
			else
			{
				v8::String::Utf8Value filename(isolate, message->GetScriptOrigin().ResourceName());
				int linenum = message->GetLineNumber(isolate->GetCurrentContext()).ToChecked();
				L_ERROR << *filename << ":" << linenum << ": " << *exception;
			}
		}
	}

	Resource::Resource(const std::string &name)
			: name(name),
				nodeEnvironment(nullptr, node::FreeEnvironment),
				asyncContext{}
	{
	}

	Resource::Resource()
			: name(""),
				nodeEnvironment(nullptr, node::FreeEnvironment),
				asyncContext{}
	{
	}

	Resource::~Resource()
	{
		Stop();
	}

	void Resource::Init()
	{
		v8::Isolate *isolate = sampnode::nodeImpl.GetIsolate();
		v8::Locker locker(isolate);
		v8::Isolate::Scope isolateScope(isolate);
		v8::HandleScope handleScope(isolate);

		v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
		sampnode::functions::init(isolate, global);

		v8::Local<v8::Context> context = node::NewContext(isolate, global);
		this->context.Reset(isolate, context);
		v8::Context::Scope scope(context);

		node::EnvironmentFlags::Flags flags = static_cast<node::EnvironmentFlags::Flags>(
				node::EnvironmentFlags::kOwnsProcessState);

		auto env = node::CreateEnvironment(
				sampnode::nodeImpl.GetNodeIsolate(),
				context,
				{}, // args
				{}, // exec_args
				flags);

		if (env)
		{
			node::LoadEnvironment(env, bootstrap.c_str());
			nodeEnvironment.reset(env);

			v8::Local<v8::Object> asyncResourceObj = v8::Object::New(isolate);
			asyncResource.Reset(isolate, asyncResourceObj);
			
			asyncContext = node::EmitAsyncInit(isolate, asyncResourceObj, "sampNodeResource");
		}
		else
		{
			L_ERROR << "Failed to create Node.js environment";
		}
	}

	void Resource::Stop()
	{
		{
			v8::Isolate *isolate = sampnode::nodeImpl.GetIsolate();
			v8::Context::Scope scope(context.Get(isolate));
			node::EmitAsyncDestroy(isolate, asyncContext);
			asyncResource.Reset();
		}

		node::Environment *env = nodeEnvironment.get();

		node::EmitProcessBeforeExit(env);
		node::EmitProcessExit(env);

		context.Reset();

		node::Stop(env);

		node::FreeEnvironment(env);
	}
}
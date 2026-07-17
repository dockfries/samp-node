#include <thread>
#include <chrono>
#include <cstdlib>
#include "config.hpp"
#include "resource.hpp"
#include "nodeimpl.hpp"

void OnMessage(v8::Local<v8::Message> message, v8::Local<v8::Value> error)
{
	auto isolate = sampnode::nodeImpl.GetIsolate();
	v8::Locker locker(isolate);
	v8::Isolate::Scope isolateScope(isolate);
	v8::HandleScope handleScope(isolate);

	// Prefer error.stack if available (respects --enable-source-maps)
	if (error->IsObject()) {
		v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
		v8::Local<v8::Object> errObj = error.As<v8::Object>();
		v8::Local<v8::Value> stackVal;
		if (errObj->Get(ctx, v8::String::NewFromUtf8(isolate, "stack").ToLocalChecked()).ToLocal(&stackVal) && stackVal->IsString()) {
			v8::String::Utf8Value msgStr(isolate, message->Get());
			v8::String::Utf8Value stackStr(isolate, stackVal);
			L_ERROR << *msgStr << "\n"
							<< *stackStr;
			return;
		}
	}

	// Fallback: raw V8 stack trace
	v8::String::Utf8Value messageStr(isolate, message->Get());
	v8::String::Utf8Value errorStr(isolate, error);

	std::stringstream stack;
	auto stackTrace = message->GetStackTrace();
	if (!stackTrace.IsEmpty()) {
		for (int i = 0; i < stackTrace->GetFrameCount(); i++)
		{
			auto frame = stackTrace->GetFrame(isolate, i);
			v8::String::Utf8Value sourceStr(isolate, frame->GetScriptNameOrSourceURL());
			v8::String::Utf8Value functionStr(isolate, frame->GetFunctionName());
			stack << *sourceStr << "(" << frame->GetLineNumber() << "," << frame->GetColumn() << "): " << (*functionStr ? *functionStr : "") << "\n";
		}
	}

	L_ERROR << *messageStr << "\n"
					<< stack.str() << "\n"
					<< *errorStr;
}

namespace sampnode
{
	NodeImpl nodeImpl;
	bool NodeImpl::esmLoading;

	NodeImpl::NodeImpl() : nodeData(nullptr, node::FreeIsolateData) {}
	NodeImpl::~NodeImpl() {}

	void NodeImpl::Tick()
	{
		v8::Locker locker(v8Isolate);
		v8::Isolate::Scope isolateScope(v8Isolate);
		v8::HandleScope hs(v8Isolate);

		if (resource)
		{
			v8::Local<v8::Context> ctx = resource->GetContext().Get(v8Isolate);
			v8::Context::Scope contextScope(ctx);

			node::CallbackScope callbackScope(v8Isolate, resource->GetAsyncResource(v8Isolate), resource->GetAsyncContext());

			uv_run(nodeLoop->GetLoop(), UV_RUN_NOWAIT);
			v8Isolate->PerformMicrotaskCheckpoint();
		}
		v8Platform->DrainTasks(v8Isolate);
	}

	void NodeImpl::Initialize(const Props_t &config)
	{
		esmLoading = true;
		mainConfig = config;

		std::vector<std::string> args;
			args.push_back(""); // argv[0] executable path
			if (!config.node_flags.empty())
			{
				args.insert(args.end(), config.node_flags.begin(), config.node_flags.end());
			}
			else
			{
				const char* nodeEnv = std::getenv("NODE_ENV");
					if (nodeEnv && std::strcmp(nodeEnv, "development") == 0)
					{
						args.push_back("--enable-source-maps");
					}
			}

		for (auto &flag : args)
		{
			L_DEBUG << "node flags: " << flag;
		}

		auto result = node::InitializeOncePerProcess(args, {node::ProcessInitializationFlags::kNoInitializeV8,
																												node::ProcessInitializationFlags::kNoInitializeNodeV8Platform,
																												node::ProcessInitializationFlags::kNoInitOpenSSL});

		for (const std::string &error : result->errors())
		{
			L_ERROR << args[0] << ": " << error;
		}
		if (result->early_return() != 0)
		{
			L_ERROR << result->exit_code();
			return;
		}

		v8Platform = node::MultiIsolatePlatform::Create(4);
		v8::V8::InitializePlatform(v8Platform.get());
		v8::V8::Initialize();

		arrayBufferAllocator = node::ArrayBufferAllocator::Create();
		nodeLoop = std::make_unique<UvLoop>("mainNode");
		v8Isolate = node::NewIsolate(arrayBufferAllocator.get(), nodeLoop->GetLoop(), v8Platform.get());

		v8::Locker locker(v8Isolate);
		v8::Isolate::Scope isolateScope(v8Isolate);
		v8::HandleScope handle_scope(v8Isolate);

		v8Isolate->SetFatalErrorHandler([](const char *location, const char *message)
																		{
            L_ERROR << "at " << location << ": " << message;
            exit(1); });

		v8Isolate->SetCaptureStackTraceForUncaughtExceptions(true);
		v8Isolate->AddMessageListener(OnMessage);

		nodeData.reset(node::CreateIsolateData(v8Isolate, nodeLoop->GetLoop(), v8Platform.get(), arrayBufferAllocator.get()));
	}

	bool NodeImpl::LoadResource()
	{
		resource = std::make_shared<Resource>();
		resource->Init();

		while (esmLoading)
		{
			Tick();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		return true;
	}

	bool NodeImpl::UnloadResource()
	{
		if (!resource)
			return false;
		resource.reset();
		return true;
	}

	bool NodeImpl::ReloadResource()
	{
		return UnloadResource() && LoadResource();
	}

	void NodeImpl::Stop()
	{
		esmLoading = false;
		UnloadResource();

		v8Isolate->Dispose();
		v8Isolate = nullptr;

		arrayBufferAllocator = nullptr;
		nodeLoop = nullptr;

		node::FreeIsolateData(nodeData.release());
		node::FreePlatform(v8Platform.release());

		v8::V8::Dispose();
		v8::V8::DisposePlatform();

		node::TearDownOncePerProcess();
	}
}

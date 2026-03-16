#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "config.hpp"
#include "resource.hpp"
#include "nodeimpl.hpp"

void OnMessage(v8::Local<v8::Message> message, v8::Local<v8::Value> error)
{
	auto isolate = sampnode::nodeImpl.GetIsolate();
	v8::Locker locker(isolate);
	v8::Isolate::Scope isolateScope(isolate);
	v8::HandleScope handleScope(isolate);
	v8::String::Utf8Value messageStr(isolate, message->Get());
	v8::String::Utf8Value errorStr(isolate, error);

	std::stringstream stack;
	auto stackTrace = message->GetStackTrace();

	for (int i = 0; i < stackTrace->GetFrameCount(); i++)
	{
		auto frame = stackTrace->GetFrame(isolate, i);
		v8::String::Utf8Value sourceStr(isolate, frame->GetScriptNameOrSourceURL());
		v8::String::Utf8Value functionStr(isolate, frame->GetFunctionName());
		stack << *sourceStr << "(" << frame->GetLineNumber() << "," << frame->GetColumn() << "): " << (*functionStr ? *functionStr : "") << "\n";
	}

	L_ERROR << *messageStr << "\n"
					<< stack.str() << "\n"
					<< *errorStr;
}

namespace sampnode
{
	NodeImpl nodeImpl;
	std::unordered_map<node::Environment *, std::shared_ptr<Resource>> NodeImpl::resourcesPool;
	bool NodeImpl::esmLoading;

	NodeImpl::NodeImpl() : nodeData(nullptr, node::FreeIsolateData) {}
	NodeImpl::~NodeImpl() {}

	void NodeImpl::Tick()
	{
		v8::Locker locker(v8Isolate);
		v8::Isolate::Scope isolateScope(v8Isolate);
		v8::HandleScope hs(v8Isolate);

		for (auto &res : resourcesPool)
		{
			std::shared_ptr<Resource> resource = res.second;
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
		if (config.node_flags.empty())
		{
			args.push_back("");
		}
		else
		{
			args = config.node_flags;
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

	bool NodeImpl::LoadResource(const std::string &name)
	{
		auto resource = std::make_shared<Resource>(name);
		resource->Init();
		resourcesPool.emplace(resource->GetEnv(), resource);
		resourceNamesPool.emplace(name, resource->GetEnv());

		while (esmLoading)
		{
			Tick();
		}

		return true;
	}

	bool NodeImpl::UnloadResource(const std::string &name)
	{
		v8::Locker locker(v8Isolate);
		v8::Isolate::Scope isolateScope(v8Isolate);
		v8::HandleScope handleScope(v8Isolate);

		auto it = resourceNamesPool.find(name);
		if (it == resourceNamesPool.end())
			return false;

		resourcesPool.erase(it->second);
		resourceNamesPool.erase(it);
		return true;
	}

	bool NodeImpl::ReloadResource(const std::string &name)
	{
		return UnloadResource(name) && LoadResource(name);
	}

	void NodeImpl::Stop()
	{
		esmLoading = false;

		for (auto &resource : resourceNamesPool)
		{
			UnloadResource(resource.first);
		}

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
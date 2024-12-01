#include <cstring>
#include <unordered_map>
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

	printf("%s\n%s\n%s\n", *messageStr, stack.str().c_str(), *errorStr);
}

namespace sampnode
{
	NodeImpl nodeImpl;
	std::unordered_map<node::Environment*, std::shared_ptr<Resource>> NodeImpl::resourcesPool;

	NodeImpl::NodeImpl(): nodeData(nullptr, node::FreeIsolateData)
	{}

	NodeImpl::~NodeImpl()
	{}

	void NodeImpl::Tick()
	{
		v8::Locker locker(v8Isolate);
		v8::Isolate::Scope isolateScope(v8Isolate);
		v8::HandleScope hs(v8Isolate);

		for (auto& res : resourcesPool) {
			std::shared_ptr<Resource> resource = res.second;
			v8::Local<v8::Context> _context = resource->GetContext().Get(v8Isolate);

			v8::Context::Scope contextScope(_context);

			uv_run(nodeLoop->GetLoop(), UV_RUN_NOWAIT);
		}
	}

	void NodeImpl::Initialize(const Props_t& config)
	{
		mainConfig = config;
		
		std::vector<std::string> args{"--expose-internals", "--trace-uncaught", "--inspect", "--trace-warnings"};

		std::unique_ptr<node::InitializationResult> result = 
			node::InitializeOncePerProcess(args, {
				node::ProcessInitializationFlags::kNoInitializeV8,
				node::ProcessInitializationFlags::kNoInitializeNodeV8Platform,
				node::ProcessInitializationFlags::kNoInitOpenSSL
			});
		
		for (const std::string& error : result->errors())
			fprintf(stderr, "%s: %s\n", args[0].c_str(), error.c_str());
		if (result->early_return() != 0) {
			fprintf(stderr, "%d\n", result->exit_code());
			return;
		}

		v8Platform = node::MultiIsolatePlatform::Create(4);
		v8::V8::InitializePlatform(v8Platform.get());
		v8::V8::Initialize();

		arrayBufferAllocator = node::ArrayBufferAllocator::Create();

		nodeLoop = std::make_unique<UvLoop>("mainNode");

		v8Isolate = node::NewIsolate(arrayBufferAllocator.get(), nodeLoop->GetLoop(), v8Platform.get());

		{
			v8::Locker locker(v8Isolate);
			v8::Isolate::Scope isolateScope(v8Isolate);
			v8::HandleScope handle_scope(v8Isolate);
			v8Isolate->SetFatalErrorHandler([](const char* location, const char* message)
			{
				L_ERROR << "at " << location;
				L_ERROR << message;
				exit(1);
			});

			v8Isolate->SetCaptureStackTraceForUncaughtExceptions(true);
			v8Isolate->AddMessageListener(OnMessage);

			auto isolateData = node::CreateIsolateData(v8Isolate, nodeLoop->GetLoop(), v8Platform.get(), arrayBufferAllocator.get());

			nodeData.reset(isolateData);
		}
	}

	bool NodeImpl::LoadResource(const std::string& name)
	{
		std::shared_ptr<Resource> resource = std::make_shared<Resource>(name);
		resource->Init();
		resourcesPool.insert({ resource->GetEnv(), resource });
		resourceNamesPool.insert({ name, resource->GetEnv() });
		return true;
	}

	bool NodeImpl::UnloadResource(const std::string& name)
	{
		v8::Locker locker(v8Isolate);
		v8::Isolate::Scope isolateScope(v8Isolate);
		node::Environment* nodeEnv = resourceNamesPool[name];
		resourcesPool.erase(nodeEnv);
		resourceNamesPool.erase(name);
		return true;
	}

	bool NodeImpl::ReloadResource(const std::string& name)
	{
		return true;
	}

	void NodeImpl::Stop()
	{
		for (auto& resource : resourceNamesPool)
		{
			UnloadResource(resource.first);
		}
		node::FreeIsolateData(nodeData.get());
		v8::V8::Dispose();
		v8::V8::DisposePlatform();
		node::TearDownOncePerProcess();
	}
}
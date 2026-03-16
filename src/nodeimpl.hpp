#pragma once

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <memory>
#include "node.h"
#include "v8.h"
#include "uv.h"
#include "libplatform/libplatform.h"
#include "config.hpp"
#include "resource.hpp"
#include "uvloop.hpp"

namespace sampnode
{

	class NodeImpl
	{
	public:
		static std::unordered_map<node::Environment *, std::shared_ptr<Resource>> resourcesPool;
		static bool esmLoading;

		NodeImpl();
		~NodeImpl();

		void Initialize(const Props_t &config);
		bool LoadResource(const std::string &name);
		bool UnloadResource(const std::string &name);
		bool ReloadResource(const std::string &name);

		v8::Platform *GetPlatform() noexcept { return v8Platform.get(); }
		v8::Isolate *GetIsolate() noexcept { return v8Isolate; }
		node::IsolateData *GetNodeIsolate() noexcept { return nodeData.get(); }
		UvLoop *GetUVLoop() noexcept { return nodeLoop.get(); }
		Props_t &GetMainConfig() noexcept { return mainConfig; }

		void Tick();
		void Stop();

	private:
		v8::Isolate *v8Isolate;
		std::unique_ptr<node::IsolateData, decltype(&node::FreeIsolateData)> nodeData;
		std::unique_ptr<node::MultiIsolatePlatform> v8Platform;
		std::unique_ptr<node::ArrayBufferAllocator> arrayBufferAllocator;
		std::unique_ptr<UvLoop> nodeLoop;
		Props_t mainConfig;
		std::unordered_map<std::string, node::Environment *> resourceNamesPool;
	};

	extern NodeImpl nodeImpl;

}
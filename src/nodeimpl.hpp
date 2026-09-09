#pragma once

#include <memory>

#include "config.hpp"
#include "node.h"
#include "resource.hpp"
#include "uv.h"
#include "v8.h"

namespace sampnode {

class NodeImpl {
public:
  static bool esmLoading;

  NodeImpl();
  ~NodeImpl();

  void Initialize(const Props_t &config);
  bool LoadResource();
  bool UnloadResource();
  bool ReloadResource();

  v8::Platform *GetPlatform() noexcept { return v8Platform.get(); }
  v8::Isolate *GetIsolate() noexcept { return v8Isolate; }
  node::IsolateData *GetNodeIsolate() noexcept { return nodeData.get(); }
  Props_t &GetMainConfig() noexcept { return mainConfig; }

  void Tick();
  void Stop();

private:
  v8::Isolate *v8Isolate;
  std::unique_ptr<node::IsolateData, decltype(&node::FreeIsolateData)> nodeData;
  std::unique_ptr<node::MultiIsolatePlatform> v8Platform;
  std::unique_ptr<node::ArrayBufferAllocator> arrayBufferAllocator;
  uv_loop_t *uvLoop = nullptr;
  std::shared_ptr<Resource> resource;
  Props_t mainConfig;
};

extern NodeImpl nodeImpl;

} // namespace sampnode

#pragma once

#include "node.h"
#include "utils.hpp"
#include "uv.h"
#include "v8.h"

namespace sampnode {
class Resource {
public:
  Resource();
  ~Resource();

  void Init();
  void Stop();

  inline v8::UniquePersistent<v8::Context> &GetContext() { return context; }

  node::Environment *GetEnv() { return nodeEnvironment.get(); }

  node::async_context GetAsyncContext() { return asyncContext; }

  v8::Local<v8::Object> GetAsyncResource(v8::Isolate *isolate) {
    return asyncResource.Get(isolate);
  }

private:
  v8::UniquePersistent<v8::Context> context;
  std::unique_ptr<node::Environment, decltype(&node::FreeEnvironment)>
      nodeEnvironment;
  v8::UniquePersistent<v8::Object> asyncResource;
  node::async_context asyncContext{};
};
} // namespace sampnode

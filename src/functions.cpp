#include "functions.hpp"
#include "callbacks.hpp"
#include "common.hpp"
#include "config.hpp"
#include "events.hpp"
#include "natives.hpp"
#include "nodeimpl.hpp"
#include <string>
#include <utility>

static std::pair<std::string, v8::FunctionCallback>
    sampnodeSpecificFunctions[] = {
        {"on", sampnode::event::on},
        {"addEventListener", sampnode::event::on},
        {"addListener", sampnode::event::on},
        {"removeListener", sampnode::event::remove_listener},
        {"removeEventListener", sampnode::event::remove_listener},
        {"registerEvent", sampnode::event::register_event},
        {"callNative", sampnode::native::call},
        {"callNativeFloat", sampnode::native::call_float},
        {"callPublic", sampnode::callback::call},
        {"callPublicFloat", sampnode::callback::call_float},
        {"logprint", sampnode::functions::logprint}};

static void onESMLoaded(const v8::FunctionCallbackInfo<v8::Value> &info) {
  sampnode::nodeImpl.esmLoading = false;
}

namespace sampnode {
void functions::init(v8::Isolate *isolate,
                     v8::Local<v8::ObjectTemplate> &global) {

  Props_t &config = nodeImpl.GetMainConfig();

  v8::Locker locker(isolate);
  v8::Local<v8::ObjectTemplate> sampObject = v8::ObjectTemplate::New(isolate);
  for (auto &routine : sampnodeSpecificFunctions) {
    sampObject->Set(v8::String::NewFromUtf8(isolate, routine.first.c_str(),
                                            v8::NewStringType::kNormal)
                        .ToLocalChecked(),
                    v8::FunctionTemplate::New(isolate, routine.second));
  }

  global->Set(
      v8::String::NewFromUtf8(isolate, "samp", v8::NewStringType::kNormal)
          .ToLocalChecked(),
      sampObject);

  v8::Local<v8::ObjectTemplate> resourceObj = v8::ObjectTemplate::New(isolate);

  resourceObj->Set(
      v8::String::NewFromUtf8(isolate, "entryFile", v8::NewStringType::kNormal)
          .ToLocalChecked(),
      v8::String::NewFromUtf8(isolate, config.entry_file.c_str())
          .ToLocalChecked());

  global->Set(
      v8::String::NewFromUtf8(isolate, "__internal_resource").ToLocalChecked(),
      resourceObj);
  global->Set(
      v8::String::NewFromUtf8(isolate, "__internal_esmLoaded").ToLocalChecked(),
      v8::FunctionTemplate::New(isolate, &onESMLoaded));
}

void functions::logprint(const v8::FunctionCallbackInfo<v8::Value> &info) {
  if (info.Length() > 0) {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Locker locker(isolate);
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope scope(isolate);

    if (!info[0]->IsString())
      return;

    v8::String::Utf8Value _str(isolate, info[0]);

    LogLevel level = LogLevel::LOG_INFO;
    if (info.Length() > 1 && info[1]->IsNumber())
      level = static_cast<LogLevel>(
          info[1]->Int32Value(isolate->GetCurrentContext()).ToChecked());

    Log().Get(level) << *_str;
  }
}
} // namespace sampnode
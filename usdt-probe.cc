#include <napi.h>

#include "usdt.h"

using namespace Napi;

Napi::FunctionReference* USDTProbe::New = nullptr;

USDTProbe::USDTProbe(const CallbackInfo& info) : ObjectWrap<USDTProbe>(info) {
  argc = 0;
  probe = nullptr;
}

USDTProbe::~USDTProbe() {
  probe = nullptr;
}

Napi::Object USDTProbe::Init(Napi::Env env, Napi::Object exports) {
  Function func = DefineClass(env, "USDTProbe", {
      InstanceMethod("fire", &USDTProbe::Fire),
      });

  New = new FunctionReference();
  *New = Persistent(func);
  env.SetInstanceData(New);
  exports.Set("USDTProbe", func);
  return exports;
}

Napi::Value USDTProbe::Fire(const CallbackInfo& info) {
  auto env = info.Env();
  if (!info[0].IsFunction()) {
    Napi::TypeError::New(env, "Must give probe value callback as first argument")
      .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (probeIsEnabled(this->probe) == 0) {
    return env.Undefined();
  }

  size_t cblen = info.Length() - 1;

  Napi::Array cbargs = Napi::Array::New(env, cblen);

  for (size_t i = 0; i < cblen; i++) {
    cbargs[i] = info[i + 1];
  }

  Napi::Function cb = info[0].As<Napi::Function>();
  Napi::Value probe_args = cb.Call(env.Global(), {Napi::Number::New(env, cblen), cbargs});

  // exception in args callback?
  if (env.IsExceptionPending()) {
    Napi::Error::Fatal("USDTProbe::Fire", "Exception in callback");
    return env.Undefined();
  }

  if (argc > 0 && !probe_args.IsArray()) {
    return env.Undefined();
  }

  Napi::Array a = probe_args.As<Napi::Array>();
  void* argv[MAX_ARGUMENTS];

  // convert each argument value
  for (size_t i = 0; i < argc; i++) {
    if (a.Get(i).IsString()) {
      std::string argValue = a.Get(i).ToString().Utf8Value();
      // FIXME: Free string
      argv[i] = (void*)strdup(argValue.c_str());
    } else {
      argv[i] = (void*)a.Get(i).ToNumber().Uint32Value();
    }
  }

  // finally fire the probe
  switch (argc) {
    case 6:
      probeFire(this->probe, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
      break;
    case 5:
      probeFire(this->probe, argv[0], argv[1], argv[2], argv[3], argv[4]);
      break;
    case 4:
      probeFire(this->probe, argv[0], argv[1], argv[2], argv[3]);
      break;
    case 3:
      probeFire(this->probe, argv[0], argv[1], argv[2]);
      break;
    case 2:
      probeFire(this->probe, argv[0], argv[1]);
      break;
    case 1:
      probeFire(this->probe, argv[0]);
      break;
    case 0:
    default:
      probeFire(this->probe);
      break;
  }

  return Napi::Boolean::New(env, true);
}

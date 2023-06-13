#include <napi.h>

#include <stdio.h>
#include "usdt.h"
#include "v8.h"

using namespace Napi;

USDTProvider::USDTProvider(const CallbackInfo& info) : ObjectWrap<USDTProvider>(info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "Must give provider name as argument")
        .ThrowAsJavaScriptException();
    return;
  }

  std::string name = info[0].As<String>();
  provider = providerInit(name.c_str());
  // Unlikely?
  if (provider == NULL) {
    Error::Fatal("usdt-provider.cc:20", "providerInit failed");
    return;
  }
}

USDTProvider::~USDTProvider() {
  providerDestroy(provider);
}

Napi::Object USDTProvider::Init(Napi::Env env, Napi::Object exports) {
  Function func = DefineClass(env, "USDTProvider", {
      InstanceMethod("addProbe", &USDTProvider::AddProbe),
      InstanceMethod("enable", &USDTProvider::Enable),
  });

  FunctionReference* constructor = new FunctionReference();
  *constructor = Persistent(func);
  env.SetInstanceData(constructor);
  exports.Set("USDTProvider", func);
  return exports;
}

void USDTProvider::Enable(const Napi::CallbackInfo& info) {
  Napi::HandleScope scope(info.Env());

  if (providerLoad(this->provider) != 0) {
    Napi::Error::Fatal("USDTProvider::Enable", "Unable to load provider");
  }
}

Napi::Value USDTProvider::AddProbe(const Napi::CallbackInfo& info) {
  Napi::HandleScope scope(info.Env());

  if (info.Length() == 0) {
    Napi::TypeError::New(info.Env(), "Must give probe name as argument")
        .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  Napi::Value probeA = USDTProbe::New->New({});
  USDTProbe* probe = USDTProbe::Unwrap(probeA.As<Napi::Object>());
  probe->argc = 0;

  for (int i = 0; i < MAX_ARGUMENTS; i++) {
    if (i < info.Length() - 1) {
      std::string type = info[i + 1].ToString().Utf8Value();

      if (strncmp("char *", type.c_str(), 6) == 0) {
        probe->arguments[i] = uint64;
      } else if (strncmp("int", type.c_str(), 3) == 0) {
        probe->arguments[i] = int32;
      } else {
        probe->arguments[i] = uint64;
      }
      probe->argc++;
    }
  }

  std::string name(info[0].ToString().Utf8Value());

  switch (probe->argc) {
    case 6:
      probe->probe = providerAddProbe(provider, name.c_str(), probe->argc,
          probe->arguments[0],
          probe->arguments[1],
          probe->arguments[2],
          probe->arguments[3],
          probe->arguments[4],
          probe->arguments[5]
          );
      break;
    case 5:
      probe->probe = providerAddProbe(provider, name.c_str(), probe->argc,
          probe->arguments[0],
          probe->arguments[1],
          probe->arguments[2],
          probe->arguments[3],
          probe->arguments[4]
          );
      break;
    case 4:
      probe->probe = providerAddProbe(provider, name.c_str(), probe->argc,
          probe->arguments[0],
          probe->arguments[1],
          probe->arguments[2],
          probe->arguments[3]
          );
      break;
    case 3:
      probe->probe = providerAddProbe(provider, name.c_str(), probe->argc,
          probe->arguments[0],
          probe->arguments[1],
          probe->arguments[2]
          );
      break;
    case 2:
      probe->probe = providerAddProbe(provider, name.c_str(), probe->argc,
          probe->arguments[0],
          probe->arguments[1]
          );
      break;
    case 1:
      probe->probe = providerAddProbe(provider, name.c_str(), probe->argc,
          probe->arguments[0]
          );
      break;
    case 0:
    default:
      probe->probe = providerAddProbe(provider, name.c_str(), probe->argc);
      break;
  }

  return probeA;
}

Object Init(Env env, Object target) {
  USDTProvider::Init(env, target);
  USDTProbe::Init(env, target);
  return target;
}

NODE_API_MODULE(USDTProviderBindings, Init)

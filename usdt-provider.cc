#include <nan.h>

#include <iostream>
#include <stdio.h>
#include "usdt.h"

namespace node {
  using namespace v8;

  USDTProvider::USDTProvider() : Nan::ObjectWrap() {
    provider = NULL;
  }

  USDTProvider::~USDTProvider() {
    providerDestroy(provider);
  }

  Nan::Persistent<FunctionTemplate> USDTProvider::constructor_template;

  void USDTProvider::Initialize(v8::Local<Object> target) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(USDTProvider::New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(Nan::New<String>("USDTProvider").ToLocalChecked());
    constructor_template.Reset(t);

    Nan::SetPrototypeMethod(t, "addProbe", USDTProvider::AddProbe);
    // Nan::SetPrototypeMethod(t, "removeProbe", USDTProvider::RemoveProbe);
    Nan::SetPrototypeMethod(t, "enable", USDTProvider::Enable);
    Nan::SetPrototypeMethod(t, "disable", USDTProvider::Disable);
    // Nan::SetPrototypeMethod(t, "fire", USDTProvider::Fire);

    target->Set(Nan::New<String>("USDTProvider").ToLocalChecked(), t->GetFunction());

    USDTProbe::Initialize(target);
  }

  NAN_METHOD(USDTProvider::New) {
    Nan::HandleScope scope;
    USDTProvider *p = new USDTProvider();

    p->Wrap(info.This());

    if (info.Length() < 1 || !info[0]->IsString()) {
      Nan::ThrowTypeError("Must give provider name as argument");
      return;
    }

    String::Utf8Value name(info[0]->ToString());

    if ((p->provider = providerInit(*name)) == NULL) {
      Nan::ThrowError("providerInit failed");
      return;
    }

    info.GetReturnValue().Set(info.This());
  }

  NAN_METHOD(USDTProvider::AddProbe) {
    Nan::HandleScope scope;

    v8::Local<Object> obj = info.Holder();
    USDTProvider *provider = Nan::ObjectWrap::Unwrap<USDTProvider>(obj);

    // create a USDTProbe object
    v8::Local<Function> klass =
        Nan::New<FunctionTemplate>(USDTProbe::constructor_template)->GetFunction();
    v8::Local<Object> pd = klass->NewInstance();

    // store in provider object
    USDTProbe *probe = Nan::ObjectWrap::Unwrap<USDTProbe>(pd->ToObject());
    obj->Set(info[0]->ToString(), pd);

    // reference the provider to avoid GC'ing it when only probes remain in scope.
    Nan::ForceSet(pd, Nan::New<String>("__prov__").ToLocalChecked(), obj,
        static_cast<PropertyAttribute>(DontEnum | ReadOnly | DontDelete));

    // add probe to provider
    probe->argc = 0;
    for (int i = 0; i < MAX_ARGUMENTS; i++) {
      if (i < info.Length() - 1) {
        String::Utf8Value type(info[i + 1]->ToString());

        if (strncmp("char *", *type, 6) == 0) {
          probe->arguments[i] = uint64;
        }
        else if (strncmp("int", *type, 3) == 0) {
          probe->arguments[i] = int32;
        }
        else {
          probe->arguments[i] = uint64;
        }
        probe->argc++;
      }
    }

    String::Utf8Value name(info[0]->ToString());

    switch (probe->argc) {
      case 6:
        probe->probe = providerAddProbe(provider->provider, *name, probe->argc,
          probe->arguments[0],
          probe->arguments[1],
          probe->arguments[2],
          probe->arguments[3],
          probe->arguments[4],
          probe->arguments[5]
        );
        break;
      case 5:
        probe->probe = providerAddProbe(provider->provider, *name, probe->argc,
          probe->arguments[0],
          probe->arguments[1],
          probe->arguments[2],
          probe->arguments[3],
          probe->arguments[4]
        );
        break;
      case 4:
        probe->probe = providerAddProbe(provider->provider, *name, probe->argc,
          probe->arguments[0],
          probe->arguments[1],
          probe->arguments[2],
          probe->arguments[3]
        );
        break;
      case 3:
        probe->probe = providerAddProbe(provider->provider, *name, probe->argc,
          probe->arguments[0],
          probe->arguments[1],
          probe->arguments[2]
        );
        break;
      case 2:
        probe->probe = providerAddProbe(provider->provider, *name, probe->argc,
          probe->arguments[0],
          probe->arguments[1]
        );
        break;
      case 1:
        probe->probe = providerAddProbe(provider->provider, *name, probe->argc,
          probe->arguments[0]
        );
        break;
      case 0:
      default:
        probe->probe = providerAddProbe(provider->provider, *name, probe->argc);
        break;
    }

    info.GetReturnValue().Set(pd);
  }

  NAN_METHOD(USDTProvider::Enable) {
    Nan::HandleScope scope;
    USDTProvider *provider = Nan::ObjectWrap::Unwrap<USDTProvider>(info.Holder());

    if (providerLoad(provider->provider) != 0) {
      // TODO (mmarchini) get error string from libstapsdt
      Nan::ThrowError("Unable to load provider");
      return;
    }

    return;
  }

  NAN_METHOD(USDTProvider::Disable) {
    Nan::HandleScope scope;
    USDTProvider *provider = Nan::ObjectWrap::Unwrap<USDTProvider>(info.Holder());

    if (providerUnload(provider->provider) != 0) {
      Nan::ThrowError("Unable to unload provider");
      return;
    }

    return;
  }

  extern "C" void
  init(v8::Local<Object> target) {
    USDTProvider::Initialize(target);
  }

  NODE_MODULE(USDTProviderBindings, init)
}

#include <iostream>
#include <nan.h>
#include "usdt.h"

namespace node {

  using namespace v8;

  USDTProbe::USDTProbe() : Nan::ObjectWrap() {
    argc = 0;
    probe = NULL;
  }

  USDTProbe::~USDTProbe() {
    // XXX (mmarchini) probe is cleaned by the provider
    probe = NULL;
  }

  Nan::Persistent<FunctionTemplate> USDTProbe::constructor_template;

  void USDTProbe::Initialize(v8::Local<Object> target) {
    Nan::HandleScope scope;

    Local<FunctionTemplate> t = Nan::New<FunctionTemplate>(USDTProbe::New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(Nan::New<String>("USDTProbe").ToLocalChecked());
    constructor_template.Reset(t);

    Nan::SetPrototypeMethod(t, "fire", USDTProbe::Fire);

    target->Set(Nan::New<String>("USDTProbe").ToLocalChecked(), t->GetFunction());
  }

  NAN_METHOD(USDTProbe::New) {
    Nan::HandleScope scope;
    USDTProbe *probe = new USDTProbe();
    probe->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }

  NAN_METHOD(USDTProbe::Fire) {
    Nan::HandleScope scope;

    if (!info[0]->IsFunction()) {
      Nan::ThrowTypeError("Must give probe value callback as first argument");
      return;
    }

    USDTProbe *pd = Nan::ObjectWrap::Unwrap<USDTProbe>(info.Holder());
    info.GetReturnValue().Set(pd->_fire(info, 0));
  }

  v8::Local<Value> USDTProbe::_fire(Nan::NAN_METHOD_ARGS_TYPE argsinfo, size_t fnidx) {
    Nan::HandleScope scope;

    if (probeIsEnabled(this->probe) == 0) {
      return Nan::Undefined();
    }

    // invoke fire callback
    Nan::TryCatch try_catch;

    size_t cblen = argsinfo.Length() - fnidx - 1;
    Local<Value> *cbargs = new Local<Value>[cblen];

    for (size_t i = 0; i < cblen; i++) {
        cbargs[i] = argsinfo[i + fnidx + 1];
    }

    Local<Function> cb = Local<Function>::Cast(argsinfo[fnidx]);
    Local<Value> probe_args = cb->Call(this->handle(), cblen, cbargs);

    delete [] cbargs;

    // exception in args callback?
    if (try_catch.HasCaught()) {
      Nan::FatalException(try_catch);
      return Nan::Undefined();
    }

    // check return
    if (!probe_args->IsArray()) {
      return Nan::Undefined();
    }

    // TODO (mmarchini) check if array size and probe args size is equal

    Local<Array> a = Local<Array>::Cast(probe_args);
    void *argv[MAX_ARGUMENTS];

    // convert each argument value
    for (size_t i = 0; i < argc; i++) {
      if(a->Get(i)->IsString() ){
        // FIXME (mmarchini) free string
        argv[i] = (void *) strdup(*(static_cast<String::Utf8Value> (a->Get(i)->ToString())));
      } else {
        argv[i] = (void *)((**(a->Get(i))).Uint32Value());
      }
    }

    // finally fire the probe
    switch(argc) {
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

    return Nan::True();
  }

} // namespace node

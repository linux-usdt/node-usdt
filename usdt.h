#include <nan.h>
#include <node_object_wrap.h>

extern "C" {
#include <libstapsdt.h>
}

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h>

namespace node {

using namespace v8;

class USDTProbe : public Nan::ObjectWrap {

public:
  static void Initialize(v8::Local<v8::Object> target);
  SDTProbe_t *probe;
  ArgType_t arguments[MAX_ARGUMENTS];
  size_t argc;

  static NAN_METHOD(New);
  static NAN_METHOD(Fire);

  v8::Local<Value> _fire(Nan::NAN_METHOD_ARGS_TYPE, size_t);

  static Nan::Persistent<FunctionTemplate> constructor_template;

  USDTProbe();
  ~USDTProbe();
private:
};

class USDTProvider : public Nan::ObjectWrap {

public:
  static void Initialize(v8::Local<v8::Object> target);
  SDTProvider_t *provider;

  static NAN_METHOD(New);
  static NAN_METHOD(AddProbe);
  // static NAN_METHOD(RemoveProbe);
  static NAN_METHOD(Enable);
  static NAN_METHOD(Disable);
  // static NAN_METHOD(Fire);

  USDTProvider();
  ~USDTProvider();
private:
  static Nan::Persistent<FunctionTemplate> constructor_template;
};

void InitUSDTProvider(v8::Local<v8::Object> target);

};

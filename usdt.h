#include <napi.h>

extern "C" {
#include <libstapsdt.h>
}

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h>

using Napi::CallbackInfo;

class USDTProbe : public Napi::ObjectWrap<USDTProbe> {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object target);
    static Napi::FunctionReference* New;

    Napi::Value Fire(const Napi::CallbackInfo& info);

    USDTProbe(const Napi::CallbackInfo& info);
    ~USDTProbe();

    SDTProbe_t *probe;
    ArgType_t arguments[MAX_ARGUMENTS];
    size_t argc;
};

class USDTProvider : public Napi::ObjectWrap<USDTProvider> {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object object);

    Napi::Value AddProbe(const Napi::CallbackInfo& info);
    void Enable(const Napi::CallbackInfo& info);

    USDTProvider(const Napi::CallbackInfo& info);
    ~USDTProvider();
    SDTProvider_t *provider;
};

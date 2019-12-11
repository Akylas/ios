#ifndef Runtime_h
#define Runtime_h

#include "libplatform/libplatform.h"
#include "Common.h"
#include "ModuleInternal.h"
#include "MetadataBuilder.h"

namespace tns {

class Runtime {
public:
    Runtime();
    ~Runtime();
    void Init();
    void RunMainScript();
    void RunScript(std::string file, v8::TryCatch& tc);
    v8::Isolate* GetIsolate();

    const int WorkerId();

    void SetWorkerId(int workerId);

    void RunModule(const std::string moduleName);

    static void Initialize();

    static Runtime* GetCurrentRuntime() {
        return currentRuntime_;
    }

    static bool IsWorker() {
        if (currentRuntime_ == nullptr) {
            return false;
        }

        return currentRuntime_->WorkerId() > 0;
    }

    static v8::Platform* GetPlatform() {
        return platform_;
    }

    static id GetAppConfigValue(std::string key);
private:
    static thread_local Runtime* currentRuntime_;
    static v8::Platform* platform_;
    static bool mainThreadInitialized_;

    void DefineGlobalObject(v8::Local<v8::Context> context);
    void DefineCollectFunction(v8::Local<v8::Context> context);
    void DefineNativeScriptVersion(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> globalTemplate);
    void DefinePerformanceObject(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> globalTemplate);
    void DefineTimeMethod(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> globalTemplate);
    static void PerformanceNowCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Isolate* isolate_;
    ModuleInternal moduleInternal_;
    int workerId_;
};

}

#endif /* Runtime_h */

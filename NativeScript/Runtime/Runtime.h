#ifndef Runtime_h
#define Runtime_h

#include "libplatform/libplatform.h"
#include "Common.h"
#include "ModuleInternal.h"
#include "MetadataBuilder.h"

namespace tns {

class Runtime {
public:
    static void InitializeMetadata(void* metadataPtr);
    Runtime();
    void Init(const std::string& baseDir);
    void Init(const std::string& baseDir, const std::string& script);
    void RunScript(std::string file);
    v8::Isolate* GetIsolate();
private:
    static bool mainThreadInitialized_;
    static v8::Platform* platform_;

    void InitInternal(const std::string& baseDir);
    void DefineGlobalObject(v8::Local<v8::Context> context);
    void DefineNativeScriptVersion(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> globalTemplate);
    void DefinePerformanceObject(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> globalTemplate);
    void DefineTimeMethod(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> globalTemplate);
    static void PerformanceNowCallback(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Isolate* isolate_;
    MetadataBuilder metadataBuilder_;
    ModuleInternal moduleInternal_;
    std::string baseDir_;
};

}

#endif /* Runtime_h */

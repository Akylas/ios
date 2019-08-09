#ifndef Reference_h
#define Reference_h

#include "DataWrapper.h"
#include "Common.h"

namespace tns {

class Reference {
public:
    static void Register(v8::Isolate* isolate, v8::Local<v8::Object> interop);
    static v8::Local<v8::Function> GetInteropReferenceCtorFunc(v8::Isolate* isolate);
    static void* GetWrappedPointer(v8::Isolate* isolate, v8::Local<v8::Value> reference);
private:
    static v8::Local<v8::Value> GetReferredValue(v8::Isolate* isolate, v8::Local<v8::Value> value);
    static void ReferenceConstructorCallback(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void IndexedPropertySetCallback(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info);

    static void GetValueCallback(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetValueCallback(v8::Local<v8::Name> name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void RegisterToStringMethod(v8::Isolate* isolate, v8::Local<v8::Object> prototype);
};

}

#endif /* Reference_h */

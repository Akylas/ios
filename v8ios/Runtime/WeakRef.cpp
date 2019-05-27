#include "WeakRef.h"
#include "ArgConverter.h"
#include "Helpers.h"

using namespace v8;

namespace tns {

void WeakRef::Init(Isolate* isolate) {
    Local<Context> context = isolate->GetCurrentContext();
    Local<Object> global = context->Global();

    Local<FunctionTemplate> weakRefCtorFuncTemplate = FunctionTemplate::New(isolate, ConstructorCallback, Local<Value>());

    Local<v8::Function> weakRefCtorFunc;
    if (!weakRefCtorFuncTemplate->GetFunction(context).ToLocal(&weakRefCtorFunc)) {
        assert(false);
    }

    Local<v8::String> name = tns::ToV8String(isolate, "WeakRef");
    weakRefCtorFunc->SetName(name);
    global->Set(name, weakRefCtorFunc);
}

void WeakRef::ConstructorCallback(const FunctionCallbackInfo<Value>& info) {
    assert(info.IsConstructCall());
    assert(info.Length() == 1);
    Local<Value> target = info[0];
    assert(target->IsObject());

    Isolate* isolate = info.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    Local<Object> targetObj = target.As<Object>();

    Local<Object> weakRef = ArgConverter::CreateEmptyObject(context);

    Persistent<Object>* poTarget = new Persistent<Object>(isolate, targetObj);
    Persistent<Object>* poHolder = new Persistent<Object>(isolate, weakRef);
    CallbackState* callbackState = new CallbackState(poTarget, poHolder);

    poTarget->SetWeak(callbackState, WeakTargetCallback, WeakCallbackType::kFinalizer);
    poHolder->SetWeak(callbackState, WeakHolderCallback, WeakCallbackType::kFinalizer);

    weakRef->Set(tns::ToV8String(isolate, "get"), GetGetterFunction(isolate));
    weakRef->Set(tns::ToV8String(isolate, "clear"), GetClearFunction(isolate));
    tns::SetPrivateValue(isolate, weakRef, tns::ToV8String(isolate, "target"), External::New(isolate, poTarget));

    info.GetReturnValue().Set(weakRef);
}

void WeakRef::WeakTargetCallback(const WeakCallbackInfo<CallbackState>& data) {\
    CallbackState* callbackState = data.GetParameter();
    Persistent<Object>* poTarget = callbackState->target_;
    poTarget->Reset();
    delete poTarget;
    callbackState->target_ = nullptr;

    Isolate* isolate = data.GetIsolate();
    Persistent<Object>* poHolder = callbackState->holder_;
    if (poHolder != nullptr) {
        Local<Object> holder = poHolder->Get(isolate);
        tns::SetPrivateValue(isolate, holder, tns::ToV8String(isolate, "target"), External::New(isolate, nullptr));
    }

    if (callbackState->holder_ == nullptr) {
        delete callbackState;
    }
}

void WeakRef::WeakHolderCallback(const WeakCallbackInfo<CallbackState>& data) {
    CallbackState* callbackState = data.GetParameter();
    Persistent<Object>* poHolder = callbackState->holder_;
    Isolate* isolate = data.GetIsolate();
    Local<Object> holder = Local<Object>::New(isolate, *poHolder);

    Local<Value> hiddenVal = tns::GetPrivateValue(isolate, holder, tns::ToV8String(isolate, "target"));
    Persistent<Object>* poTarget = reinterpret_cast<Persistent<Object>*>(hiddenVal.As<External>()->Value());

    if (poTarget != nullptr) {
        poHolder->SetWeak(callbackState, WeakHolderCallback, WeakCallbackType::kFinalizer);
    } else {
        poHolder->Reset();
        delete poHolder;
        callbackState->holder_ = nullptr;
        if (callbackState->target_ == nullptr) {
            delete callbackState;
        }
    }
}

Local<v8::Function> WeakRef::GetGetterFunction(Isolate* isolate) {
    if (poGetterFunc_ != nullptr) {
        return poGetterFunc_->Get(isolate);
    }

    Local<Context> context = isolate->GetCurrentContext();
    Local<v8::Function> getterFunc = FunctionTemplate::New(isolate, GetCallback)->GetFunction(context).ToLocalChecked();
    poGetterFunc_ = new Persistent<v8::Function>(isolate, getterFunc);
    return getterFunc;
}

Local<v8::Function> WeakRef::GetClearFunction(Isolate* isolate) {
    if (poClearFunc_ != nullptr) {
        return poClearFunc_->Get(isolate);
    }

    Local<Context> context = isolate->GetCurrentContext();
    Local<v8::Function> clearFunc = FunctionTemplate::New(isolate, ClearCallback)->GetFunction(context).ToLocalChecked();
    poClearFunc_ = new Persistent<v8::Function>(isolate, clearFunc);
    return clearFunc;
}

void WeakRef::GetCallback(const FunctionCallbackInfo<Value>& info) {
    Local<Object> holder = info.This();
    Isolate* isolate = info.GetIsolate();
    Local<Value> hiddenVal = tns::GetPrivateValue(isolate, holder, tns::ToV8String(isolate, "target"));
    Persistent<Object>* poTarget = reinterpret_cast<Persistent<Object>*>(hiddenVal.As<External>()->Value());

    if (poTarget != nullptr) {
        Local<Object> target = poTarget->Get(isolate);
        info.GetReturnValue().Set(target);
    } else {
        info.GetReturnValue().Set(Null(isolate));
    }
}

void WeakRef::ClearCallback(const FunctionCallbackInfo<Value>& info) {
    Local<Object> holder = info.This();
    Isolate* isolate = info.GetIsolate();
    tns::SetPrivateValue(isolate, holder, tns::ToV8String(isolate, "target"), External::New(isolate, nullptr));
}

Persistent<v8::Function>* WeakRef::poGetterFunc_;
Persistent<v8::Function>* WeakRef::poClearFunc_;

}
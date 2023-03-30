#ifndef Common_h
#define Common_h

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#include "v8.h"

// FIXME: Move these to a secondary header?
#include "v8-cppgc.h"
#include "cppgc/allocation.h"
#pragma clang diagnostic pop

namespace tns {

#define ALLOCATION_HANDLE_FOR_ISOLATE(isolate) (isolate->GetCppHeap()->GetAllocationHandle())

template <typename T, typename... Args>
T* MakeGarbageCollected(v8::Isolate* isolate, Args&&... args) {
    return cppgc::MakeGarbageCollected<T>(ALLOCATION_HANDLE_FOR_ISOLATE(isolate), std::forward<Args>(args)...);
}

}

#endif /* Common_h */

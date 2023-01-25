#pragma once

#include <mutex>
#include <atomic>

using BYTE = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;


// atomic, mutex 
template<class T>
using Atomic = std::atomic<T>;
using Mutex = std::mutex;
using CondVal = std::condition_variable;
using UlockG = std::unique_lock<std::mutex>;
using lockG = std::lock_guard<std::mutex>;
#pragma once
#include <iostream>
// 일반적인 레퍼런스 카운트
class RefCountable
{
public:
	RefCountable(): _refCnt(1) { }
	virtual  ~RefCountable() { }
	int32 AddRef() { return ++_refCnt; }
	int32 ReleaseRef() 
	{
		int32 ref = --_refCnt;
		if (ref == 0)
		{
			delete this;
		}
		return ref;
	}
	int GetRef() { return _refCnt; }
protected:
	Atomic<int32> _refCnt;
};

// 직접 스마트 포인터 구현
template <class T>
class SharedPtr 
{
public:
	SharedPtr() {}
	SharedPtr(T* obj) { Set(obj); };
	~SharedPtr() { Release(); };

	//복사
	SharedPtr(const SharedPtr& rhs) { Set(rhs._obj); }
	//이동
	SharedPtr(SharedPtr&& rhs) { _obj = rhs._obj; rhs = nullptr; }
	// 상속 복사 부모 자식 업 다운 캐스팅 가능
	template <class U>
	SharedPtr(const SharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._obj)); }
// 연산자
	//복사 대입
	SharedPtr& operator=(const SharedPtr<T>& rhs)
	{
		
		if (_obj != rhs._obj)
		{
			
			Release(); // 3->2
			Set(rhs._obj);
		}
		return *this;
		
	}
	// 이동 대입
	 SharedPtr& operator=(SharedPtr&& rhs)
	{
		 std::cout << typeid(this).name() << std::endl;
		Release();
		_obj = rhs._obj;
		rhs._obj = nullptr;

		return *this;
	}

	bool	 operator==(const SharedPtr& rhs) const { return (_obj == rhs._obj); }
	bool	 operator==(T* robj) const { return (_obj == robj); }
	bool	 operator!=(const SharedPtr& rhs) const { return (_obj != rhs._obj); }
	bool	 operator!=(T* robj) const { return (_obj != robj); }
	bool	 operator<(const SharedPtr& rhs) { return (_obj < rhs._obj); }
	
	T*		 operator* () { return _obj; }
	const T* operator* () const { return _obj; };
		     operator T* () const { return _obj; }
	T*		 operator->() { return _obj; }
	const T* operator->() const { return _obj; }

	bool IsNull() { return (_obj == nullptr); }
// ======= 기능 ========
private:
	inline void Set(T* obj)
	{
		_obj = obj; // ref 2
		if(_obj != nullptr) _obj->AddRef();
		
	}

	inline void Release()
	{
		if (_obj == nullptr) return;
		_obj->ReleaseRef();
		_obj = nullptr;
	}
private:
	T* _obj = nullptr;
};
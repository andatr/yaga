#ifndef YAGA_COMMON_AUTO_DELETER
#define YAGA_COMMON_AUTO_DELETER

#include <functional>
#include <boost/core/noncopyable.hpp>

#include "Exception.h"

namespace yaga
{
	// smart pointer for not a pointer base class T
	// can't use std smart pointers for this
	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	class AutoDeleter : private boost::noncopyable
	{
	public:
		typedef std::function<T()> ConstructorT;
		typedef std::function<void(T)> DestructorT;
	public:
		AutoDeleter();
		~AutoDeleter();
		void Construct(const ConstructorT& ctor, const DestructorT& dtor);
		void Reset();
		const T& Get() const;
		bool Destoyed() const { return _destoyed; }
	private:
		bool _destoyed;
		T _object;
		DestructorT _destructor;
	};

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	AutoDeleter<T>::AutoDeleter():
		_destoyed(true)
	{
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void AutoDeleter<T>::Construct(const ConstructorT& ctor, const DestructorT& dtor)
	{
		if (!_destoyed) {
			THROW("AutoDeleter: attempt to construct object twice")
		}
		_destructor = dtor;
		_object = std::move(ctor());
		_destoyed = false;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	AutoDeleter<T>::~AutoDeleter()
	{
		Reset();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void AutoDeleter<T>::Reset()
	{
		if (_destoyed) return;
		_destoyed = true;
		_destructor(_object);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	const T& AutoDeleter<T>::Get() const
	{
		if (_destoyed) {
			THROW("AutoDeleter: attempt to access destoyed object")
		}
		return _object;
	}
}

#endif // !YAGA_COMMON_AUTO_DELETER
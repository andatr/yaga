#ifndef YAGA_COMMON_SMART_DESTRUCTOR
#define YAGA_COMMON_SMART_DESTRUCTOR

#include <functional>
#include <boost/core/noncopyable.hpp>

#include "Exception.h"

namespace yaga
{
	// smart pointer for not a pointer base class T
	// can't use std smart pointers for this
	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	class SmartDestructor : private boost::noncopyable
	{
	public:
		typedef std::function<T()> ConstructorT;
		typedef std::function<void(T)> DestructorT;
	public:
		SmartDestructor();
		~SmartDestructor();
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
	SmartDestructor<T>::SmartDestructor():
		_destoyed(true)
	{
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void SmartDestructor<T>::Construct(const ConstructorT& ctor, const DestructorT& dtor)
	{
		if (!_destoyed) {
			THROW("SmartDestructor: attempt to construct object twice")
		}
		_destructor = dtor;
		_object = std::move(ctor());
		_destoyed = false;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	SmartDestructor<T>::~SmartDestructor()
	{
		Reset();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void SmartDestructor<T>::Reset()
	{
		if (_destoyed) return;
		_destoyed = true;
		_destructor(_object);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	const T& SmartDestructor<T>::Get() const
	{
		if (_destoyed) {
			THROW("SmartDestructor: attempt to access destoyed object")
		}
		return _object;
	}
}

#endif // !YAGA_COMMON_SMART_DESTRUCTOR
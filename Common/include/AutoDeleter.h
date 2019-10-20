#ifndef YAGA_COMMON_AUTO_DELETER
#define YAGA_COMMON_AUTO_DELETER

#include <functional>
#include <boost/core/noncopyable.hpp>

#include "Exception.h"

namespace yaga
{
	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	class AutoDeleter : private boost::noncopyable
	{
	public:
		typedef std::function<void(T)> DestructorT;
	public:
		AutoDeleter();
		AutoDeleter(AutoDeleter<T>&& other) noexcept;
		AutoDeleter<T>& operator=(AutoDeleter<T>&& other) noexcept;
		~AutoDeleter();
		void Assign(T& obj, const DestructorT& dtor);
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
	AutoDeleter<T>::AutoDeleter(AutoDeleter<T>&& other) noexcept
	{
		_destoyed = other._destoyed;
		_destructor = other._destructor;
		_object = std::move(other._object);
		other._destoyed = true;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	AutoDeleter<T>& AutoDeleter<T>::operator=(AutoDeleter<T>&& other) noexcept
	{
		_destoyed = other._destoyed;
		_destructor = other._destructor;
		_object = std::move(other._object);
		other._destoyed = true;
		return *this;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void AutoDeleter<T>::Assign(T& obj, const DestructorT& dtor)
	{
		if (!_destoyed) {
			THROW("AutoDeleter: attempt to construct object twice");
		}
		_destructor = dtor;
		_object = std::move(obj);
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
			THROW("AutoDeleter: attempt to access destoyed object");
		}
		return _object;
	}
}

#endif // !YAGA_COMMON_AUTO_DELETER
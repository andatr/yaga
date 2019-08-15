#ifndef YAGA_COMMON_VECTOR
#define YAGA_COMMON_VECTOR

namespace yaga
{
	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	class Vector
	{
	public:
		Vector();
		~AutoDeleter();
		void Construct(const ConstructorT& ctor, const DestructorT& dtor);
		void Reset();
		const T* Data() const;
		bool Destoyed() const { return _destoyed; }
	private:
		std::unique_ptr<T> _data;
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

#endif // !YAGA_COMMON_VECTOR
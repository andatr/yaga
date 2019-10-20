#ifndef YAGA_COMMON_ARRAY
#define YAGA_COMMON_ARRAY

namespace yaga
{
	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	class Array
	{
	public:
		explicit Array(size_t size = 0);
		void Resize(size_t size);
		T* operator*() const { return _data.get(); }
		T* Data() const { return _data.get(); }
		T& operator[](size_t i) { return _data.get()[i]; }
		size_t Size() const { return _size; }
	private:
		static void Delete(const T* p);
	private:
		size_t _size;
		std::shared_ptr<T> _data;
	};

	typedef Array<char> ByteArray;

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Array<T>::Array(size_t size) : _size(size), _data(size > 0 ? new T[size] : nullptr, Delete)
	{
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void Array<T>::Resize(size_t size)
	{
		_size = size;
		_data = std::shared_ptr<T>(size > 0 ? new T[size] : nullptr, Delete);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T>
	void Array<T>::Delete(const T* p)
	{
		delete[] p;
	}
}

#endif // !YAGA_COMMON_ARRAY
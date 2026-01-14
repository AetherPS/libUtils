#pragma once

#define JUMP_32SIZE 5
#define JUMP_64SIZE 14
#define CALL_32SIZE JUMP_32SIZE

class BaseDetour
{
public:
	void* StubPtr;

	virtual void Create(void* address, void* destination) = 0;
	virtual void Restore() {};

	template <typename result, typename... Args>
	result Invoke(Args... args)
	{
		result(*f)(Args... args) = decltype(f)(StubPtr);
		return f(args...);
	}

	virtual inline ~BaseDetour()
	{
		Restore();
	}

protected:
	bool IsRelativelyClose(void* address1, void* address2);
	uintptr_t GetRelativeAddress(void* address, void* destination);
	void WriteJump32(void* address, void* destination);
	void WriteJump64(void* address, void* destination);
	void WriteCall32(void* address, void* destination);

	size_t StubSize;
	void* Address;
	std::unique_ptr<uint8_t[]> OriginalBytes;
	size_t OriginalSize;
	bool DetourSet;
};

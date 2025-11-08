#pragma once

class Detour32 : public BaseDetour
{
public:
	template<class T>
	Detour32(void* address, T(*function))
	{
		Create(address, reinterpret_cast<void*>(function));
	}

	template<class T>
	Detour32(Detour32** inst, void* address, T(*function))
	{
		*inst = this;
		Create(address, reinterpret_cast<void*>(function));
	}

	template<class T>
	Detour32(uintptr_t address, T(*function))
	{
		Create(reinterpret_cast<void*>(address), reinterpret_cast<void*>(function));
	}

	template<class T>
	Detour32(Detour32** inst, uintptr_t address, T(*function))
	{
		*inst = this;
		Create(reinterpret_cast<void*>(address), reinterpret_cast<void*>(function));
	}

	~Detour32() { Restore(); }

	virtual void Create(void* address, void* destination) override;
	virtual void Restore() override;

private:
	void* TrampolinePtr;
};
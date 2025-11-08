#pragma once
#include "BaseDetour.h"

class DetourManager
{
public:
	DetourManager() {};
	~DetourManager();

	template<class D, class T>
	void AddDetour(std::string name, uintptr_t address, T(*function))
	{
		DetourList.insert(std::make_pair(name, nullptr));
		new D((D**)&DetourList[name], address, function);
	}

	template <typename result, typename... Args>
	result Invoke(std::string name, Args... args)
	{
		return DetourList[name]->Invoke<result>(args...);
	}

private:
	std::map<std::string, BaseDetour*> DetourList;
};
#pragma once
#include <string>

#include "../../../../ext/jni/jni.h"
#include "../mapper/Mapper.h"

class VersionDetector
{
private: 
	
public:

	static GameVersions GetVersion();

	static std::string GetClientName();
};
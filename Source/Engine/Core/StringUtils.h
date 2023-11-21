#pragma once
#include <string>
#include <ctype.h>

namespace nc {
	class StringUtils {

	public:
		std::string static ToUpper(const std::string& s);
		std::string static ToLower(const std::string& s);
		bool static IsEqualIgnoreCase(const std::string& s1, const std::string& s2);
		std::string static CreateUnique(const std::string& s);

	public:
	};
}
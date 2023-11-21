#include "StringUtils.h"

namespace nc {
	std::string StringUtils::ToUpper(const std::string& s) {
		//move to a string I can manipulate
		std::string result = s;

		//loop through the new string
		for (char& c : result)
		{
			//calls the to upper function for each char
			c = std::toupper(c);
		}

		//returns the changed string
		return result;
	}

	std::string StringUtils::ToLower(const std::string& s) {
		//move to a string I can manipulate
		std::string result = s;

		//loop through the new string
		for (char& c : result)
		{
			//calls the to lower function for each char
			c = std::tolower(c);
		}

		//returns the changed string
		return result;
	}

	bool StringUtils::IsEqualIgnoreCase(const std::string& s1, const std::string& s2) {
		//seeing if the string are even the same size
		if (!(s1.size() == s2.size())) return false;

		//converting each string to an uppercase version (using my to upper function)
		std::string r1 = ToUpper(s1);
		std::string r2 = ToUpper(s2);


		//doing a loop that is the size of the first string
		for (int i = 0; i < r1.size(); i++) {
			//seeing if each char is the same as the other
			if (r1[i] != r2[i]) return false;
		}
		return true;
	}

	std::string StringUtils::CreateUnique(const std::string& s)
	{
		static uint32_t unique = 0;

		return s + std::to_string(unique++);
	}
}
#include <ice/Utility.hpp>

namespace ice {
	std::string Format(const std::string& format, const std::vector<std::string>& arguments) {
		std::string result;

		std::size_t begin = 0;
		std::size_t percent;
		std::size_t index = 0;

		while ((percent = format.find('%', begin)) != std::string::npos) {
			result.insert(result.end(), format.begin() + begin, format.begin() + percent);
			if (percent + 1 < format.size() && format[percent + 1] == '%') {
				result.push_back('%');
				begin = percent + 2;
			} else {
				result.append(arguments[index++]);
				begin = percent + 1;
			}
		}

		if (begin < format.size()) {
			result.insert(result.end(), format.begin() + begin, format.end());
		}

		return result;
	}
}
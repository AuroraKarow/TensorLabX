#ifndef __Logger__
#define __Logger__

#include <iostream>
#include <filesystem>
#include <fstream>

namespace Tool {
	enum class OutputMode {
		Console,
		File,
	};

	/*static*/ class Logger {
	private:
	OutputMode mode;
	std::string fileName;
	std::unique_ptr<std::fstream> outFile = nullptr;

public:
	Logger();
	Logger(OutputMode mode);
	~Logger();

	void Debug(std::string info);
	void Log(std::string info);
	};
}
#endif // !__Logger__

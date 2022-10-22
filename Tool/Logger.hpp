#ifndef __Logger__
#define __Logger__

#include <iostream>

namespace Tool {
	class Logger {
	private:
	public:
		Logger() {}
		~Logger() {}

		void Debug(std::string info) {
#ifdef _DEBUG
			Log(info);
#endif
		}
        void Log(std::string info)
        {
            std::cout << info << std::endl;
        }
    };
}

#endif // !__Logger__

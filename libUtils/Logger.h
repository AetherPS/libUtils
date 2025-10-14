#pragma once

class Logger
{
public:
	enum LoggingLevels
	{
		LogLevelNone = 0,
		LogLevelError,
		LogLevelWarn,
		LogLevelAll,
	};

	enum LogType
	{
		LogTypeError = 1,
		LogTypeWarn,
		LogTypeInfo,
	};

	static void Init(bool useKlog, LoggingLevels loggingLevel, bool showTimeStamp = true)
	{
		UseKlog = useKlog;
		LoggingLevel = loggingLevel;
		ShowTimestamp = showTimeStamp;
	}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
	template<class...Args>
	static void Log(LogType level, const char* fmt, Args&&...args)
	{
		if (LoggingLevel < level)
			return;

		// lock the mutex
		std::unique_lock<std::mutex> lock(LoggingMtx);

		if (UseKlog)
			klog(ShowTimestamp ? va("[%s]%s", GetTimeNow().c_str(), fmt).c_str() : fmt, std::forward<Args>(args)...);
		else
			printf(ShowTimestamp ? va("[%s]%s", GetTimeNow().c_str(), fmt).c_str() : fmt, std::forward<Args>(args)...);
	}
#pragma clang diagnostic pop

	template<class...Args>
	static void Info(const char* fmt, Args&&...args)
	{
		Log(LogTypeInfo, va("[\u001b[94mInfo\u001b[0m] %s\n", fmt).c_str(), std::forward<Args>(args)...);
	}

	template<class...Args>
	static void Success(const char* fmt, Args&&...args)
	{
		Log(LogTypeInfo, va("[\u001b[92mSuccess\u001b[0m] %s\n", fmt).c_str(), std::forward<Args>(args)...);
	}

	template<class...Args>
	static void Warn(const char* fmt, Args&&...args)
	{
		Log(LogTypeWarn, va("[\u001b[33mWarn\u001b[0m] %s\n", fmt).c_str(), std::forward<Args>(args)...);
	}

	template<class...Args>
	static void Error(const char* fmt, Args&&...args)
	{
		Log(LogTypeError, va("[\u001b[31mError\u001b[0m] %s\n", fmt).c_str(), std::forward<Args>(args)...);
	}

private:
	static inline bool ShowTimestamp;
	static inline int LoggingLevel;
	static inline bool UseKlog;
	static inline std::mutex LoggingMtx;
};
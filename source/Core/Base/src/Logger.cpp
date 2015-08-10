#include <BaseHeaders.h>
#include <Logger.h>
#include <ApplicationContext.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(LogManager);LoggerBase ::LoggerBase(const char* _name) :
		name(_name) {
}

Logger::Logger(const char* _name, bool log2file) :
		LoggerBase(_name), logtofile(log2file), lastsev(LOG_MESSAGE) {
	if (logtofile) {
		String filename = name + ".log";
		output.open(filename.c_str());
		real_out = &output;
	} else
		real_out = &std::cerr;

	time_t tsec;
	time(&tsec);
#ifdef NEX_MSVC
	struct tm vt;
	struct tm *t = &vt;
	localtime_s(&vt, &tsec);
	char asct[32];
	asctime_s(asct,t);
#else
	struct tm* t = localtime(&tsec);
	const char* asct = asctime(t);
#endif
	(*real_out) << "Log opened on..." << asct << std::endl;
}

Logger::~Logger() {

	time_t tsec;
	time(&tsec);
#ifdef NEX_MSVC
	struct tm vt;
	struct tm *t = &vt;
	localtime_s(&vt, &tsec);
	char asct[32];
	asctime_s(asct,t);
#else
	struct tm* t = localtime(&tsec);
	const char* asct = asctime(t);
#endif
	(*real_out) << "\nLog closed on..." << asct << std::endl;
	if (output.is_open())
		output.close();
}

void Logger::AddListener(nextar::LogListener *listener) {

	listeners.push_back(listener);
}

void Logger::RemoveListener(nextar::LogListener *listener) {
	BestErase(listeners, listener); 
}

void Logger::LogMsg(nextar::LogSeverity logSeverity, const String& intf,
		const nextar::String &msg) {
#ifdef NEX_DEBUG
	OutStringStream fakestream;
	std::ostream* old = real_out;
	real_out = &fakestream;
#endif
	bool postInfo = false;
	if (lastintf != intf) {
		lastintf = intf;
		for (size_t i = 0; i < listeners.size(); ++i) {
			listeners[i]->SetInterface(intf);
		}
		(*real_out) << std::endl << intf << ": " << std::endl;
		postInfo = true;
	}

	//if (lastsev != logSeverity || postInfo) {
	switch (logSeverity) {
	case LOG_ERROR:
		(*real_out) << "[Error]        ";
		break;
	case LOG_WARNING:
		(*real_out) << "[Warning]      ";
		break;
	case LOG_MESSAGE:
		(*real_out) << "[Information]  ";
		break;
	case LOG_DEBUG:
		(*real_out) << "[Debug]        ";
		break;
	}
	lastsev = logSeverity;
	//} else
	//	(*real_out) << "               ";

	(*real_out) << msg << std::endl;
#ifdef NEX_DEBUG
	String rmsg = fakestream.str();
	real_out = old;
	(*real_out) << rmsg;
	(*real_out).flush();
#ifndef NEX_GCC
	Platform::OutputDebug(rmsg.c_str());
#endif

#endif
	for (size_t i = 0; i < listeners.size(); ++i) {
		listeners[i]->LogMsg(logSeverity, msg);
	}
}

LogManager::LogManager() :
		defaultLogger(0) {
}

LogManager::~LogManager() {
}

void LogManager::AddNewLogger(LoggerBase* ptr, bool defaultlog) {
	NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
	loggers.push_back(ptr);
	if (defaultlog || !defaultLogger)
		defaultLogger = ptr;
}

LoggerBase* LogManager::FindLog(const char *name) {
	NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
	String tmpname = name;
	for (LoggerBaseList::iterator i = loggers.begin(); i != loggers.end();
			++i) {
		if ((*i)->GetName() == tmpname)
			return (*i);
	}
	return 0;
}

void LogManager::LogMsg(nextar::LogSeverity logSeverity,
		const nextar::String &intf, const nextar::String &msg) {
	//LoggerBase* bp = GetSingleton().defaultLogger;
	LogManager& logManager = LogManager::Instance();
	NEX_THREAD_LOCK_GUARD_MUTEX(logManager.accessLock);
	LoggerBaseList& loggers = logManager.loggers;
	for (LoggerBaseList::iterator i = loggers.begin(); i != loggers.end();
			++i) {
		(*i)->LogMsg(logSeverity, intf, msg);
	}
}

void LogManager::RemoveLogger(nextar::LoggerBase* logger) {
	NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
	BestErase(loggers, logger);
}
}


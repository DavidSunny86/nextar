#ifndef NEXTAR_NEXREPORT_H__
#define NEXTAR_NEXREPORT_H__

/*!
 *  \file Logger.h
 *  @author Abhishek Dey
 *  @brief Nextar reporting support.
 *
 *   Nextar errors are dumped into std streams. The streams can be redirected
 *   to output windows.
 *   The basic stream that nextar uses to dump errors is defined by the:
 *   \n \a Logger
 */
#include <Singleton.h>
#include <NexThread.h>
#include <RefPtr.h>

namespace nextar {
class LoggerBase;
class Logger;
class LogListener;

/*!
 * \enum LogSeverity
 * @brief This directly corresponds to exceptions severity,
 *        providing information on the severity of the log.
 *
 */
enum LogSeverity {
	LOG_ERROR, LOG_WARNING, LOG_MESSAGE, LOG_DEBUG,
// debug information
};

/*!
 * @class LogListener
 * @brief This class can be used to listen to any log events.
 *
 * \attention This class does not support virtual dtor, so it is adviced
 *  not to delete a pointer of this class.
 *
 */
class _NexNoVtable LogListener {
public:

	/**
	 * @brief	Sets the interface currently posting this log.
	 *          This is called prior to posting the actual message,
	 *          in case there is a new interface posting the message.
	 *          However if it is the same old interface, it is not called.
	 *
	 * @param	intf	The interface.
	 **/
	virtual void SetInterface(const String& intf) = 0;
	/*!
	 * @brief This is posted by the logger;
	 * @param logString Log string type.
	 * @param logSeverity Log severity.
	 * @param msg Log message.
	 */
	virtual void LogMsg(LogSeverity logSeverity, const String& msg) = 0;
protected:
	~LogListener() {
	}
};

class _NexBaseAPI LoggerBase {
protected:
	String name;
public:
	LoggerBase(const char* name);

	virtual ~LoggerBase() {
	}

	_NexInline const String& GetName() const {
		return name;
	}

	virtual void LogMsg(LogSeverity logSeverity, const String& intf,
			const String& msg) = 0;
};

/*!
 * @class Logger
 * @brief This class offers std loggin services. This is the default implementation.
 *
 *  This class can redirect output from a perticualar stream to itself.
 *  Mostly logging is done to std::ostream.
 */
class _NexBaseAPI Logger: public LoggerBase, public AllocGeneral {
public:
	typedef vector<LogListener*>::type LogListenerList;

protected:
	bool logtofile;
	std::ofstream output;
	std::ostream* real_out;
	LogSeverity lastsev;
	String lastintf;
	LogListenerList listeners;

public:

	_NexInline bool IsLoggingToFile() const {
		return logtofile;
	}

	Logger(const char* name = 0, bool log2file = false);
	virtual ~Logger();
	/*!
	 * @brief Add a log listener
	 * @param listener The listener to Add
	 */
	void AddListener(LogListener* listener);
	/*!
	 * @brief Remove a log listener
	 * @param listener The listener to remove
	 */
	void RemoveListener(LogListener* listener);
	/*!
	 * @brief Log message
	 * @param logSeverity The log level
	 * @param msg The log message
	 */
	virtual void LogMsg(LogSeverity logSeverity, const String& intf,
			const String& msg);

public:
};

/**
 * @class LogManager
 *
 * @brief Manager for logs.
 *
 * @author Abhishek Dey
 * @date 10/29/2009
 **/
class _NexBaseAPI LogManager: public Singleton<LogManager>, public AllocGeneral {
	typedef vector<LoggerBase*>::type LoggerBaseList;
	LoggerBaseList loggers;
	LoggerBase* defaultLogger;NEX_THREAD_MUTEX(accessLock);

public:
	LogManager();
	~LogManager();
	/**
	 * @remarks	Add a new logger.
	 **/
	void AddNewLogger(LoggerBase*, bool defaultlog = false);

	/**
	 * @brief Searches for the logger with specified name.
	 *
	 * @author Abhishek Dey
	 * @date 10/29/2009
	 *
	 * @param [in,out] name If non-null, the name.
	 *
	 * @return null if it fails, else the found log.
	 **/
	LoggerBase* FindLog(const char* name);

	/**
	 * @brief	Removes this logger from manager.
	 *
	 * @author	Abhishek Dey
	 * @date	11/1/2009
	 *
	 * @param [in]	logger	If non-null, the logger.
	 **/
	void RemoveLogger(LoggerBase* logger);

	/**
	 * @brief Logs a message to the default logger.
	 *
	 * @author Abhishek Dey
	 * @date 10/29/2009
	 *
	 * @param logSeverity The log severity.
	 * @param msg 		   The message.
	 **/
	static void LogMsg(LogSeverity logSeverity, const String& intf,
			const String& msg);
};

#define NEX_LOG_HELPER(T)                                                               \
		static void LogMsg(nextar::LogSeverity logSeverity,const nextar::String& msg)	{       \
			LogManager::LogMsg(logSeverity, nextar::String( NEX_MAKE_TEXT(T) ),msg);        \
		}                                                                       \
		static void Warn(const nextar::String& msg) {                                   \
			LogMsg(nextar::LOG_WARNING,msg);                                        \
		}                                                                       \
		static void Trace(const nextar::String& msg) {                                  \
			LogMsg(nextar::LOG_MESSAGE,msg);                                        \
		}                                                                       \
		static void Error(const nextar::String& msg) {                                  \
			LogMsg(nextar::LOG_ERROR,msg);                                          \
		}                                                                       \
		static void Debug(const nextar::String& msg) {                                  \
			LogMsg(nextar::LOG_DEBUG,msg);                                          \
		}                                                                       \

}

#endif //NEXTAR_NEXREPORT_H__

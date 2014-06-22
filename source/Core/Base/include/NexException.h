#ifndef NEX_NEXEXCEPT_H
#define NEX_NEXEXCEPT_H

#include <exception>

/*! \file NexException.h
 *  @author Abhishek Dey
 *  @brief Nextar exception and handling.
 * 
 *  Most errors in nextar are handled by exceptions, exceptions are of 3 types:
 *  \n 1. \a FatalErrors which are unrecoverable.
 *  \n 2. \a GracefulError which are recoverable but still errors.
 *  \n 3. \a Warning which are not errors but are potential problems.
 */

// todo C++0x changs this to noexcept
#define NEX_NOTHROW() throw()
#define NEX_THROW_ON(failure,except) if(failure) throw except

namespace nextar {

/*! \enum ErrorSeverity
 *
 *   @brief The error depth.
 *
 *   The 3 error classes.
 */
enum ErrorSeverity {
	ES_FATAL, ES_GRACEFUL, ES_WARNING,
};

/*! @class Exception
 *
 *   @brief Base class for exceptions.
 *
 *   This class is derived by the 3 exception classes.
 *   Any other exception class must not derive from here rather,
 *   should use other exception classes.
 */
class _NexBaseAPI Exception : public std::exception {
	uint32 errorCode; /**< The error code, used to identify the error. */
	ErrorSeverity severity;
	const char* file;
	const char* function;
	uint32 line;
public:

	/** @brief Default constructor.
	 *   @param eCode The error code.
	 *
	 *   This is the default constructor
	 */
	_NexInline Exception(ErrorSeverity s,
			uint32 eCode,
			const char* file,
			const char* func,
			uint32 line) :
	severity(s),
	errorCode(eCode) {
		this->file = file;
		this->function = func;
		this->line = line;
	}
	virtual ~Exception() NEX_NOTHROW() {
	}

	/** @brief Get severity.
	 *   @return The error severity.
	 *
	 */
	_NexInline ErrorSeverity GetSeverity() const {
		return severity;
	}

	/** @brief get error code.
	 *   @return The error code.
	 *
	 */
	_NexInline uint32 GetErrorCode() const {
		return errorCode;
	}

	/** @brief Get location and details of exception
	 *   @return The file,function and line location.
	 *
	 */
	virtual const char* GetMsg() const;
};

/*! @class FatalError
 *
 *   @brief Base class for fatal exceptions.
 *
 */
class _NexBaseAPI FatalErrorExcept : public Exception {
public:

	/** @brief Constructor.
	 *   @param errorCode The error code.
	 *   @param iId Interface id that reported the problem.
	 *
	 *   This constructor will accept interface information.
	 *
	 */
	_NexInline FatalErrorExcept(uint32 errorCode,
			const char* file,
			const char* func,
			uint32 line) : Exception(ES_FATAL, errorCode, file, func, line) {
	}

};
#define NEX_THROW_FatalError(errorCode)					\
	throw nextar::FatalErrorExcept(errorCode,				\
	NEX_SOURCEFILE_NAME,NEX_FUNCTION_NAME,NEX_SOURCEFILE_LINE)		\
	/*! @class GracefulError
	*
	*   @brief Base class for graceful exceptions.
	*   
	*/

class _NexBaseAPI GracefulErrorExcept : public Exception {
public:

	/** @brief Constructor.
	 *   @param errorCode The error code.
	 *   @param iId Interface id that reported the problem.
	 *
	 *   This constructor will accept interface information.
	 *
	 */
	_NexInline GracefulErrorExcept(uint32 errorCode,
			const char* file,
			const char* func,
			uint32 line) : Exception(ES_GRACEFUL, errorCode, file, func, line) {
	}
};
#define NEX_THROW_GracefulError(errorCode)					\
	throw nextar::GracefulErrorExcept(errorCode,           		\
	NEX_SOURCEFILE_NAME,NEX_FUNCTION_NAME,NEX_SOURCEFILE_LINE)		\
	/*! @class Warning
	*
	*   @brief Base class for warnings.
	*   
	*/

class _NexBaseAPI WarningExcept : public Exception {
public:

	/** @brief Constructor.
	 *   @param errorCode The error code.
	 *   @param iId Interface id that reported the problem.
	 *
	 *   This constructor will accept interface information.
	 *
	 */
	_NexInline WarningExcept(uint32 errorCode,
			const char* file,
			const char* func,
			uint32 line) : Exception(ES_WARNING, errorCode, file, func, line) {
	}
};
#define NEX_THROW_Warning(errorCode)						\
	throw nextar::WarningExcept(errorCode, 				\
	NEX_SOURCEFILE_NAME,NEX_FUNCTION_NAME,NEX_SOURCEFILE_LINE)		\

}

#endif //NEX_NEXEXCEPT_H
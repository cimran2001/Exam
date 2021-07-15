#include "Exceptions.h"

Exception::Exception (const char* reason) : reason(reason) {
}

const char* Exception::what() {
	return reason;
}

OutOfRangeException::OutOfRangeException(const char* reason) : Exception(reason) {
}

MaxQuantityLimitException::MaxQuantityLimitException(const char* reason) : Exception(reason) {
}

NotAFinishedQuestionException::NotAFinishedQuestionException(const char* reason) : Exception(reason) {
}

FileNotLoadedException::FileNotLoadedException(const char* reason) : Exception(reason) {
}

InvalidPasswordException::InvalidPasswordException(const char* reason) : Exception(reason) {
}

InvalidUsernameException::InvalidUsernameException(const char* reason) : Exception(reason) {
}

TestImportFailureException::TestImportFailureException(const char* reason) : Exception(reason) {
}

NotFoundException::NotFoundException(const char* reason) : Exception(reason) {
}

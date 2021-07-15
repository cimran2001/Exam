#pragma once

/*

Created on 05.07.2021

*/

class Exception {
private:
	const char* reason;
public: 
	Exception(const char* reason);

	const char* what();
};

class OutOfRangeException : public Exception {
public:
	OutOfRangeException(const char* reason);
};

class MaxQuantityLimitException : public Exception {
public:
	MaxQuantityLimitException(const char* reason);
};

class NotAFinishedQuestionException : public Exception {
public:
	NotAFinishedQuestionException(const char* reason);
};

class FileNotLoadedException : public Exception {
public:
	FileNotLoadedException(const char* reason);
};

class InvalidPasswordException : public Exception {
public:
	InvalidPasswordException(const char* reason);
};

class InvalidUsernameException : public Exception {
public:
	InvalidUsernameException(const char* reason);
};

class TestImportFailureException : public Exception {
public:
	TestImportFailureException(const char* reason);
};

class NotFoundException : public Exception {
public:
	NotFoundException(const char* reason);
};
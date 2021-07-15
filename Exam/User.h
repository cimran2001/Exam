#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <sstream>
#include "FileProcess.h"
#include "Test.h"

#define USERNAME_MAX_LEN 256
#define PHONE_NUMBER_MAX_LEN 32
#define NAME_MAX_LEN 256


class User {
private:
	bool admin;
	size_t password;
	char username[USERNAME_MAX_LEN];
	char number[PHONE_NUMBER_MAX_LEN];
	char name[NAME_MAX_LEN];
public:
	User(const std::string& username, const std::string& password, const std::string& name, const std::string& number, bool admin = false);

	void setUsername(const std::string& username);
	std::string getUsername() const;

	void setName(const std::string& name);
	std::string getName() const;

	void setNumber(const std::string& number);
	std::string getNumber() const;

	void setPassword(const std::string& newPassword);
	size_t getPasswordHash() const;

	bool isAdmin() const;
};


class UsersHandler {
private:
	std::string filename;
	std::vector<User> allUsers;
public:
	UsersHandler(const std::string& filename);

	void loadUsers();

	void saveUsers();

	size_t usersCount() const;

	User& operator[](size_t index);

	User* checkUserCredentials(const std::string& username, const std::string& password);

	void registerUser(const User& newUser);

	void removeUser(const std::string& username, const std::string& password);

	void removeUser(size_t index);

	bool freeUsername(const std::string& username);
	
	std::vector<User>& getUsers();
};

class UserController {
private:
	User* currentUser;

	std::string userTestsFilename;
public:
	UserController();

	UserController(User* user);

	User* getCurrentUser();

	size_t getCompletedTestsCount() const;

	size_t getCompletedTestsCountBySubject(TestsHandler& testsHandler, const std::string& subject) const;

	void solveTest(const Test& test);

	std::vector<TestController> getNonFinishedTests() const;

	void setCurrentUser(User* user);
};

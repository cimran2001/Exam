#include "User.h"

User::User(const std::string& username, const std::string& password, const std::string& name, const std::string& number, bool admin) : \
	password(std::hash<std::string>{}(password)), admin(admin) {
	strncpy_s(this->username, username.c_str(), USERNAME_MAX_LEN);
	strncpy_s(this->name, name.c_str(), NAME_MAX_LEN);
	strncpy_s(this->number, number.c_str(), PHONE_NUMBER_MAX_LEN);
}

void User::setUsername(const std::string& username) {
	strncpy_s(this->username, username.c_str(), USERNAME_MAX_LEN);
}

std::string User::getUsername() const {
	return std::string{ username };
}

std::string User::getName() const {
	return std::string{ name };
}

std::string User::getNumber() const {
	return std::string{ number };
}

size_t User::getPasswordHash() const {
	return password;
}

bool User::isAdmin() const {
	return admin;
}

void User::setPassword(const std::string& newPassword) {
	password = std::hash<std::string>{}(newPassword);
}

void User::setNumber(const std::string& number) {
	strncpy_s(this->number, number.c_str(), PHONE_NUMBER_MAX_LEN);
}

void User::setName(const std::string& name) {
	strncpy_s(this->name, name.c_str(), NAME_MAX_LEN);
}

UsersHandler::UsersHandler(const std::string& filename) : filename(filename) {
}

void UsersHandler::loadUsers() {
	try {
		file_process::readFromFileBinary(filename, allUsers);
	}
	catch (FileNotLoadedException& e) {
		std::cout << e.what() << std::endl;
	}

}

void UsersHandler::saveUsers() {
	try {
		file_process::writeToFileBinary(filename, allUsers);
	} 
	catch (FileNotLoadedException& e) {
		std::cout << e.what() << std::endl;
	}
}

size_t UsersHandler::usersCount() const {
	return allUsers.size();
}

User& UsersHandler::operator[](size_t index) {
	if (index >= allUsers.size())
		throw OutOfRangeException{ "No user with such an index." };

	return allUsers[index];
}

User* UsersHandler::checkUserCredentials(const std::string& username, const std::string& password) {
	size_t passwordHash = std::hash<std::string>{}(password);

	for (User& user : allUsers)
		if (username == user.getUsername() && passwordHash == user.getPasswordHash())
			return &user;

	return nullptr;
}

void UsersHandler::registerUser(const User& newUser) {
	for (User& user : allUsers)
		if (user.getUsername() == newUser.getUsername())
			throw InvalidUsernameException{ "This username is already taken. Try another." };

	allUsers.push_back(newUser);
}

void UsersHandler::removeUser(const std::string& username, const std::string& password) {
	size_t passwordHash = std::hash<std::string>{}(password);

	for (size_t i = 0; i < allUsers.size(); i++) {
		if (allUsers[i].getUsername() != username)
			continue;

		if (allUsers[i].getPasswordHash() != passwordHash)
			throw InvalidPasswordException{ "The password entered is incorrect. Couldn't remove the user" };

		allUsers.erase(allUsers.begin() + i);
		return;
	}

	throw InvalidUsernameException{ "The username is incorrect." };
}

void UsersHandler::removeUser(size_t index) {
	if (index >= allUsers.size())
		throw OutOfRangeException{ "No user with such an index." };

	allUsers.erase(allUsers.begin() + index);
}

bool UsersHandler::freeUsername(const std::string& username) {
	for (const User& user : allUsers)
		if (user.getUsername() == username)
			return false;

	return true;
}

std::vector<User>& UsersHandler::getUsers() {
	return allUsers;
}

UserController::UserController(User* user) : currentUser(user) {
	std::stringstream ss;
	ss << std::hash<std::string>{}(user->getUsername()) << ".bin";

	userTestsFilename = ss.str();
}

UserController::UserController() : currentUser(nullptr) {
}

User* UserController::getCurrentUser() {
	return currentUser;
}

size_t UserController::getCompletedTestsCount() const {
	size_t testControllerSize = sizeof(TestController);
	size_t testControllersCount = file_process::fileSize(userTestsFilename) / testControllerSize;
	
	std::ios::openmode mode = std::ios::in | std::ios::binary;
	std::ifstream input{ userTestsFilename, mode };
	
	if (!input.is_open())
		throw FileNotLoadedException{ "Couldn't load tests for this user." };

	size_t completedTestsCount = 0;

	char* temp = new char[testControllerSize];
	for (size_t _ = 0; _ < testControllersCount; _++) {
		input.read(temp, testControllerSize);
		if (reinterpret_cast<TestController*>(temp)->isSolved())
			completedTestsCount++;
	}

	delete[] temp;
	input.close();

	return completedTestsCount;
}

size_t UserController::getCompletedTestsCountBySubject(TestsHandler& testsHandler, const std::string& subject) const {
	size_t testControllerSize = sizeof(TestController);
	size_t testControllersCount = file_process::fileSize(userTestsFilename) / testControllerSize;

	std::ios::openmode mode = std::ios::in | std::ios::binary;
	std::ifstream input{ userTestsFilename, mode };

	if (!input.is_open())
		throw FileNotLoadedException{ "Couldn't load tests for this user." };

	size_t completedTestsCountBySubject = 0;

	char* temp = new char[testControllerSize];
	for (size_t _ = 0; _ < testControllersCount; _++) {
		input.read(temp, testControllerSize);
		if (reinterpret_cast<TestController*>(temp)->getTest().getSubject() == subject)
			completedTestsCountBySubject++;
	}

	delete[] temp;
	input.close();

	return completedTestsCountBySubject;
}

void UserController::solveTest(const Test& test) {
	TestController* testController = new TestController{ test };

	testController->solveTest();

	std::ios::openmode mode = std::ios::out | std::ios::app | std::ios::binary;
	std::ofstream output(userTestsFilename, mode);

	if (!output.is_open())
		throw FileNotLoadedException{ "Couldn't save test to the file." };

	output.write(reinterpret_cast<char*>(testController), sizeof(TestController));
	output.close();

	delete testController;
}

std::vector<TestController> UserController::getNonFinishedTests() const {
	std::vector<TestController> nonFinished;

	const size_t testControllerSize = sizeof(TestController);
	size_t testsCount = file_process::fileSize(userTestsFilename) / testControllerSize;

	if (testsCount == 0)
		return nonFinished;

	std::ios::openmode mode = std::ios::in | std::ios::binary;
	std::ifstream input{ userTestsFilename, mode };

	if (!input.is_open())
		throw FileNotLoadedException{ "Couldn't load started tests." };

	char* tempArray = new char[testControllerSize];
	for (size_t _ = 0; _ < testsCount; _++) {
		input.read(tempArray, testControllerSize);
		if (!reinterpret_cast<TestController*>(tempArray)->isSolved())
			nonFinished.push_back(*reinterpret_cast<TestController*>(tempArray));
	}

	delete[] tempArray;

	return nonFinished;
}

void UserController::setCurrentUser(User* user) {
	currentUser = user;

	std::stringstream ss;
	ss << std::hash<std::string>{}(user->getUsername()) << ".bin";

	userTestsFilename = ss.str();
}

#include <iostream>
#include <string>
#include "Test.h"
#include "User.h"

const std::string usersFilename = "users.bin";
const std::string testsFilename = "tests.bin";

void registerUser(UsersHandler& usersHandler, bool admin);
void changeUsername(UsersHandler& usersHandler, User* user);
void changePassword(UsersHandler& usersHandler, User* user);
void changeName(UsersHandler& usersHandler, User* user);
void changeNumber(UsersHandler& usersHandler, User* user);
void modifyUser(UsersHandler& usersHandler);
void removeUser(UsersHandler& usersHandler);
size_t getAllCompletedTestsCount(UsersHandler& usersHandler);
size_t getAllCompletedTestsCountBySubject(UsersHandler& usersHandler, TestsHandler& testsHandler, const std::string& subject);
void addTest(TestsHandler& testsHandler);
void modifyTest(TestsHandler& testsHandler);
void removeTest(TestsHandler& testsHandler);

int main() {
	UsersHandler usersHandler{ usersFilename };
	usersHandler.loadUsers();

	TestsHandler testsHandler{ testsFilename };
	testsHandler.loadTests();

	if (usersHandler.usersCount() == 0) {
		std::cout << "\t\tADMIN REGISTRATION!" << std::endl;
		registerUser(usersHandler, true);
	}

	UserController userController;
	
	std::cout << "\t\tLOGIN PAGE" << std::endl;
	do {
		std::string username, password;

		std::cout << "Enter your username: ";
		std::getline(std::cin, username);

		std::cout << "Enter your password: ";
		std::getline(std::cin, password);

		userController.setCurrentUser(usersHandler.checkUserCredentials(username, password));
		if (userController.getCurrentUser() == nullptr)
			std::cout << "Invalid credentials. Try to log in again. " << std::endl;
	} while (userController.getCurrentUser() == nullptr);
	
	size_t mode = 12;
	if (userController.getCurrentUser()->isAdmin()) {
		do {
			std::cout << "\t1. Change username" << std::endl
				<< "\t2. Change password" << std::endl
				<< "\t3. Create a user" << std::endl
				<< "\t4. Modify a user" << std::endl
				<< "\t5. Remove a user" << std::endl
				<< "\t6. Number of completed tests" << std::endl
				<< "\t7. Number of completed tests by subject" << std::endl
				<< "\t8. Add a test" << std::endl
				<< "\t9. Import a test" << std::endl
				<< "\t10. Modify a test" << std::endl
				<< "\t11. Remove a test" << std::endl
				<< "\t12. Go to user mode" << std::endl
				<< "\t0. Exit" << std::endl;

			std::cout << "Select a mode: ";
			std::cin >> mode;
			std::cin.ignore();

			switch (mode) {
			case 1:
				changeUsername(usersHandler, userController.getCurrentUser());
				break;
			case 2:
				changePassword(usersHandler, userController.getCurrentUser());
				break;
			case 3:
				registerUser(usersHandler, false);
				break;
			case 4:
				modifyUser(usersHandler);
				break;
			case 5:
				removeUser(usersHandler);
				break;
			case 6:
				std::cout << "The number of all completed tests is " << getAllCompletedTestsCount(usersHandler) << "." << std::endl;
				break;
			case 7: {
				std::string subject;
				std::cout << "Enter the name of the subject: ";
				std::getline(std::cin, subject);

				std::cout << "The number of all completed tests by this subject is " <<
					getAllCompletedTestsCountBySubject(usersHandler, testsHandler, subject) << "." << std::endl;
				break;
			}
			case 8:
				addTest(testsHandler);
				break;
			case 9: {
				std::string stringTemp;

				std::cout << "Enter the name of the file with a test: ";
				std::getline(std::cin, stringTemp);

				testsHandler.importTest(stringTemp);
				break;
			}
			case 10:
				modifyTest(testsHandler);
				break;
			case 11:
				removeTest(testsHandler);
				break;
			case 12:
			case 0:
				break;
			default:
				std::cout << "Not a correct number. Try again. " << std::endl;
				break;
			}
		} while (mode && mode != 12);
	}
	
	if (mode == 12) {
		do {
			std::cout << "\t1. Show statictics" << std::endl
				<< "\t2. Pass a new test" << std::endl
				<< "\t3. Continue a non-finished test" << std::endl
				<< "\t4. Change username" << std::endl
				<< "\t5. Change password" << std::endl
				<< "\t6. Change name" << std::endl
				<< "\t7. Change phone number" << std::endl
				<< "\t0. Exit" << std::endl;

			std::cout << "Select a mode: ";
			std::cin >> mode;

			switch (mode) {
			case 1: {
				size_t wholeStatistics;

				do {
					std::cout << "To show the whole statictics or by subject (1/0)? ";
					std::cin >> wholeStatistics;

					if (wholeStatistics > 1)
						std::cout << "Not a correct number. Try again. " << std::endl;
				} while (wholeStatistics > 1);

				if (wholeStatistics) {
					std::cout << "The number of completed tests - ";
					try {
						std::cout << userController.getCompletedTestsCount();
					}
					catch (FileNotLoadedException& e) {
						std::cout << 0;
					}
					std::cout << ". " << std::endl;
					break;
				}
				std::cin.ignore();

				std::string subject;
				std::cout << "Enter the subject: ";
				std::getline(std::cin, subject);

				std::cout << "The number of completed tests of this subject - ";
				try {
					std::cout << userController.getCompletedTestsCountBySubject(testsHandler, subject);
				}
				catch (FileNotLoadedException& e) {
					std::cout << 0;
				}
				std::cout << ". " << std::endl;
				break;
			}
			case 2: {
				if (testsHandler.getTestsCount() == 0) {
					std::cout << "There a no tests to pass. " << std::endl;
					break;
				}

				for (size_t i = 0; i < testsHandler.getTestsCount(); i++) {
					std::cout << testsHandler.getTestEditable(i).getSubject() << std::endl;
					try {
						std::cout << testsHandler.getTestEditable(i).getQuestion(0) << std::endl;
					}
					catch (OutOfRangeException& e) {
						std::cout << e.what() << std::endl;
					}
				}

				size_t index;

				do {
					std::cout << "Select a test: ";
					std::cin >> index;

					if (index == 0 || index > testsHandler.getTestsCount())
						std::cout << "Not a correct number. Try again. " << std::endl;
				} while (index == 0 || index > testsHandler.getTestsCount());

				userController.solveTest(testsHandler.getTestEditable(index - 1));
				break;
			}
			case 3: {
				std::vector<TestController> testControllers = userController.getNonFinishedTests();

				if (testControllers.size() == 0) {
					std::cout << "There are no non-finished tests. " << std::endl;
					break;
				}

				size_t index;

				do {
					std::cout << "Select a test: ";
					std::cin >> index;

					if (index == 0 || index > testControllers.size())
						std::cout << "Not a correct number. Try again. " << std::endl;
				} while (index == 0 || index > testControllers.size());

				userController.solveTest(testsHandler.getTestEditable(index - 1));
				break;
			}
			case 4:
				changeUsername(usersHandler, userController.getCurrentUser());
				break;
			case 5:
				changePassword(usersHandler, userController.getCurrentUser());
				break;
			case 6:
				changeName(usersHandler, userController.getCurrentUser());
				break;
			case 7:
				changeNumber(usersHandler, userController.getCurrentUser());
				break;
			case 0:
				break;
			default:
				std::cout << "Not a correct number. Try again. " << std::endl;
				break;
			}
		} while (mode != 0);
	}

	usersHandler.saveUsers();
	testsHandler.saveTests();

	return 0;
}

void registerUser(UsersHandler& usersHandler, bool admin) {
	std::string username, password, name, phone;

	if (std::cin.peek() == '\n')
		std::cin.ignore();

	bool isFree;

	std::cout << "Enter the name: ";
	std::getline(std::cin, name);

	do {
		std::cout << "Enter a username: ";
		std::getline(std::cin, username);

		isFree = usersHandler.freeUsername(username);
		if (!isFree)
			std::cout << "This username is not free. Try another.";
	} while (!isFree);

	std::cout << "Enter the password: ";
	std::getline(std::cin, password);

	std::cout << "Enter the phone number: ";
	std::getline(std::cin, phone);

	usersHandler.registerUser(User{ username, password, name, phone, admin });
}

void changeUsername(UsersHandler& usersHandler, User* user) {
	if (std::cin.peek() == '\n')
		std::cin.ignore();

	bool isFree;
	std::string newUsername;
	do {
		std::cout << "Enter a new username for this user: ";
		std::getline(std::cin, newUsername);

		isFree = usersHandler.freeUsername(newUsername);
		if (!isFree)
			std::cout << "This username is already taken. Try another." << std::endl;
	} while (!isFree);
	
	user->setUsername(newUsername);
}

void changePassword(UsersHandler& usersHandler, User* user) {
	if (std::cin.peek() == '\n')
		std::cin.ignore();

	std::string newPassword;
	std::cout << "Enter a new password: ";
	std::getline(std::cin, newPassword);

	user->setPassword(newPassword);
}

void changeName(UsersHandler& usersHandler, User* user) {
	if (std::cin.peek() == '\n')
		std::cin.ignore();

	std::string newName;
	std::cout << "Enter a new name: ";
	std::getline(std::cin, newName);

	user->setName(newName);
}

void changeNumber(UsersHandler& usersHandler, User* user) {
	if (std::cin.peek() == '\n')
		std::cin.ignore();

	std::string newNumber;
	std::cout << "Enter a new number: ";
	std::getline(std::cin, newNumber);

	user->setNumber(newNumber);
}

void modifyUser(UsersHandler& usersHandler) {
	size_t mode;

	do {
		User* user;

		std::vector<User>& users = usersHandler.getUsers();
		const size_t usersCount = users.size();

		for (size_t index = 0; index < usersCount; index++)
			std::cout << index + 1 << ". " << users[index].getUsername() << std::endl;

		size_t chosenUserIndex;

		do {
			std::cout << "Select a user (0 to exit): ";
			std::cin >> chosenUserIndex;

			if (chosenUserIndex > usersCount)
				std::cout << "Invalid user. Try again. " << std::endl;
		} while (chosenUserIndex > usersCount);

		if (chosenUserIndex == 0)
			return;

		user = &users[chosenUserIndex - 1];

		std::cout << "\t1. Change username" << std::endl
			<< "\t2. Change password" << std::endl
			<< "\t3. change name" << std::endl
			<< "\t4. Change phone number" << std::endl
			<< "\t0. Exit" << std::endl;

		std::cout << "Select a mode: ";
		std::cin >> mode;
		std::cin.ignore();

		switch (mode) {
		case 1:
			changeUsername(usersHandler, user);
			break;
		case 2:
			changePassword(usersHandler, user);
			break;
		case 3:
			changeName(usersHandler, user);
			break;
		case 4:
			changeNumber(usersHandler, user);
			break;
		case 0:
			break;
		default:
			std::cout << "Not a correct mode. Try again. " << std::endl;
			break;
		}
	} while (mode != 0);
}

void removeUser(UsersHandler& usersHandler) {
	std::vector<User>& users = usersHandler.getUsers();
	const size_t usersCount = users.size();

	for (size_t index = 0; index < usersCount; index++)
		std::cout << index + 1 << ". " << users[index].getUsername() << std::endl;

	size_t chosenUserIndex;

	do {
		std::cout << "Select a user: ";
		std::cin >> chosenUserIndex;

		if (chosenUserIndex == 0 || chosenUserIndex > usersCount)
			std::cout << "Invalid user. Try again. " << std::endl;
	} while (chosenUserIndex == 0 || chosenUserIndex > usersCount);

	users.erase(users.begin() + chosenUserIndex - 1);
}

size_t getAllCompletedTestsCount(UsersHandler& usersHandler) {
	size_t allCompletedTestsCount = 0;

	for (User& user : usersHandler.getUsers()) {
		try {
			allCompletedTestsCount += UserController{ &user }.getCompletedTestsCount();
		}
		catch (FileNotLoadedException& e) {
			std::cout << e.what() << std::endl;
		}
	}

	return allCompletedTestsCount;
}

size_t getAllCompletedTestsCountBySubject(UsersHandler& usersHandler, TestsHandler& testsHandler, const std::string& subject) {
	size_t allCompletedTestsCountBySubject = 0;

	for (User& user : usersHandler.getUsers()) {
		try {
			allCompletedTestsCountBySubject += UserController{ &user }.getCompletedTestsCountBySubject(testsHandler, subject);
		}
		catch (FileNotLoadedException& e) {
			std::wcout << e.what() << std::endl;
		}
	}

	return allCompletedTestsCountBySubject;
}

void addTest(TestsHandler& testsHandler) {
	if (std::cin.peek() == '\n')
		std::cin.ignore();

	std::string subject;
	std::cout << "Enter the subject: ";
	std::getline(std::cin, subject);

	Test* newTest = new Test{ subject };
	size_t addQuestion, addVariant;
	std::string stringTemp;

	do {
		std::cout << "\t1. Add question" << std::endl
			<< "\t0. Exit" << std::endl;

		std::cout << "Select a mode: ";
		std::cin >> addQuestion;
		std::cin.ignore();

		switch (addQuestion) {
		case 1: {
			std::cout << "Enter the question text: ";
			std::getline(std::cin, stringTemp);
			Question question(stringTemp);

			std::cout << "Now you have to add at least two variants. " << std::endl;

			for (size_t i = 0; i < 2; i++) {
				std::cout << "Enter the variant " << i + 1 << ": ";
				std::getline(std::cin, stringTemp);
				
				question.addVariant(stringTemp);
			}

			do {
				std::cout << "\t1. Add another variant" << std::endl
					<< "\t0. Exit" << std::endl;

				std::cout << "Select a mode: ";
				std::cin >> addVariant;
				std::cin.ignore();

				switch (addVariant) {
				case 1:
					std::cout << "Enter a new variant: ";
					std::getline(std::cin, stringTemp);

					question.addVariant(stringTemp);
					break;
				case 0:
					break;
				default:
					std::cout << "Not a correct number. Try again. " << std::endl;
				}

				if (question.getVariantsCount() == VARIANTS_MAX_COUNT) {
					std::cout << "Maximum number of variants reached. " << std::endl;
					break;
				}
			} while (addVariant != 0);

			size_t answer;
			std::cout << "Now you have to choose the right answer index. ";
			do {
				std::cout << "Enter the index: ";
				std::cin >> answer;

				if (answer == 0 || answer > question.getVariantsCount())
					std::cout << "Not a correct number. Try again. " << std::endl;
			} while (answer == 0 || answer > question.getVariantsCount());

			question.setCorrectVariant(answer - 1);

			newTest->addQuestion(question);
			break;
		}
		case 0:
			break;
		default:
			std::cout << "Not a correct number. Try again. " << std::endl;
			break;
		}

		if (newTest->getQuestionsCount() == QUESTIONS_MAX_COUNT)
			std::cout << "Maximum number of questions reached. " << std::endl;
	} while (addQuestion != 0);

	testsHandler.addTest(*newTest);
	delete newTest;
}

void modifyTest(TestsHandler& testsHandler) {
	if (testsHandler.getTestsCount() == 0) {
		std::cout << "No tests exist. " << std::endl;
		return;
	}

	for (size_t i = 0; i < testsHandler.getTestsCount(); i++) {
		std::cout << '\t' << i + 1 << ". " << testsHandler.getTestEditable(i).getSubject() << std::endl;
		try {
			std::cout << testsHandler.getTestEditable(i).getQuestion(0) << std::endl;
		}
		catch (OutOfRangeException& e) {
			std::cout << "Empty test." << std::endl;
		}
	}

	size_t testIndex;

	do {
		std::cout << "Choose a test: ";
		std::cin >> testIndex;

		if (testIndex == 0 || testIndex > testsHandler.getTestsCount())
			std::cout << "Not a correct number. Try again. " << std::endl;
	} while (testIndex == 0 || testIndex > testsHandler.getTestsCount());

	Test& modifyingTest = testsHandler.getTestEditable(testIndex - 1);

	size_t mod1;

	do {
		std::cout << "\t1. Subject" << std::endl
			<< "\t2. Questions" << std::endl
			<< "\t0. Exit" << std::endl;

		std::cout << "Select what to change: ";
		std::cin >> mod1;
		std::cin.ignore();

		switch (mod1) {
		case 1: {	
			std::string newSubject;
			std::cout << "Enter a new subject: ";
			std::getline(std::cin, newSubject);

			modifyingTest.setSubject(newSubject);
			break;
		}
		case 2: {
			size_t mod2;

			do {
				std::cout << "\t1. Add a question" << std::endl
					<< "\t2. Edit a question" << std::endl
					<< "\t3. Remove a question" << std::endl
					<< "\t0. Exit" << std::endl;

				std::cout << "Select a mode: ";
				std::cin >> mod2;
				std::cin.ignore();

				switch (mod2) {
				case 1: {
					std::string stringTemp;
					size_t addVariant;

					std::cout << "Enter the question text: ";
					std::getline(std::cin, stringTemp);
					Question question(stringTemp);

					std::cout << "Now you have to add at least two variants. " << std::endl;

					for (size_t i = 0; i < 2; i++) {
						std::cout << "Enter the variant " << i + 1 << ": ";
						std::getline(std::cin, stringTemp);

						question.addVariant(stringTemp);
					}

					do {
						std::cout << "\t1. Add another variant" << std::endl
							<< "\t0. Exit" << std::endl;

						std::cout << "Select a mode: ";
						std::cin >> addVariant;
						std::cin.ignore();

						switch (addVariant) {
						case 1:
							std::cout << "Enter a new variant: ";
							std::getline(std::cin, stringTemp);

							question.addVariant(stringTemp);
							break;
						case 0:
							break;
						default:
							std::cout << "Not a correct number. Try again. " << std::endl;
						}

						if (question.getVariantsCount() == VARIANTS_MAX_COUNT) {
							std::cout << "Maximum number of variants reached. " << std::endl;
							break;
						}
					} while (addVariant != 0);

					size_t answer;
					std::cout << "Now you have to choose the right answer index. ";
					do {
						std::cout << "Enter the index: ";
						std::cin >> answer;

						if (answer == 0 || answer > question.getVariantsCount())
							std::cout << "Not a correct number. Try again. " << std::endl;
					} while (answer == 0 || answer > question.getVariantsCount());
					question.setCorrectVariant(answer - 1);

					modifyingTest.addQuestion(question);
					break;
				}
				case 2: {
					if (modifyingTest.getQuestionsCount() == 0) {
						std::cout << "No question to edit. " << std::endl;
						break;
					}

					for (size_t i = 0; i < modifyingTest.getQuestionsCount(); i++) {
						std::cout << '\t' << i + 1 << " - " << modifyingTest.getQuestion(i).getQuestionText() << std::endl;
					}

					size_t toEdit;

					do {
						std::cout << "Select a test to edit: ";
						std::cin >> toEdit;

						if (toEdit == 0 || toEdit > modifyingTest.getQuestionsCount())
							std::cout << "Not a correct number. Try again. " << std::endl;
					} while (toEdit == 0 || toEdit > modifyingTest.getQuestionsCount());

					Question& modifyingQuestion = modifyingTest.getQuestionEditable(toEdit - 1);

					size_t mod3;

					do {
						std::cout << "\t1. Question text" << std::endl
							<< "\t2. Variants" << std::endl
							<< "\t0. Exit" << std::endl;

						std::cout << "Select what to change: ";
						std::cin >> mod3;
						std::cin.ignore();

						switch (mod3) {
						case 1: {
							std::string newText;

							std::cout << "Enter a new question text: ";
							std::getline(std::cin, newText);

							modifyingQuestion.setQuestionText(newText);
							break;
						}
						case 2: {
							size_t mod4;

							do {
								std::cout << "\t1. Add variant" << std::endl
									<< "\t2. Edit a variant" << std::endl
									<< "\t3. Remove a variant" << std::endl
									<< "\t4. Change the correct answer" << std::endl
									<< "\t0. Exit" << std::endl;

								std::cout << "Choose a mode: ";
								std::cin >> mod4;
								std::cin.ignore();
								
								switch (mod4) {
								case 1: {
									if (modifyingQuestion.getVariantsCount() == VARIANTS_MAX_COUNT) {
										std::cout << "Maximum number of variants reached. " << std::endl;
										break;
									}

									std::string variantText;
									std::cout << "Enter the text of the new variant: ";
									std::getline(std::cin, variantText);

									modifyingQuestion.addVariant(variantText);
									break;
								}
								case 2: {
									if (modifyingQuestion.getVariantsCount() == 0) {
										std::cout << "No variant to edit. " << std::endl;
										break;
									}

									for (size_t i = 0; i < modifyingQuestion.getVariantsCount(); i++) {
										std::cout << '\t' << i + 1 << ". " << modifyingQuestion.getVariant(i).getText() << std::endl;
									}

									size_t toEdit;

									do {
										std::cout << "Select a variant to edit: ";
										std::cin >> toEdit;
										std::cin.ignore();

										if (toEdit == 0 || toEdit > modifyingQuestion.getVariantsCount())
											std::cout << "Not a correct number. Try again. " << std::endl;
									} while (toEdit == 0 || toEdit > modifyingQuestion.getVariantsCount());

									std::string newVariantText;
									std::cout << "Enter the new variant text: ";
									std::getline(std::cin, newVariantText);
									
									modifyingQuestion.getVariantEditable(toEdit - 1).setText(newVariantText);
									break;
								}
								case 3: {
									if (modifyingQuestion.getVariantsCount() == 0) {
										std::cout << "No variant to delete. " << std::endl;
										break;
									}

									for (size_t i = 0; i < modifyingQuestion.getVariantsCount(); i++) {
										std::cout << '\t' << i + 1 << ". " << modifyingQuestion.getVariant(i).getText() << std::endl;
									}

									size_t toRemove;

									do {
										std::cout << "Select a variant to remove: ";
										std::cin >> toRemove;
										std::cin.ignore();

										if (toRemove == 0 || toRemove > modifyingQuestion.getVariantsCount())
											std::cout << "Not a correct number. Try again. " << std::endl;
									} while (toRemove == 0 || toRemove > modifyingQuestion.getVariantsCount());

									modifyingQuestion.removeVariant(toRemove - 1);
									break;
								}
								case 4: {
									if (modifyingQuestion.getVariantsCount() == 0) {
										std::cout << "No variant to choose as correct. " << std::endl;
										break;
									}

									for (size_t i = 0; i < modifyingQuestion.getVariantsCount(); i++) {
										std::cout << '\t' << i + 1 << ". " << modifyingQuestion.getVariant(i).getText() << std::endl;
									}

									size_t newCorrect;

									do {
										std::cout << "Select a variant to remove: ";
										std::cin >> newCorrect;
										std::cin.ignore();

										if (newCorrect == 0 || newCorrect > modifyingQuestion.getVariantsCount())
											std::cout << "Not a correct number. Try again. " << std::endl;
									} while (newCorrect == 0 || newCorrect > modifyingQuestion.getVariantsCount());

									modifyingQuestion.setCorrectVariant(newCorrect - 1);
									break;
								}
								case 0:
									break;
								default:
									std::cout << "Not a correct number. Try again. " << std::endl;
									break;
								}
							} while (mod4 != 0);

							break;
						}
						case 0:
							break;
						default:
							std::cout << "Not a correct number. Try again. " << std::endl;
							break;
						}
					} while (mod3 != 0);

					break;
				}
				case 3: {
					if (modifyingTest.getQuestionsCount() == 0) {
						std::cout << "No question to remove. " << std::endl;
						break;
					}

					for (size_t i = 0; i < modifyingTest.getQuestionsCount(); i++) {
						std::cout << '\t' << i + 1 << " - " << modifyingTest.getQuestion(i).getQuestionText() << std::endl;
					}

					size_t toDelete;

					do {
						std::cout << "Select a test to remove: ";
						std::cin >> toDelete;

						if (toDelete == 0 || toDelete > modifyingTest.getQuestionsCount())
							std::cout << "Not a correct number. Try again. " << std::endl;
					} while (toDelete == 0 || toDelete > modifyingTest.getQuestionsCount());

					modifyingTest.removeQuestion(toDelete - 1);
					break;
				}
				case 0:
					break;
				default:
					std::cout << "Not a correct number. Try again. " << std::endl;
					break;
				}

			} while (mod2 != 0);

			break;
		}
		case 0:
			break;
		default:
			std::cout << "Not a correct number. Try again. " << std::endl;
			break;
		}
	} while (mod1 != 0);
}

void removeTest(TestsHandler& testsHandler) {
	for (size_t i = 0; i < testsHandler.getTestsCount(); i++) {
		std::cout << '\t' << i + 1 << ". " << testsHandler.getTestEditable(i).getSubject() << std::endl;
		try {
			std::cout << testsHandler.getTestEditable(i).getQuestion(0) << std::endl;
		}
		catch (OutOfRangeException& e) {
			std::cout << "Empty test." << std::endl;
		}
	}

	size_t testIndex;

	do {
		std::cout << "Choose a test: " << std::endl;
		std::cin >> testIndex;

		if (testIndex == 0 || testIndex >= testsHandler.getTestsCount())
			std::cout << "Not a correct number. Try again. " << std::endl;
	} while (testIndex == 0 || testIndex >= testsHandler.getTestsCount());

	testsHandler.removeTest(testIndex - 1);
}

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Exceptions.h"
#include "FileProcess.h"

#define TEXT_MAX_LEN 256
#define QUESTIONS_MAX_COUNT 256
#define VARIANTS_MAX_COUNT 5

class Variant {
private:
	char variantText[TEXT_MAX_LEN];
public:
	Variant();
	Variant(const std::string& text);

	std::string getText() const;
	void setText(const std::string& text);

	bool operator==(const Variant& variant) const;
	bool operator!=(const Variant& variant) const;
};

class Question {
private:
	size_t variantsCount;
	int correctVariant;

	char questionText[TEXT_MAX_LEN];
	Variant variants[VARIANTS_MAX_COUNT];
public:
	Question();
	Question(const std::string& text);

	void addVariant(const std::string& variantText);
	void removeVariant(size_t index);

	const std::string getQuestionText() const;
	void setQuestionText(const std::string& text);

	int getCorrectVariantIndex() const;
	void setCorrectVariant(int index);

	const Variant& getVariant(size_t index) const;
	Variant& getVariantEditable(size_t index);

	size_t getVariantsCount() const;

	friend std::ostream& operator<<(std::ostream& os, const Question& question);
	bool operator==(const Question& question) const;
	bool operator!=(const Question& question) const;
};

class Test {
private:
	size_t questionsCount;

	char subject[TEXT_MAX_LEN];
	Question questions[QUESTIONS_MAX_COUNT];
public:
	Test(const std::string& subject);

	void addQuestion(const Question& question);

	const Question& getQuestion(size_t index) const;

	Question& getQuestionEditable(size_t index);

	void setSubject(const std::string& subject);

	void removeQuestion(size_t index);

	std::string getSubject() const;

	size_t getQuestionsCount() const;

	bool operator==(const Test& test);
	bool operator!=(const Test& test);
};

class TestsHandler {
private:
	std::string filename;
	std::vector<Test> allTests;
public:
	TestsHandler(const std::string& filename);

	void addTest(const Test& test);

	void loadTests();

	void removeTest(size_t index);

	Test& getTestEditable(size_t index);

	void saveTests();

	size_t getTestsCount() const;

	void importTest(const std::string& filename);
};

class TestController {
private:
	bool solved;
	size_t passedQuestions;
	size_t correctAnswers;

	Test test;
public:
	TestController(const Test& test);

	bool isSolved() const;

	const Test& getTest() const;

	void solveTest();
};
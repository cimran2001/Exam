#include "Test.h"

Variant::Variant() {
    strncpy_s(variantText, "", TEXT_MAX_LEN);
}

Variant::Variant(const std::string& text){
    strncpy_s(variantText, text.c_str(), TEXT_MAX_LEN);
}

std::string Variant::getText() const {
    return std::string{ variantText };
}


void Variant::setText(const std::string& text) {
    strncpy_s(variantText, text.c_str(), TEXT_MAX_LEN);
}

bool Variant::operator==(const Variant& variant) const {
    return getText() == variant.getText();
}

bool Variant::operator!=(const Variant& variant) const {
    return getText() != variant.getText();
}

Question::Question() : variantsCount(0), correctVariant(-1) {
    strncpy_s(questionText, "", TEXT_MAX_LEN);
}

Question::Question(const std::string& text) : variantsCount(0), correctVariant(-1) {
    strncpy_s(questionText, text.c_str(), TEXT_MAX_LEN);
}

void Question::addVariant(const std::string& variantText) {
    if (variantsCount >= VARIANTS_MAX_COUNT)
        throw MaxQuantityLimitException{ "Maximum number of variants is reached." };

    variants[variantsCount++] = Variant{ variantText };
}

void Question::removeVariant(size_t index) {
    if (index >= variantsCount)
        throw OutOfRangeException{ "No variant with such an index exists." };

    for (size_t i = index; i < variantsCount - 1; i++)
        variants[i] = variants[i + 1];

    variantsCount--;
}

const std::string Question::getQuestionText() const {
    return std::string{questionText};
}

void Question::setQuestionText(const std::string& text) {
    strncpy_s(questionText, text.c_str(), TEXT_MAX_LEN);
}

int Question::getCorrectVariantIndex() const {
    return correctVariant;
}

void Question::setCorrectVariant(int index) {
    if (index < 0 || index >= variantsCount)
        throw OutOfRangeException{ "No variant with such an index exists." };
 
    correctVariant = index;
}

const Variant& Question::getVariant(size_t index) const {
    if (index >= variantsCount)
        throw OutOfRangeException{ "No variant with such an index." };

    return variants[index];
}

Variant& Question::getVariantEditable(size_t index) {
    if (index >= variantsCount)
        throw OutOfRangeException{ "No variant with such an index." };

    return variants[index];
}

size_t Question::getVariantsCount() const {
    return variantsCount;
}

bool Question::operator==(const Question& question) const {
    if (getVariantsCount() != question.getVariantsCount() || getCorrectVariantIndex() != question.getCorrectVariantIndex())
        return false;

    if (getQuestionText() != question.getQuestionText())
        return false;

    for (size_t i = 0; i < getVariantsCount(); i++)
        if (getVariant(i) != question.getVariant(i))
            return false;

    return true;
}

bool Question::operator!=(const Question& question) const {
    return !(*this == question);
}

Test::Test(const std::string& subject) : questionsCount(0) {
    strncpy_s(this->subject, subject.c_str(), TEXT_MAX_LEN);
}

void Test::addQuestion(const Question& question) {
    if (questionsCount >= QUESTIONS_MAX_COUNT)
        throw MaxQuantityLimitException{ "Maximum number of questions is reached." };

    questions[questionsCount++] = question;
}

const Question& Test::getQuestion(size_t index) const {
    if (index >= questionsCount)
        throw OutOfRangeException{ "No question with such an index." };

    return questions[index];
}

Question& Test::getQuestionEditable(size_t index) {
    if (index >= questionsCount)
        throw OutOfRangeException{ "No question with such an index." };

    return questions[index];
}

void Test::setSubject(const std::string& subject) {
    strncpy_s(this->subject, subject.c_str(), TEXT_MAX_LEN);
}

void Test::removeQuestion(size_t index) {
    if (index >= questionsCount)
        throw OutOfRangeException{ "No question with such an index." };

    questionsCount--;
    for (; index < questionsCount; index++)
        questions[index] = questions[index + 1];
}

std::string Test::getSubject() const {
    return std::string{ subject };
}


size_t Test::getQuestionsCount() const {
    return questionsCount;
}

bool Test::operator==(const Test& test) {
    if (getQuestionsCount() != test.getQuestionsCount() || getSubject() != test.getSubject())
        return false;

    for (size_t i = 0; i < getQuestionsCount(); i++)
        if (getQuestion(i) != test.getQuestion(i))
            return false;

    return true;
}

bool Test::operator!=(const Test& test) {
    return !(*this == test);
}

TestsHandler::TestsHandler(const std::string& filename) : filename(filename) {
}

void TestsHandler::addTest(const Test& test) {
    allTests.push_back(test);
}

void TestsHandler::loadTests() {
    try {
        file_process::readFromFileBinary(filename, allTests);
    }
    catch (FileNotLoadedException& e) {
        std::cout << e.what() << std::endl;
    }
}

void TestsHandler::removeTest(size_t index) {
    if (index >= getTestsCount())
        throw OutOfRangeException{ "Not a correct index. Try again. " };

    allTests.erase(allTests.begin() + index);
}

Test& TestsHandler::getTestEditable(size_t index) {
    if (index >= getTestsCount())
        throw OutOfRangeException{ "Not a correct index. Try again. " };

    return allTests[index];
}

void TestsHandler::saveTests() {
    try {
        file_process::writeToFileBinary(filename, allTests);
    }
    catch (FileNotLoadedException& e) {
        std::cout << e.what() << std::endl;
    }
}

size_t TestsHandler::getTestsCount() const {
    return allTests.size();
}

void TestsHandler::importTest(const std::string& filename) {
    std::ios::openmode mode = std::ios::in;
    std::ifstream input{ filename, mode };

    if (!input.is_open())
        throw FileNotLoadedException{ "Couldn't import the test." };


    std::string temp;
    std::getline(input, temp);

    Test* test = new Test{temp};

    size_t questionsCount, variantsCount, correctVariant;

    input >> questionsCount;
    input.ignore();

    for (size_t _ = 0; _ < questionsCount; _++) {
        Question question;
        std::getline(input, temp);
        question.setQuestionText(temp);

        input >> variantsCount >> correctVariant;
        input.ignore();

        for (size_t _ = 0; _ < variantsCount; _++) {
            std::getline(input, temp);
            question.addVariant(temp);
        }

        question.setCorrectVariant(correctVariant - 1);

        test->addQuestion(question);
    }

    allTests.push_back(*test);
    delete test;
}



TestController::TestController(const Test& test) : passedQuestions(0), correctAnswers(0), solved(false), test(test) {
}

bool TestController::isSolved() const {
    return solved;
}

const Test& TestController::getTest() const {
    return test;
}

void TestController::solveTest() {
    size_t variantIndex;
    std::cout << "\t\tSolving " << test.getSubject() << std::endl;

    while (passedQuestions < test.getQuestionsCount()) {
        std::cout << test.getQuestion(passedQuestions) << std::endl;

        std::cout << "Select a variant (0 to stop solving the test): ";
        std::cin >> variantIndex;

        if (variantIndex == 0) {
            std::cout << "Terminating..." << std::endl;
            break;
        }

        if (variantIndex > test.getQuestion(passedQuestions).getVariantsCount()) {
            std::cout << "Not a correct index. Try again" << std::endl;
            continue;
        }

        if (test.getQuestion(passedQuestions).getCorrectVariantIndex() == variantIndex - 1)
            correctAnswers++;

        passedQuestions++;
    }

    solved = true;
}

std::ostream& operator<<(std::ostream& os, const Question& question) {
    os << "\tQuestion: " << question.getQuestionText() << '\n';

    for (size_t i = 0; i < question.getVariantsCount(); i++) {
        os << i + 1 << ". " << question.getVariant(i).getText() << '\n';
    }

    return os;
}

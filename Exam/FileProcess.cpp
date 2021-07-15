#include "FileProcess.h"

size_t file_process::fileSize(const std::string& filename) {
	std::ios::openmode mode = std::ios::in | std::ios::binary;
	std::ifstream in{ filename, mode };

	if (!in.is_open())
		return 0;

	in.seekg(0, std::ios::end);
	return in.tellg();
}

bool file_process::fileExists(const std::string& filename) {
	return file_process::fileSize(filename) != 0;
}

void file_process::createFile(const std::string& filename) {
	std::ios::openmode mode = std::ios::out | std::ios::binary | std::ios::trunc;
	std::ofstream file{ filename, mode };
	file.close();
}

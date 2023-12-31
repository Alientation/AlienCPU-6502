#include <string>
#include <vector>
#include "File.h"

#ifndef DIRECTORY_H
#define DIRECTORY_H


class Directory;

class Directory {
	public:
		static bool isValidDirectoryPath(const std::string dirPath) {
			return dirPath.find_first_of("*?\"<>|") == std::string::npos;
		}

		Directory(const std::string dirPath, bool createDirectoryIfNotPresent = false);
		~Directory();

		std::string getDirectoryName();
		std::string getDirectoryPath();
		int getDirectorySize();
		std::vector<File*> getSubfiles();
		std::vector<Directory*> getSubdirectories();
		Directory* getSubdirectory(const std::string subdirectoryPath);
		File* getSubfile(const std::string subfilePath);

		bool subdirectoryExists(const std::string subdirectoryPath);
		bool subfileExists(const std::string subfilePath);

		bool exists();
		void create();

	private:
		std::string dirPath;

};

#endif
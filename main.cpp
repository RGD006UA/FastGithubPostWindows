#include <stdexcept>
#include <iostream>

#include "command-parser.hpp"

int main(int argc, char* argv[])
{
	try
	{
		MyGit MyGit(argv, argc);

		MyGit.make_git_init();

		MyGit.proccess();

		return 0;
	}
	catch (std::invalid_argument& e)
	{
		std::cout << "Error: " << e.what() << "\n";
		return 1;
	}
	catch (std::logic_error& e)
	{
		std::cout << "Erorr: " << e.what() << "\n";
		return 1;
	}
}
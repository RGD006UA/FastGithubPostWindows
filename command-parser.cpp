#include "command-parser.hpp"

#include <stdexcept>
#include <iostream>
#include <cstring>

MyGit::MyGit(char* argv[], const int csize)
{
	/*
	 * 
	 * Init functors in constructor from lambda functions 
	 * this->function_for_command_
	 */
	functions_for_command_[all_files_]  = [this]() { add_all_files(); };
	functions_for_command_[put_]	    = [this]() { push_files(); };
	functions_for_command_[get_]	    = [this]() { get_files(); };
	functions_for_command_[clone_]		= [this]() { clone_files(); };
	functions_for_command_[file_]		= [this]() { add_file(); };

	for (int i = 1; i < csize; i++)
	{
		if (std::strcmp(argv[i], "-f") == 0)
		{
			if (std::string(argv[i + 1]).find("\\") != std::string::npos)
			{
				path_ = std::string(argv[i + 1]);
				i++;
				continue;
			}
			else
				throw std::logic_error("the path isn't write correct");
		}
		else if (std::strcmp(argv[i], "-u") == 0)
		{
			if (std::string(argv[i + 1]).find("//") != std::string::npos)
			{
				url_ = std::string(argv[i + 1]);
				i++;
				commands[finded_url_] = true;
				continue;
			}
			else 
				throw std::logic_error("the url isn't write correct");

		}
		else if (std::strcmp(argv[i], "-all") == 0)
		{
				commands[all_files_] = true;
		}
		else if (std::strcmp(argv[i], "put") == 0)
		{
			commands[put_] = true;
		}
		else if (std::strcmp(argv[i], "get") == 0)
		{
			if (!commands.test(put_)) commands[get_] = true;
			else throw std::logic_error("You have already used function with operations");
		}
		else if (std::strcmp(argv[i], "clone") == 0)
		{
			if (!commands.test(put_) && !commands.test(get_)) commands[clone_] = true;
			else throw std::logic_error("You have already used function with operations");
		}
		else if (std::strcmp(argv[i], "-fs") == 0)
		{
			if (!commands.test(all_files_))
			{
				i++;
				const int files_size = std::strtol(argv[i], nullptr, 10) + i;

				for (int file = i + 1; file <= files_size; ++file)
					files_.push_back(std::string(argv[file]));

				commands[file_] = true;

				i += std::strtol(argv[i], nullptr, 10);
			}
			else
				throw std::logic_error("parameter -all are written");
		}
		else
		{
			throw std::invalid_argument(std::string(argv[i]) + " this command isn't work");
		}
	}

	if (!commands.test(finded_url_) && commands.test(put_) || commands.test(get_) || commands.test(clone_)) throw std::logic_error("The url isn't writted");
	if (path_.empty())
	{
		std::string tmp(argv[0]);
		size_t cut = tmp.size();
		for (size_t i = tmp.size() - 1; i >= 0; i--)
		{
			if (tmp[i] == '\\')
			{
				cut = i;
				break;
			}
		}
		tmp = tmp.substr(0, cut);
		main_path_ = tmp;
		path_ = tmp;

		//if (commands.test(all_files_) || commands.test(put_) || commands.test(get_))
		//	throw std::logic_error("You need to be in root dir for adding/putting/getting commands");

		std::cout << path_ << "\n";
	}
	std::cout << "Done!\n";
}

namespace fs = boost::filesystem;
namespace pr = boost::process;

void MyGit::make_git_init()
{
	const fs::path git_command_path(path_);

	for (const auto& entry : fs::directory_iterator(git_command_path))
	{
		if (fs::is_directory(entry.path()) && entry.path().filename() == ".git")
		{
			std::cout << "There is init directory\n";
			return;
		}
	}

	std::cout << "Initializating git\n";
	const std::string command("init");

	if (pr::system(gitexe_path, command, path_) == 0)
		std::cout << "Initializating git done\n";
	else
		std::cout << "Error!\n";
}

void MyGit::add_all_files()
{
	const std::string command("add");

	if (pr::system(gitexe_path, "-C", path_, command, "--all") == 0)
		std::cout << "Added all files\n";
	else
		std::cout << "Error to adding all files!\n";
}

#include <chrono>


void MyGit::push_files()
{
	using namespace std::chrono;

	const std::string command_commit("commit");
	const std::string command_push("push");
	const auto time = system_clock::now();
	const std::string timestamp = std::to_string(duration_cast<seconds>(time.time_since_epoch()).count());

	if (pr::system(gitexe_path, "-C", path_, command_commit, "-m", timestamp) == 0)
		std::cout << "commited files\n";
	else
		std::cout << "files not commited\n";

	if (pr::system(gitexe_path, "-C", path_, command_push, "--set-upstream",url_, "master") == 0)
		std::cout << "pushed files!\n";
	else
		std::cout << "files didn't push\n";
}

void MyGit::get_files()
{
	const std::string command_fetch("fetch");

	// TODO: make here git fetch
}

void MyGit::clone_files()
{
	const std::string command_clone("clone");

	if (pr::system(gitexe_path, "-C", path_, command_clone, url_) == 0)
		std::cout << "Clone completed!\n";
	else
		std::cout << "Clone not completed\n";
}

void MyGit::add_file()
{
	const std::string command_add("add");

	for (const std::string& file : files_)
	{
		auto full_path = fs::path(path_ + "\\" + file);

		std::cout << full_path << "\n";

		if (!fs::exists(full_path))
			throw std::logic_error("There are no such as file");

		if (pr::system(gitexe_path, "-C", path_, command_add, file) == 0)
			std::cout << "adding " + file + " done\n";
		else
			std::cout << "file " + file + " not added\n";
	}
}


void MyGit::proccess() const
{
	std::cout << commands << '\n';

	for (size_t i = 7; i >= 0; --i)
	{
		if (commands.test(i))
			functions_for_command_[i]();
	}
}

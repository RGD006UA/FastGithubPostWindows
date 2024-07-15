#pragma once

#include <bitset>
#include <string>
#include <vector>
#include <functional>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
namespace pr = boost::process;

class MyGit
{
private:
	void push_files();
	void get_files();
	void add_all_files();
	void clone_files();
	void add_file();

	using function = std::function<void()>;

	function functions_for_command_[8];

	std::string path_;
	std::string url_;
	std::string main_path_;
	unsigned int finded_url_ = 0;
	unsigned int all_files_ = 1;
	unsigned int put_ = 2;
	unsigned int get_ = 3;
	unsigned int clone_ = 4;
	unsigned int file_ = 5;
	std::vector<std::string> files_;
	fs::path gitexe_path = pr::search_path("git");
	std::bitset<8> commands;
public:
	MyGit(char* argv[], const int csize);
	void make_git_init();
	void proccess() const;
};
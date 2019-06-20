#pragma once

#include <sqlite3.h>

namespace gh {
	
	class command_repository
	{
	public:
		command_repository(std::string const & filename);
		~command_repository();

		std::map<std::string, std::string> load_commands();
		void add_command(std::string const & name, std::string const & reply);

	private:
		std::unique_ptr<sqlite3, decltype(&sqlite3_close)> db_handle;
	};

}
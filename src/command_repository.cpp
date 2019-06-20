#include "pch.hpp"
#include "command_repository.h"

namespace {
	std::unique_ptr<sqlite3, decltype(&sqlite3_close)>
		connect(std::string const& filename)
	{
		sqlite3* handle;
		int result = sqlite3_open("hubballbot.db", &handle);
		return std::unique_ptr<sqlite3, decltype(&sqlite3_close)>(handle, &sqlite3_close);
	}

	std::string const
		sqlite3_column_string(sqlite3_stmt* statement, int const column)
	{
		return std::string(reinterpret_cast<char const*>(sqlite3_column_text(statement, column)));
	}
}

namespace gh {
	command_repository::command_repository(std::string const& filename)
		: db_handle(connect(filename)) { }

	command_repository::~command_repository()
	{
		sqlite3_close(db_handle.get());
	}

	std::map<std::string, std::string> command_repository::load_commands()
	{
		std::map<std::string, std::string> commands;
		sqlite3_stmt* statement;
		sqlite3_prepare_v2(this->db_handle.get(), "SELECT Name, Reply FROM Commands", -1, &statement, NULL);

		int result;
		while ((result = sqlite3_step(statement)) == SQLITE_ROW) {
			std::string name(sqlite3_column_string(statement, 0));
			std::string reply(sqlite3_column_string(statement, 1));
			commands[name] = reply;
		}

		sqlite3_finalize(statement);
		return commands;
	}

	void command_repository::add_command(std::string const& name, std::string const& reply)
	{
		sqlite3_stmt* add_command_statement;

		sqlite3_prepare_v2(
			this->db_handle.get(),
			"INSERT INTO Commands (Name, Reply) VALUES (:Name, :Reply)",
			-1,
			&add_command_statement,
			NULL);

		sqlite3_bind_text(add_command_statement, 1, name.c_str(), static_cast<int>(name.length()), SQLITE_TRANSIENT);
		sqlite3_bind_text(add_command_statement, 2, reply.c_str(), static_cast<int>(reply.length()), SQLITE_TRANSIENT);

		sqlite3_step(add_command_statement);
		sqlite3_finalize(add_command_statement);
	}
}

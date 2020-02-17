#pragma once

#include <string>
#include <array>

#include <cstdint>

class ImGuiConsole
{
public:
	ImGuiConsole(std::string name) : m_Name(name) {}

	void Render();
	void AddData(const char* data, std::size_t bytes);
	inline void AddData(const std::string& string) { AddData(string.c_str(), string.size()); }

private:
	void AddDataHelper(const char* data, std::size_t bytes, bool append);


private:
	std::string m_Name;
	std::array<std::string, 4096> m_Lines;
	int m_Current_Line = 0;
	bool m_IsFull = false;	
};


#include "ImGuiConsole.h"

#include <imgui.h>
#include <TUtil/StringUtils.h>

void ImGuiConsole::Render()
{
	ImGui::Begin(m_Name.c_str());
	//Buffer is full, draw wrapped around chars first
	if (m_IsFull)
	{
		for (int i = m_Current_Line + 1; i < m_Lines.size(); i++)
		{
			ImGui::Text("%s", m_Lines[i].c_str());
		}
	}

	for (int i = 0; i <= m_Current_Line; i++)
	{
		ImGui::Text("%s", m_Lines[i].c_str());
	}
	ImGui::End();
}

void ImGuiConsole::AddDataHelper(const char* data, std::size_t bytes, bool append)
{
	if (bytes == 0) return;

	std::string& currentLine = m_Lines[m_Current_Line];
	if (!currentLine.empty() && currentLine[currentLine.size() - 1] == '\n' && append)
	{
		currentLine.shrink_to_fit();
		m_Current_Line++;
		if (m_Current_Line == m_Lines.size())
		{
			m_Current_Line = 0;
			m_IsFull = true;
		}
		AddDataHelper(data, bytes, false);
	}
	else
	{
		if (!append) currentLine.clear();
		currentLine.append(data, data + 1);
		AddDataHelper(data + 1, bytes - 1, true);
	}
}


void ImGuiConsole::AddData(const char* data, std::size_t bytes)
{
	AddDataHelper(data, bytes, true);
}


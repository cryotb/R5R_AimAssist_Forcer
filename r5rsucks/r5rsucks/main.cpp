#include <iostream>
#include <algorithm>
#include <filesystem>
#include <Windows.h>
#include <TlHelp32.h>

namespace tools
{
	inline DWORD get_process_id_by_name(const std::string& name)
	{
		PROCESSENTRY32 entry{};
		entry.dwSize = sizeof(PROCESSENTRY32);

		DWORD result = NULL;
		auto* const snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot_handle, &entry) == FALSE)
		{
			CloseHandle(snapshot_handle);
			return result;
		}

		while (Process32Next(snapshot_handle, &entry))
		{
			std::string entry_name(entry.szExeFile);

			std::transform(entry_name.begin(), entry_name.end(), entry_name.begin(), [](wchar_t character)
				{
					return std::tolower(character);
				});

			if (entry_name.empty() || entry_name[0] == 0)
			{
				continue;
			}

			//printf("%ws.\n", entry_name.c_str());

			if (entry_name == name)
			{
				result = entry.th32ProcessID;
				break;
			}
		}

		CloseHandle(snapshot_handle);
		return result;
	}

	struct module_t
	{
		std::string m_name{};

		std::uintptr_t m_base{};
		std::uintptr_t m_size{};
	};


	using module_list = std::vector<module_t>;

	inline auto text_to_lower(const std::string& input)
	{
		auto output = std::string(input);

		std::transform(output.begin(), output.end(), output.begin(),
			[](const unsigned char c) { return std::tolower(c); });

		return output;
	}

	inline module_list get_process_modules(HANDLE process)
	{
		auto snapshot = HANDLE{};

		auto result = module_list();
		auto entry = MODULEENTRY32{};

		auto process_id = GetProcessId(process);

		entry.dwSize = sizeof(entry);

		snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32 | TH32CS_SNAPMODULE, process_id);

		if (!Module32First(snapshot, &entry))
		{
			CloseHandle(snapshot);
			return result;
		}

		do
		{
			const auto module_name = std::filesystem::path(entry.szExePath).filename().string();

			auto& info = result.emplace_back();

			info.m_name = text_to_lower(module_name);
			info.m_base = (DWORD_PTR)entry.modBaseAddr;
			info.m_size = entry.modBaseSize;

		} while (Module32Next(snapshot, &entry));

		CloseHandle(snapshot);

		return result;
	}
}

bool try_convert_str_to_float_safe(const std::string& value, float& out)
{
	__try
	{
		out = std::stof(value);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}

HANDLE ghApex{ };
uintptr_t gdwApexBase{ };

#include <thread>

bool override_it(float value)
{
	if (!WriteProcessMemory(ghApex, (void*)(gdwApexBase + 0x1718B00),
		&value, sizeof(value), nullptr))
	{
		printf("failed VM_READ!\n");
		return false;
	}
	return true;
}

void scan(int pid)
{
	while (true)
	{
		char pszTitle[512];
		memset(pszTitle, 0, sizeof(pszTitle));

		float current_value{ };
		if (!ReadProcessMemory(ghApex, (void*)(gdwApexBase + 0x1718B00),
			&current_value, sizeof(current_value), nullptr))
		{
			printf("failed VM_READ!\n");
			break;
		}

		sprintf_s(pszTitle, "drof's r5r aa overrider | current preset[%.2f%%]", current_value * 100.f);
		SetConsoleTitleA(pszTitle);

		Sleep(500);
	}
}

void doit()
{
	printf(R"(
		R5Reloaded overrider for Aim Assist - by youtube.com/@drof7659
			------- USAGE ------
			Enter your desired value for aim assist like this:
				"20" - for 20 percent aim assist
				"40" - for 40 percent aim assist
			and so on. Step doesn't matter, you can use 45, 22, etc.
			Don't input 'percent sign', just number!
			You can check this window title for game's current aim assist value.
			---------------------
		For support, contact cryotB on discord.
	)");
	int pid{ };
	printf("waiting for the process...\n");
	do
	{
		pid = tools::get_process_id_by_name("r5apex.exe");
	} while (!pid);

	printf("found, pid: %i\n", pid);

	ghApex = OpenProcess(PROCESS_VM_READ|PROCESS_VM_WRITE|PROCESS_QUERY_INFORMATION, FALSE, pid);

	if (ghApex == nullptr)
	{
		printf("error: failed to open handle for r5r.\n");
		return;
	}

	gdwApexBase = tools::get_process_modules(ghApex).front().m_base;

	std::thread(scan, pid).detach();

	while (true)
	{
		printf("enter aim assist value to be overriden:\n\n ");
		std::string input_value{ };
		std::getline(std::cin, input_value);

		float override_value = NAN;
		if (!try_convert_str_to_float_safe(input_value, override_value))
		{
			printf("invalid input! see above for instructions.\n");
			continue;
		}

		printf("overriding to %f\n", override_value);

		if (override_value > 100.f)
		{
			printf("you cannot override more than max!\n");
			continue;
		}

		if (override_it(override_value > 0.f ? (override_value / 100.f) : 0.f))
		{
			printf("override success!\n");
		}
		else
		{
			printf("override failed!\n");
		}
	}

	CloseHandle(ghApex);
	ghApex = 0;
	gdwApexBase = 0;
}

int main()
{
	doit();

	return std::getchar();
}

#include <Windows.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
class Mem {
public:
    Mem() = default;

    bool FindString(std::vector<size_t>& addresses, const std::string& target) {
        MEMORY_BASIC_INFORMATION mbi;

        for (size_t address = 0; VirtualQuery((PVOID)address, &mbi, sizeof mbi); address += mbi.RegionSize) {

            if (mbi.State == MEM_COMMIT && mbi.Protect == PAGE_READWRITE) {

                for (size_t x = 0; x < mbi.RegionSize; ++x) {

                    bool match = true;
                    for (size_t y = 0; y < target.length(); ++y) {

                        if (*(unsigned char*)(address + x + y) != target[y]) {
                            match = false;
                            break;
                        }
                    }

                    if (match) {
                        //push back to address list if matches
                        addresses.push_back(address + x);
                    }
                }
            }
        }
        return true;
    }

    bool FilterString(std::vector<size_t>& addresses, const std::string& target) {
        for (auto it = addresses.begin(); it != addresses.end();) {
            char buffer[1024];
            SIZE_T bytesRead;

            if (ReadProcessMemory(GetCurrentProcess(), (LPCVOID)*it, buffer, target.size(), &bytesRead)) {
                if (std::strncmp(buffer, target.c_str(), target.size()) == 0) {
                    ++it; // matches
                }
                else {
                    it = addresses.erase(it); // doesnt maych
                }
            }
            else {
                it = addresses.erase(it); // read failed
            }
        }
        return true;
    }

    bool ReplaceString(const std::vector<size_t>& addresses, const std::string& newValue) {
        //loop through addresses
        for (const auto& addr : addresses) {
            size_t newSize = newValue.length();
            SIZE_T bytesWritten;

            //write to the address
            if (WriteProcessMemory(GetCurrentProcess(), (LPVOID)addr, newValue.c_str(), newSize, &bytesWritten) &&
                bytesWritten == newSize) {
                memset((void*)(addr + newSize), 0, 1); //set one byte to null after the string
            }
        }
        return true;
    }

    bool FindUnicode(std::vector<size_t>& addresses, const std::wstring& target) {
        MEMORY_BASIC_INFORMATION mbi;

        for (size_t address = 0; VirtualQuery((PVOID)address, &mbi, sizeof mbi); address += mbi.RegionSize) {

            if (mbi.State == MEM_COMMIT && mbi.Protect == PAGE_READWRITE) {

                for (size_t x = 0; x < mbi.RegionSize; x += sizeof(wchar_t)) {

                    bool match = true;
                    for (size_t y = 0; y < target.length(); ++y) {

                        if (*(wchar_t*)(address + x + y * sizeof(wchar_t)) != target[y]) {
                            match = false;
                            break;
                        }
                    }

                    if (match) {
                        //push back to address list if matches
                        addresses.push_back(address + x);
                    }
                }
            }
        }
        return true;
    }

    bool FilterUnicode(std::vector<size_t>& addresses, const std::wstring& target) {
        for (auto it = addresses.begin(); it != addresses.end();) {
            wchar_t buffer[1024];
            SIZE_T bytesRead;

            if (ReadProcessMemory(GetCurrentProcess(), (LPCVOID)*it, buffer, target.size() * sizeof(wchar_t), &bytesRead)) {
                if (std::wcsncmp(buffer, target.c_str(), target.size()) == 0) {
                    ++it;
                }
                else {
                    it = addresses.erase(it);
                }
            }
            else {
                it = addresses.erase(it);
            }
        }
        return true;
    }

    bool ReplaceUnicode(const std::vector<size_t>& addresses, const std::wstring& newValue) {
        //loop through addresses
        for (const auto& addr : addresses) {
            size_t newSize = newValue.length();
            SIZE_T bytesWritten;

            //write to the address
            if (WriteProcessMemory(GetCurrentProcess(), (LPVOID)addr, newValue.c_str(), newSize * sizeof(wchar_t), &bytesWritten) &&
                bytesWritten == newSize * sizeof(wchar_t)) {
                memset((void*)(addr + newSize * sizeof(wchar_t)), 0, sizeof(wchar_t));  //set one byte to null after the string
            }
        }
        return true;
    }

    bool FindFloat(std::vector<size_t>& addresses, float target, float tolerance = 0.01) {
        MEMORY_BASIC_INFORMATION mbi;

        for (size_t address = 0; VirtualQuery((PVOID)address, &mbi, sizeof(mbi)); address += mbi.RegionSize) {
            if (mbi.State == MEM_COMMIT && mbi.Protect == PAGE_READWRITE) {
                for (size_t x = 0; x < mbi.RegionSize; x += sizeof(float)) {
                    float value = *(float*)(address + x);
                    if (fabs(value - target) <= tolerance) {
                        // Push back to address list if value is within tolerance
                        addresses.push_back(address + x);
                    }
                }
            }
        }
        return true;
    }

    bool FilterFloat(std::vector<size_t>& addresses, float target) {
        for (auto it = addresses.begin(); it != addresses.end();) {
            float buffer;
            SIZE_T bytesRead;

            if (ReadProcessMemory(GetCurrentProcess(), (LPCVOID)*it, &buffer, sizeof(float), &bytesRead)) {
                if (buffer == target) {
                    ++it; // matches
                }
                else {
                    it = addresses.erase(it); //  doesnt match
                }
            }
            else {
                it = addresses.erase(it); // read failed
            }
        }
        return true;
    }

    bool ReplaceFloat(const std::vector<size_t>& addresses, float newValue) {
        //loop through addresses
        for (const auto& addr : addresses) {
            SIZE_T bytesWritten;
            //write to address
            if (WriteProcessMemory(GetCurrentProcess(), (LPVOID)addr, &newValue, sizeof(float), &bytesWritten) &&
                bytesWritten == sizeof(float)) {
            }
        }
        return true;
    }

    bool FindInt(std::vector<size_t>& addresses, int target) {
        MEMORY_BASIC_INFORMATION mbi;

        for (size_t address = 0; VirtualQuery((PVOID)address, &mbi, sizeof mbi); address += mbi.RegionSize) {
            if (mbi.State == MEM_COMMIT && mbi.Protect == PAGE_READWRITE) {
                for (size_t x = 0; x < mbi.RegionSize; x += sizeof(int)) {
                    if (*(int*)(address + x) == target) {
                        //push back to address list if matches
                        addresses.push_back(address + x);
                    }
                }
            }
        }
        return true;
    }

    bool FilterInt(std::vector<size_t>& addresses, int target) {
        for (auto it = addresses.begin(); it != addresses.end();) {
            int buffer;
            SIZE_T bytesRead;

            if (ReadProcessMemory(GetCurrentProcess(), (LPCVOID)*it, &buffer, sizeof(int), &bytesRead)) {
                if (buffer == target) {
                    ++it; // matches
                }
                else {
                    it = addresses.erase(it); // doesnt match
                }
            }
            else {
                it = addresses.erase(it); // read failed
            }
        }
        return true;
    }

    bool ReplaceInt(const std::vector<size_t>& addresses, int newValue) {
        //loop through addresses
        for (const auto& addr : addresses) {
            SIZE_T bytesWritten;
            //write to address
            if (WriteProcessMemory(GetCurrentProcess(), (LPVOID)addr, &newValue, sizeof(int), &bytesWritten) &&
                bytesWritten == sizeof(int)) {
            }
        }
        return true;
    }

    bool FindHex(std::vector<size_t>& addresses, uint32_t target) {
        MEMORY_BASIC_INFORMATION mbi;

        for (size_t address = 0; VirtualQuery((PVOID)address, &mbi, sizeof(mbi)); address += mbi.RegionSize) {
            if (mbi.State == MEM_COMMIT && mbi.Protect == PAGE_READWRITE) {
                for (size_t x = 0; x < mbi.RegionSize; x += sizeof(uint32_t)) {
                    if (*(uint32_t*)(address + x) == target) {
                        // push back to address list if matches
                        addresses.push_back(address + x);
                    }
                }
            }
        }
        return true;
    }

    bool FilterHex(std::vector<size_t>& addresses, uint32_t target) {
        for (auto it = addresses.begin(); it != addresses.end();) {
            uint32_t buffer;
            SIZE_T bytesRead;

            if (ReadProcessMemory(GetCurrentProcess(), (LPCVOID)*it, &buffer, sizeof(uint32_t), &bytesRead)) {
                if (buffer == target) {
                    ++it; // matches
                }
                else {
                    it = addresses.erase(it); // doesnt match
                }
            }
            else {
                it = addresses.erase(it); // read failed
            }
        }
        return true;
    }

    bool ReplaceHex(const std::vector<size_t>& addresses, uint32_t newValue) {
        for (const auto& addr : addresses) {
            SIZE_T bytesWritten;
            if (WriteProcessMemory(GetCurrentProcess(), (LPVOID)addr, &newValue, sizeof(uint32_t), &bytesWritten) &&
                bytesWritten == sizeof(uint32_t)) {
            }
        }
        return true;
    }

};

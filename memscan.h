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

bool FindFloat(std::vector<size_t>& addresses, float target) {
    MEMORY_BASIC_INFORMATION mbi;

    for (size_t address = 0; VirtualQuery((PVOID)address, &mbi, sizeof mbi); address += mbi.RegionSize) {

        if (mbi.State == MEM_COMMIT && mbi.Protect == PAGE_READWRITE) {

            for (size_t x = 0; x < mbi.RegionSize; x += sizeof(float)) {

                if (*(float*)(address + x) == target) {
                    //push back to address list if matches
                    addresses.push_back(address + x);
                }
            }
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

bool FindAOB(std::vector<size_t>& addresses, const char* pattern, const char* mask) {
    MEMORY_BASIC_INFORMATION mbi;

    for (size_t address = 0; VirtualQuery((PVOID)address, &mbi, sizeof mbi); address += mbi.RegionSize) {

        if (mbi.State == MEM_COMMIT && mbi.Protect == PAGE_READWRITE) {

            for (size_t x = 0; x < mbi.RegionSize; ++x) {

                bool match = true;
                for (size_t y = 0; mask[y] != '\0'; ++y) {

                    if (mask[y] == 'x' && pattern[y] != *(unsigned char*)(address + x + y)) {
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

bool ReplaceAOB(const std::vector<size_t>& addresses, const char* newValue) {
    //loop through address
    for (const auto& addr : addresses) {
        SIZE_T bytesWritten;
        //write to address
        if (WriteProcessMemory(GetCurrentProcess(), (LPVOID)addr, newValue, strlen(newValue), &bytesWritten) &&
            bytesWritten == strlen(newValue)) {
        }
    }
    return true;
}

bool FindAOBHex(std::vector<size_t>& addresses, const char* pattern) {
    std::vector<unsigned char> patternBytes = HexStringToBytes(pattern);
    MEMORY_BASIC_INFORMATION mbi;

    for (size_t address = 0; VirtualQuery((PVOID)address, &mbi, sizeof mbi); address += mbi.RegionSize) {
        if (mbi.State == MEM_COMMIT && mbi.Protect == PAGE_READWRITE) {
            for (size_t x = 0; x < mbi.RegionSize - patternBytes.size(); ++x) {
                bool match = true;
                for (size_t y = 0; y < patternBytes.size(); ++y) {
                    if (*(unsigned char*)(address + x + y) != patternBytes[y]) {
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
    return !addresses.empty();
}

bool ReplaceAOBHex(const std::vector<size_t>& addresses, const char* newValue) {
    std::vector<unsigned char> newValueBytes = HexStringToBytes(newValue);

    //loop through addresses
    for (const auto& addr : addresses) {
        size_t newSize = newValueBytes.size();
        SIZE_T bytesWritten;

        //write to address
        if (WriteProcessMemory(GetCurrentProcess(), (LPVOID)addr, newValueBytes.data(), newSize, &bytesWritten) &&
            bytesWritten == newSize) {
            memset((void*)(addr + newSize), 0, 1);  //set one byte to null after the new value
        }
    }
    return true;
}


private:
    std::vector<unsigned char> HexStringToBytes(const char* hexString) {
        std::vector<unsigned char> bytes;
        std::istringstream hexStream(hexString);
        unsigned int byteValue;

        while (hexStream >> std::hex >> byteValue) {
            bytes.push_back(static_cast<unsigned char>(byteValue));
        }

        return bytes;
    }
};

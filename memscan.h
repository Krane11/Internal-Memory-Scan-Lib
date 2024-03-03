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

                        if (*(byte*)(address + x + y) != target[y]) {
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

    bool ReplaceString(const std::vector<size_t>& addresses, const std::string& oldValue, const std::string& newValue) {
        //loop through addresses
        for (const auto& addr : addresses) {
            size_t oldSize = oldValue.length();
            size_t newSize = newValue.length();
            SIZE_T bytesWritten;
            //write to the address
            if (WriteProcessMemory(GetCurrentProcess(), (LPVOID)addr, newValue.c_str(), newSize, &bytesWritten) &&
                bytesWritten == newSize) {
                size_t sizeDifference = newSize - oldSize;
                //adjust the surrounding data if diff size
                if (sizeDifference > 0) {
                    //new value is greater then old value
                    memmove((void*)(addr + newSize), (void*)(addr + oldSize), sizeDifference);
                }
                else if (sizeDifference < 0) {
                    //new value is less then old value
                    memset((void*)(addr + newSize), 0, -static_cast<ptrdiff_t>(sizeDifference));
                }
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

bool ReplaceUnicode(const std::vector<size_t>& addresses, const std::wstring& oldValue, const std::wstring& newValue) {
    //loop through addresses
    for (const auto& addr : addresses) {
        size_t oldSize = oldValue.length();
        size_t newSize = newValue.length();
        size_t lengthToWrite = std::min<size_t>(oldSize, newSize);
        //write to the address
        SIZE_T bytesWritten;
        if (WriteProcessMemory(GetCurrentProcess(), (LPVOID)addr, newValue.c_str(), lengthToWrite * sizeof(wchar_t), &bytesWritten) &&
            bytesWritten == lengthToWrite * sizeof(wchar_t)) {
            //adjust the surrounding data if diff size
            size_t sizeDifference = (newSize > oldSize) ? 0 : oldSize - newSize;
            if (sizeDifference > 0) {
                //new value is greater then old value
                memset((void*)(addr + newSize * sizeof(wchar_t)), 0, sizeDifference * sizeof(wchar_t));
            }
            else if (sizeDifference < 0) {
                //new value is smaller then the old value
                memmove((void*)(addr + newSize * sizeof(wchar_t)), (const void*)(addr + oldSize * sizeof(wchar_t)), -static_cast<ptrdiff_t>(sizeDifference) * sizeof(wchar_t));
            }
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

                    if (mask[y] == 'x' && pattern[y] != *(byte*)(address + x + y)) {
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
                    if (*(byte*)(address + x + y) != patternBytes[y]) {
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

bool ReplaceAOBHex(const std::vector<size_t>& addresses, const char* pattern, const char* newValue) {
    std::vector<unsigned char> patternBytes = HexStringToBytes(pattern);
    std::vector<unsigned char> newValueBytes = HexStringToBytes(newValue);
    //loop through addresses
    for (const auto& addr : addresses) {
        bool patternStillPresent = true;
        for (size_t y = 0; y < patternBytes.size(); ++y) {
            if (*(byte*)(addr + y) != patternBytes[y]) {
                patternStillPresent = false;
                break;
            }
        }

        if (patternStillPresent) {
            size_t lengthToWrite = std::min<size_t>(newValueBytes.size(), patternBytes.size());

            //write to the address
            SIZE_T bytesWritten;
            if (WriteProcessMemory(GetCurrentProcess(), (LPVOID)addr, newValueBytes.data(), lengthToWrite, &bytesWritten) &&
                bytesWritten == lengthToWrite) {
                size_t sizeDifference = patternBytes.size() - newValueBytes.size();
                //adjust the surrounding data if diff size
                if (sizeDifference > 0) {
                    //new value is less then old value
                    memset((void*)(addr + lengthToWrite), 0, sizeDifference);
                }
                else if (sizeDifference < 0) {
                    //new value is greater then old value
                    memmove((void*)(addr + lengthToWrite), (const void*)(addr + patternBytes.size()), -static_cast<ptrdiff_t>(sizeDifference));
                }
            }
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

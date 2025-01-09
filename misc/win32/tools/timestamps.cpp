#define NOMINMAX

#include <windows.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

long long _ModificationTimestamp(HANDLE file, const char *path)
{
    FILETIME ft;
    if (GetFileTime(file, NULL, NULL, &ft))
    {
        SYSTEMTIME stUTC, stLocal;
        FileTimeToSystemTime(&ft, &stUTC);
        SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

        struct tm tm = {0};
        tm.tm_year = stLocal.wYear - 1900;
        tm.tm_mon = stLocal.wMonth - 1;
        tm.tm_mday = stLocal.wDay;
        tm.tm_hour = stLocal.wHour;
        tm.tm_min = stLocal.wMinute;
        tm.tm_sec = stLocal.wSecond;

        time_t rawtime = mktime(&tm);
        return static_cast<long long>(rawtime);
    }
    else
    {
        fprintf(stderr, "Unable to get modification time of %s\n", path);
        exit(1);
    }
}

long long modificationTimestamp(const char *path)
{
    // Declare a variable to hold the result
    long long result = 0;

    DWORD fileAttributes = GetFileAttributesA(path);
    if (fileAttributes == INVALID_FILE_ATTRIBUTES)
    {
        fprintf(stderr, "Path %s does not exist!\n", path);
        exit(1);
    }

    if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        // If it's a directory, check modification time of the directory
        HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            fprintf(stderr, "Unable to open directory %s!\n", path);
            exit(1);
        }

        result = _ModificationTimestamp(hFile, path);

        WIN32_FIND_DATA findFileData;
        std::string dirPath = std::string(path);
        if (dirPath[dirPath.size() - 1] != '\\')
        {
            dirPath += '\\';
        }

        HANDLE hFind = FindFirstFileA((dirPath + "*").c_str(), &findFileData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0)
                {
                    std::string subPath = dirPath + findFileData.cFileName;
                    result = std::max(result, modificationTimestamp(subPath.c_str()));
                }
            } while (FindNextFileA(hFind, &findFileData) != 0);

            FindClose(hFind);
        }
        CloseHandle(hFile);
    }
    else
    {
        // If it's a file, get its modification time
        HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            fprintf(stderr, "Unable to open file %s!\n", path);
            exit(1);
        }
        result = _ModificationTimestamp(hFile, path);
        CloseHandle(hFile);
    }

    return result;
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage %s [path1] [path2] ... [path_n]\n", argv[0]);
    }
    long long result = 0;
    for (int i = 1; i < argc; ++i)
    {
        result = std::max(result, modificationTimestamp(argv[i]));
    }

    printf("%lld\n", result);

    return 0;
}
#include "Globals.h"
#include "FileAbstraction.h"
#include "Supervisor.h"

D3DFORMAT g_d3dFormats[] = { D3DFMT_UNKNOWN, D3DFMT_A8R8G8B8, D3DFMT_A1R5G5B5, D3DFMT_R5G6B5, D3DFMT_R8G8B8, D3DFMT_A4R4G4B4 };
uint32_t g_bytesPerPixelLookupTable[] = { 4, 4, 2, 2, 3, 2, 0, 1, 2 };
const char* g_shotFiles[] = {
    "pl00a.sht", "pl00b.sht", "pl00c.sht",
    "pl01a.sht", "pl01b.sht", "pl01c.sht"
};

float g_playerHurtboxes[2] = {2.0f, 3.5f};
float g_playerItemAttractBoxes[2] = {60.0f, 70.0f};
float g_playerItemAttractBoxesFocused[2] = {100.0f, 118.0f};
float g_playerRelated_5_or_7[2] = {5.0, 7.0};

byte* openFile(const char* filename, size_t* outSize, BOOL isExternalResource)
{
    printf("Opening %s\n", filename);
    byte* outBuffer;
    int decompressedSize;
    
    g_supervisor.enterCriticalSection(2);

    // Find existing file
    if (isExternalResource == 0)
    {
        // Finds last backslash in path
        const char* lastBackslash = strrchr(filename, '\\');
        const char* baseFilename = filename;
        if (lastBackslash)
            baseFilename = lastBackslash + 1;
        
        // Finds last forwardslash in path
        baseFilename = strrchr(baseFilename, '/');
        if (baseFilename)
            filename = baseFilename + 1;

        PbgArchiveEntry* entries = g_pbgArchive.m_entries;
        if (!entries)
        {
            g_supervisor.leaveCriticalSection(2);
            return nullptr;
        }

        bool foundArchive = false;
        int i = g_pbgArchive.m_numEntries;
        while (i)
        {
            if (_stricmp(filename, entries->filename) == 0)
            {
                foundArchive = true;
                break;
            }
            --i;
            ++entries;
        }

        if (!foundArchive)
        {
            printf("openFile: Archive not found!\n");
            if (outSize)
                *outSize = 0;
            return nullptr;
        }

        decompressedSize = entries->decompressedSize;
        if (!decompressedSize)
        {
            g_supervisor.leaveCriticalSection(2);
            return nullptr;
        }

        // Update outSize if it's asked for
        if (outSize) 
            *outSize = decompressedSize;

        // Decrypt entry
        printf("Decoding %s\n", filename);
        //outBuffer = new byte[decompressedSize];
        outBuffer = (byte*)game_malloc(decompressedSize);

        if (!outBuffer)
        {
            printf("openFile: Buffer allocation failed!\n");
            g_supervisor.leaveCriticalSection(2);
            return nullptr;
        }
        g_pbgArchive.readDecompressEntry(&g_pbgArchive, outBuffer, filename);
        g_supervisor.leaveCriticalSection(2);
        return outBuffer;
    }

    // Load from file
    else
    {
        printf("Loading %s\n", filename);
        HANDLE file = CreateFileA(
            filename,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (file == INVALID_HANDLE_VALUE)
        {
            printf("Error: %s was not found.\n", filename);
            g_supervisor.leaveCriticalSection(2);
            return nullptr;
        }

        DWORD fileSize = GetFileSize(file,NULL);

        //outBuffer = new byte[fileSize];
        outBuffer = (byte*)game_malloc(fileSize);

        if (!outBuffer)
        {
            printf("Error: %s allocation error.\n", filename);
            CloseHandle(file);
            g_supervisor.leaveCriticalSection(2);
            return nullptr;
        }

        ReadFile(
            file,
            outBuffer,
            fileSize,
            &fileSize,
            NULL
        );

        if (outSize)
            *outSize = fileSize;

        CloseHandle(file);
        g_supervisor.leaveCriticalSection(2);
        return outBuffer;

    }
}

// 0x458670
int writeToFile(LPCSTR fileName, DWORD numBytes, LPVOID bytes)
{
    DWORD dwMessageId;
    DWORD dwLanguageId;
    DWORD nSize;
    DWORD numBytesWritten;
    
    g_supervisor.enterCriticalSection(2);
    HANDLE hFile = CreateFileA(
        fileName,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    if (hFile == INVALID_HANDLE_VALUE)
    {
        nSize = 0;
        dwLanguageId = 0x400;
        dwMessageId = GetLastError();
        FormatMessageA(
            0x1300,
            NULL,
            dwMessageId,
            dwLanguageId,
            (LPSTR)&bytes,
            nSize,
            NULL
        );
        printf("error : %s write error\n",fileName);
        LocalFree(bytes);
        g_supervisor.leaveCriticalSection(2);
        return -1;
    }

    WriteFile(hFile,
        bytes,
        numBytes,
        &numBytesWritten,
        NULL
    );

    if (numBytes != numBytesWritten)
    {
        CloseHandle(hFile);
        printf("error : %s write error\n",fileName);
        g_supervisor.leaveCriticalSection(2);
        return -2;
    }
    CloseHandle(hFile);
    printf("%s write ...\n",fileName);
    g_supervisor.leaveCriticalSection(2);
    return 0;
}

// 0x460885
int createDirectory(LPCSTR pathName)
{
    BOOL status = CreateDirectoryA(pathName, NULL);
    if (!status)
    {
        uint32_t error = GetLastError();
        std::cerr << error << "\n";
        return -1;
    }
    return 0;
}

// 0x4585e0
int fileExists(LPCSTR filePath)
{
    g_supervisor.enterCriticalSection(2);
    HANDLE fileHandle = CreateFileA(
        filePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(fileHandle);
        g_supervisor.leaveCriticalSection(2);
        return 1;
    }

    g_supervisor.leaveCriticalSection(2);
    return 0;
}

void loadTh11Dat()
{
    size_t outSize;
    char buffer[128];

    if (!g_pbgArchive.load("th11.dat"))
    {
        printf("Could not save data file\n");
        return;
    }
    sprintf_s(buffer, "th11_%.4x%c.ver", 0x100, 0x61);
    g_supervisor.th11DatBytes = openFile(buffer, &outSize, 0);
    g_supervisor.th11DatSize = outSize;
    if (!g_supervisor.th11DatBytes)
    {
        printf("Data version is incorrect\n");
        g_supervisor.th11DatBytes = nullptr;
    }
}

void projectMagnitudeToVectorComponents(D3DXVECTOR3* vec, float theta, float scale)
{
    float c = std::cosf(theta);
    float s = std::sinf(theta);
    vec->x = (float)(c * scale);
    vec->y = (float)(s * scale);
}

float normalizeAngle(float inputAngleRadians)
{
    return std::remainder(inputAngleRadians, 2.0f * D3DX_PI);
}

void decomposeAngle(Float2* outVec, float angle, float scale)
{
    outVec->x = std::cos(angle) * scale;
    outVec->y = std::sin(angle) * scale;
}

void decomposeAngle(Float3* outVec, float angle, float scale)
{
    outVec->x = std::cos(angle) * scale;
    outVec->y = std::sin(angle) * scale;
}
#include "AnmManager.h"
#include "AnmVm.h"

AnmManager* g_anmManager;

void AnmManager::preloadAnm(int anmIdx, char* anmFileName)
{
    // Skip if already loaded
    if (g_anmManager->loadedAnms[anmIdx]) 
    {
        printf("::preloadAnim already : %s\n",anmFileName);
        return;
    }
    g_anmManager->preloadAnmFromMemory(anmFileName,anmIdx);

    if (!g_anmManager)
        return;

    // Reused as a loading flag
    g_anmManager->bulkVms[0].timeInScript.gameSpeed = (float*)1;
    do {
        if ((g_supervisor.criticalSectionFlag & 0x180) != 0) break;
        Sleep(1);
    } while (g_anmManager->bulkVms[0].timeInScript.gameSpeed != (float*)0);

    printf("::preloadAnimEnd : %s\n",anmFileName);
    return;
}

// Function to process a single ANM chunk
int openAnmLoaded(AnmLoaded* anmLoaded, AnmHeader* chunk, int chunkIndex)
{
    // Check if the version is 7 (specific to this ANM format)
    if (chunk->version != 7)
    {
        printf("アニメのバージョンが違います\n");
        return -1;
    }

    // If hasdata is 0, load an external texture file
    if (chunk->hasData == 0)
    {
        char* name = (char*)chunk + chunk->nameOffset;

        // Skip if name starts with '@' (special case)
        if (*name != '@')
        {
            char filePath[260];
            sprintf_s(filePath, "%s", name);
            size_t outSize;
            uint32_t* memoryMappedFile = (uint32_t*)openFile(filePath, &outSize, 1);
            if (memoryMappedFile == nullptr)
            {
                printf("テクスチャ %s が読み込めません。データが失われてるか壊れています\n", name);
                return -1;
            }
            // Store the loaded file and size in the chunk data buffer
            anmLoaded->anmLoadedD3D[chunkIndex].m_srcData = memoryMappedFile;
            anmLoaded->anmLoadedD3D[chunkIndex].m_srcDataSize = outSize;
        }
    }
    return 1;
}

// Function to preload an ANM file into memory
void AnmManager::preloadAnmFromMemory(char* anmFilePath, int anmSlotIndex)
{
    // Log the preload action
    printf("::preloadAnim : %s\n", anmFilePath);

    // Check if the slot index is within bounds (0-31)
    if (anmSlotIndex >= 32)
    {
        printf("テクスチャ格納先が足りません\n");
        return;
    }

    // Resolve the file path
    char resolvedFilePath[260];
    sprintf_s(resolvedFilePath, "%s", anmFilePath);

    // Open the ANM file (memory-mapped)
    AnmHeader* header = (AnmHeader*)openFile(resolvedFilePath, nullptr, 0);
    if (!header)
    {
        printf("アニメが読み込めません。データが失われてるか壊れています\n");
        return;
    }

    // Allocate and initialize the AnmLoaded structure
    AnmLoaded* anmLoaded = (AnmLoaded*) malloc(sizeof(AnmLoaded));
    if (anmLoaded)
        memset(anmLoaded, 0, sizeof(AnmLoaded));
    loadedAnms[anmSlotIndex] = anmLoaded;

    // Set initial fields
    anmLoaded->anmSlotIndex = anmSlotIndex;
    anmLoaded->header = header;
    strcpy_s(anmLoaded->filePath, anmFilePath);

    // Parse the ANM header and count chunks, sprites, and scripts
    AnmHeader* chunk = (AnmHeader*)header;
    int numSprites = chunk->numSprites;
    int numScripts = chunk->numScripts;
    int processedCount = 1;
    uint32_t nextOffset = chunk->nextOffset;
    AnmHeader* currentChunk = chunk;

    while (nextOffset != 0)
    {
        currentChunk = (AnmHeader*)((char*)currentChunk + nextOffset);
        numSprites += currentChunk->numSprites;
        numScripts += currentChunk->numScripts;
        processedCount++;
        nextOffset = currentChunk->nextOffset;
    }

    // Allocate buffers for chunk data, keyframes, and sprites
    anmLoaded->numAnmLoadedD3Ds = processedCount;
    anmLoaded->header = (AnmHeader*) malloc(processedCount * sizeof(AnmHeader));
    memset(anmLoaded->header, 0, processedCount * sizeof(AnmHeader));
    anmLoaded->keyframeData = malloc(numSprites * 72);
    anmLoaded->spriteData = malloc(numScripts * 4);
    anmLoaded->numScripts = numScripts;
    anmLoaded->numSprites = numSprites;

    // Process each chunk
    int chunkIndex = 0;
    currentChunk = header;
    while (currentChunk != nullptr)
    {
        int result = openAnmLoaded(anmLoaded, currentChunk, chunkIndex);
        if (result < 0)
        {
            printf("アニメが読み込めません。データが失われてるか壊れています\n");
            free(anmLoaded->header);
            free(anmLoaded->keyframeData);
            free(anmLoaded->spriteData);
            free(anmLoaded);
            loadedAnms[anmSlotIndex] = nullptr;
            return;
        }
        chunkIndex++;
        if (currentChunk->nextOffset == 0) 
            break;
        currentChunk = (AnmHeader*)((byte*)currentChunk + currentChunk->nextOffset);
    }
}

void AnmManager::markAnmLoadedAsReleasedInVmList(AnmLoaded* anmLoaded)
{
    AnmVmList* temp = this->primaryGlobalNext;
    while (temp)
    {
        AnmVmList* next = temp->next;
        AnmVm* entry = temp->entry;
        temp = next;
        if (entry->anmLoaded == anmLoaded)
        entry->flagsLow |= 0x4000000;
    }
    temp = this->secondaryGlobalNext;
    while (temp)
    {
        AnmVmList* next = temp->next;
        AnmVm* entry = temp->entry;
        temp = next;
        if (entry->anmLoaded == anmLoaded)
            entry->flagsLow |= 0x4000000;
    }
}

AnmVm* AnmManager::allocateVm()
{
    int index = g_anmManager->nextBulkVmIndex;
    AnmVm* vm;

    if (g_anmManager->bulkVmsIsAlive[index] == 0)
    {
        vm = &g_anmManager->bulkVms[index];
        g_anmManager->bulkVmsIsAlive[index] = 1;
        g_anmManager->nextBulkVmIndex = g_anmManager->getNextBulkVmIndex(index);
    }
    else
    {
        int nextIdx = getNextBulkVmIndex(index);
        if (g_anmManager->bulkVmsIsAlive[nextIdx] == 0)
        {
            vm = &g_anmManager->bulkVms[nextIdx];
            g_anmManager->bulkVmsIsAlive[nextIdx] = 1;
        }
        else
        {
            vm = new AnmVm();
            if (vm)
                vm->initialize();
        }
        g_anmManager->nextBulkVmIndex = getNextBulkVmIndex(nextIdx);
    }
    return vm;
}

// 0x445320
void AnmManager::releaseTextures()
{
    for (int i = 0; i < NUM_ANM_LOADEDS; ++i)
    {
        AnmLoaded* anmLoaded = g_anmManager->loadedAnms[i];
        if (anmLoaded == nullptr)
            continue; // Skip null entries

        AnmLoadedD3D* anmLoadedD3Ds = anmLoaded->anmLoadedD3D;
        int numAnmLoadedD3Ds = anmLoaded->numAnmLoadedD3Ds;
        for (int j = 0; j < numAnmLoadedD3Ds; ++j)
        {
            AnmLoadedD3D* entry = &anmLoadedD3Ds[j];
            if ((entry->m_flags & 1) != 0 && entry->m_texture != nullptr)
            {
                entry->m_texture->Release();
                entry->m_texture = nullptr;
            }
        }
    }
}

// 0x44fd10
void AnmManager::flushSprites()
{
    if ((this->vertexBuffers).leftoverSpriteCount == 0)
        return;

    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
    g_supervisor.d3dDevice->SetFVF(0x144);

    g_supervisor.d3dDevice->DrawPrimitiveUP(
        D3DPT_TRIANGLELIST,
        this->vertexBuffers.leftoverSpriteCount * 2,
        this->vertexBuffers.spriteRenderCursor,
        0x1c
    );

    ++this->refreshCounter;
    this->vertexBuffers.spriteRenderCursor = this->vertexBuffers.spriteWriteCursor;
    this->vertexBuffers.leftoverSpriteCount = 0;
}

// 0x454ec0
void AnmManager::blitTextureToSurface(BlitParams* blitParams)
{
    // Early return if the texture is null
    if (!this->loadedAnms[blitParams->anmLoadedIndex]->anmLoadedD3D[blitParams->anmLoadedD3dIndex].m_texture)
        return;

    // Ensure pending sprite operations are completed
    flushSprites();

    // Retrieve the back buffer from the Direct3D device
    IDirect3DSurface9* backBuffer = nullptr;
    HRESULT hr = g_supervisor.d3dDevice->GetBackBuffer(
        0,
        0,
        D3DBACKBUFFER_TYPE_MONO,
        &backBuffer
    );
    if (FAILED(hr))
        return;

    // Access the texture from the AnmLoadedD3D array
    AnmLoadedD3D* anmLoadedD3d = this->loadedAnms[blitParams->anmLoadedIndex]->anmLoadedD3D;
    IDirect3DTexture9* texture = anmLoadedD3d[blitParams->anmLoadedD3dIndex].m_texture;

    // Get the surface from the texture (level 0)
    IDirect3DSurface9* textureSurface = nullptr;
    hr = texture->GetSurfaceLevel(0, &textureSurface);
    if (FAILED(hr))
    {
        backBuffer->Release();
        return;
    }

    // Set up source rectangle (assuming r1.right and r1.bottom are width and height)
    RECT srcRect;
    srcRect.left = blitParams->srcRect.left;
    srcRect.top = blitParams->srcRect.top;
    srcRect.right = blitParams->srcRect.left + blitParams->srcRect.right;
    srcRect.bottom = blitParams->srcRect.top + blitParams->srcRect.bottom;

    // Set up destination rectangle (assuming r2.right and r2.bottom are width and height)
    RECT dstRect;
    dstRect.left = blitParams->destRect.left;
    dstRect.top = blitParams->destRect.top;
    dstRect.right = blitParams->destRect.left + blitParams->destRect.right;
    dstRect.bottom = blitParams->destRect.top + blitParams->destRect.bottom;

    // Copy the region from the back buffer to the texture surface
    hr = D3DXLoadSurfaceFromSurface(
        textureSurface, nullptr, &dstRect,
        backBuffer, nullptr, &srcRect,
        2, 0); // Filter and color key values preserved from original

    // If the copy succeeded, mark the texture as dirty
    if (SUCCEEDED(hr))
        texture->AddDirtyRect(nullptr);

    // Release resources
    textureSurface->Release();
    backBuffer->Release();
}

/* Globals */

void blitTextures()
{
    for (int i = 0; i < 4; ++i)
    {
        BlitParams* blitParams = &g_anmManager->blitParamsArray[i];
        if (blitParams->anmLoadedIndex >= 0)
        {
            g_anmManager->blitTextureToSurface(blitParams);
            blitParams->anmLoadedIndex = -1;
        }
    }
}
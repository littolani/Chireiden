#include "AnmManager.h"
#include "AnmLoaded.h"
#include "Globals.h"
#include "ThunkGenerator.h"

AnmManager* AnmManager::initialize(AnmManager* This)
{
    //memset(This, 0, sizeof(AnmManager));

    This->m_primaryVm = AnmVm();
    for (int i = 0; i < 4096; ++i)
        This->m_fastVms[i].initialize(&This->m_fastVms[i]);

    g_renderQuad144[0].uv.x = 0.0f; g_renderQuad144[0].uv.y = 0.0f; g_renderQuad144[0].rhw = 1.0f;
    g_renderQuad144[1].uv.x = 1.0f; g_renderQuad144[1].uv.y = 0.0f; g_renderQuad144[1].rhw = 1.0f;
    g_renderQuad144[2].uv.x = 0.0f; g_renderQuad144[2].uv.y = 1.0f; g_renderQuad144[2].rhw = 1.0f;
    g_renderQuad144[3].uv.x = 1.0f; g_renderQuad144[3].uv.y = 1.0f; g_renderQuad144[3].rhw = 1.0f;

    This->m_squareVertexBuffer = nullptr;
    This->m_anmLoadedD3D = nullptr;
    This->m_renderStateMode = 0;
    This->m_unk_4355c1 = 0;
    This->m_unk_4355c3 = 0;
    This->m_unk_4355c4 = -1;
    This->m_color = 1;
    This->m_haveFlushedSprites = 0;

    for (int i = 0; i < 4; i++)
        This->m_blitParamsArray[i].anmLoadedIndex = -1;

    auto addChain = [&](int priority, ChainCallback callback, bool isCalcChain) -> void
    {
        ChainElem* elem = (ChainElem*)game_new(sizeof(ChainElem));
        elem->nextNode = (ChainElem*)((uintptr_t)elem->nextNode & 0xfffffffe);
        elem->trackerPrevNode = nullptr;
        elem->registerChainCallback = nullptr;
        elem->runCalcChainCallback = nullptr;
        elem->jobPriority = 0;
        elem->embeddedTracker.trackerJobNode = elem;
        elem->nextNode = (ChainElem*)((uintptr_t)elem->nextNode | 3);
        elem->jobRunDrawChainCallback = callback;
        elem->args = This;

        if (isCalcChain)
        {
            //using sto = Storage<EAX, ESI, EBX>;
            //using sig = Signature<int, ChainElem*, int>;
            //static auto registerCalcChain = createCustomCallingConvention<sto, sig>(0x456b70);
            //registerCalcChain(elem, priority);
             g_chain->registerCalcChain(elem, priority);
        }
        else
        {
            //using sto = Storage<EAX, ESI, EBX>;
            //using sig = Signature<int, ChainElem*, int>;
            //static auto registerDrawChain = createCustomCallingConvention<sto, sig>(0x456c10);
            //registerDrawChain(elem, priority);
             g_chain->registerDrawChain(elem, priority);
        }
    };

    addChain(0x1a, reinterpret_cast<ChainCallback>(0x455480)/*onTick1a*/, true);
    addChain(0x08, reinterpret_cast<ChainCallback>(0x455560)/*onTick08*/, true);
    addChain(0x04, reinterpret_cast<ChainCallback>(0x455140)/*on_draw_04_just_renders_layer_00*/, false);
    addChain(0x06, reinterpret_cast<ChainCallback>(0x455150)/*on_draw_06_just_renders_layer_01*/, false);
    addChain(0x08, reinterpret_cast<ChainCallback>(0x455160)/*on_draw_08_just_renders_layer_02*/, false);
    addChain(0x0a, reinterpret_cast<ChainCallback>(0x455170)/*on_draw_0a_also_renders_layer_03*/, false);
    addChain(0x0c, reinterpret_cast<ChainCallback>(0x4551f0)/*on_draw_0c_just_renders_layer_04*/, false);
    addChain(0x0f, reinterpret_cast<ChainCallback>(0x455200)/*on_draw_0f_just_renders_layer_05*/, false);
    addChain(0x10, reinterpret_cast<ChainCallback>(0x455210)/*on_draw_10_just_renders_layer_06*/, false);
    addChain(0x11, reinterpret_cast<ChainCallback>(0x455220)/*on_draw_11_just_renders_layer_07*/, false);
    addChain(0x12, reinterpret_cast<ChainCallback>(0x455230)/*on_draw_12_just_renders_layer_08*/, false);
    addChain(0x13, reinterpret_cast<ChainCallback>(0x455240)/*on_draw_13_just_renders_layer_09*/, false);
    addChain(0x14, reinterpret_cast<ChainCallback>(0x455250)/*on_draw_15_just_renders_layer_10*/, false);
    addChain(0x17, reinterpret_cast<ChainCallback>(0x455260)/*on_draw_17_just_renders_layer_11*/, false);
    addChain(0x18, reinterpret_cast<ChainCallback>(0x455270)/*on_draw_18_just_renders_layer_12*/, false);
    addChain(0x1a, reinterpret_cast<ChainCallback>(0x455280)/*on_draw_1a_just_renders_layer_13*/, false);
    addChain(0x1c, reinterpret_cast<ChainCallback>(0x455290)/*on_draw_1c_just_renders_layer_14*/, false);
    addChain(0x20, reinterpret_cast<ChainCallback>(0x4552a0)/*on_draw_20_just_renders_layer_15*/, false);
    addChain(0x22, reinterpret_cast<ChainCallback>(0x4552b0)/*on_draw_22_just_renders_layer_16*/, false);
    addChain(0x24, reinterpret_cast<ChainCallback>(0x4552c0)/*on_draw_24_just_renders_layer_17*/, false);
    addChain(0x27, reinterpret_cast<ChainCallback>(0x4552d0)/*on_draw_27_just_renders_layer_18*/, false);
    addChain(0x28, reinterpret_cast<ChainCallback>(0x455320)/*on_draw_28_also_renders_layer_19*/, false);
    addChain(0x32, reinterpret_cast<ChainCallback>(0x455300)/*on_draw_32_just_renders_layer_22*/, false);
    addChain(0x3c, reinterpret_cast<ChainCallback>(0x4552f0)/*on_draw_3c_just_renders_layer_23*/, false);
    addChain(0x3d, reinterpret_cast<ChainCallback>(0x4552e0)/*on_draw_3d_just_renders_layer_24*/, false);
    addChain(0x31, reinterpret_cast<ChainCallback>(0x455310)/*on_draw_31_just_renders_layer_21*/, false);
    addChain(0x30, reinterpret_cast<ChainCallback>(0x455350)/*on_draw_30_also_renders_layer_20*/, false);
    addChain(0x3f, reinterpret_cast<ChainCallback>(0x4553a0)/*on_draw_3f_also_renders_layer_29*/, false);
    addChain(0x3e, reinterpret_cast<ChainCallback>(0x4553f0)/*on_draw_3e_also_renders_layer_30*/, false);
    return This;
}

AnmManager::~AnmManager()
{
    AnmVmListNode* primaryHead = m_primaryGlobalHead;
    while (primaryHead)
    {
        AnmVmListNode* primaryNext = primaryHead->next;
        removeVm(this, primaryHead->entry);
        primaryHead = primaryNext;
    }

    AnmVmListNode* secondaryHead = m_secondaryGlobalHead;
    while (secondaryHead)
    {
        AnmVmListNode* secondaryNext = secondaryHead->next;
        removeVm(this, secondaryHead->entry);
        secondaryHead = secondaryNext;
    }

    if (m_primaryVm.m_specialRenderData)
          game_free(m_primaryVm.m_specialRenderData);
    m_primaryVm.m_specialRenderData = nullptr;
}

void AnmManager::removeVm(AnmManager* This, AnmVm* vm)
{
    AnmVmListNode* node = &vm->m_globalListNode;
    if (This->m_primaryGlobalTail == node)
        This->m_primaryGlobalTail = node->prev;

    if (This->m_primaryGlobalHead == node)
        This->m_primaryGlobalHead = node->next;

    if (This->m_secondaryGlobalTail == node)
        This->m_secondaryGlobalTail = node->prev;

    if (This->m_secondaryGlobalHead == node)
        This->m_secondaryGlobalHead = node->next;

    if (node->next)
        node->next->prev = node->prev;

    if (node->prev)
        node->prev->next = node->next;

    node->next = nullptr;
    node->prev = nullptr;

    AnmVmListNode* familyNode = &vm->m_familyListNode;

    if (familyNode->next)
        familyNode->next->prev = familyNode->prev;

    if (familyNode->prev)
        familyNode->prev->next = familyNode->next;

    familyNode->next = nullptr;
    familyNode->prev = nullptr;

    if (vm >= This->m_fastVms && vm < (This->m_fastVms + 4096))
    {
        int index = vm - This->m_fastVms;
        This->m_fastVmsIsAlive[index] = 0; // Mark this slot as free (dead) in the allocation map

        if (vm->m_specialRenderData)
            game_free(vm->m_specialRenderData);

        vm->m_specialRenderData = nullptr;

        // Re-initialize the VM structure for reuse instead of deleting it
        vm->initialize(vm);
        return;
    }

    // Heap VM Cleanup (Slow path)
    if (vm)
    {
        if (vm->m_specialRenderData)
            game_free(vm->m_specialRenderData);

        vm->m_specialRenderData = nullptr;
        game_free(vm);
    }
}

void AnmManager::addVm(AnmVm* vm, AnmId* outAnmId)
{
    AnmVmListNode* newNode = &vm->m_globalListNode;
    newNode->entry = vm;
    vm->m_globalListNode.next = nullptr;
    vm->m_globalListNode.prev = nullptr;

    AnmVmListNode* globalHead = g_anmManager->m_primaryGlobalHead;
    if (!globalHead)
        g_anmManager->m_primaryGlobalTail = newNode;
    else
    {
        // Dead code because of inlining
        //AnmVmListNode* globalNext = vm->m_globalListNode.next;
        //if (globalNext)
        //{
        //    globalHead->next = globalNext;
        //    ((vm->m_globalListNode).next)->prev = globalHead;
        //}
        vm->m_globalListNode.next = globalHead;
        globalHead->prev = newNode;
    }
    g_anmManager->m_primaryGlobalHead = newNode;

    // 1-indexed?
    ++g_anmManager->m_id;
    if (g_anmManager->m_id == 0)
        ++g_anmManager->m_id;

    vm->m_id.id = g_anmManager->m_id;
    outAnmId->id = g_anmManager->m_id;
}

AnmLoaded* AnmManager::preloadAnm(int anmSlotIndex, const char* anmFileName)
{
    AnmManager* This = g_anmManager;

    // Return existing animation if already loaded
    if (This->m_loadedAnms[anmSlotIndex] != nullptr)
    {
        printf("preloadAnm: already loaded %s\n", anmFileName);
        return This->m_loadedAnms[anmSlotIndex];
    }

    // Load animation from memory
    AnmLoaded* anmLoaded = preloadAnmFromMemory(This, anmSlotIndex, anmFileName);
    if (anmLoaded == nullptr)
        return nullptr;

    anmLoaded->m_anmsLoading = 1;
    while (anmLoaded->m_anmsLoading != 0 && (g_supervisor.flags & 0x180) == 0) // Wait until animation loading is complete
        Sleep(1);

    printf("preloadAnm: %s\n", anmFileName);
    return anmLoaded;
}

int AnmManager::openAnmLoaded(AnmLoaded* anmLoaded, AnmHeader* anmHeader, int chunkIndex)
{
    // Check if the version is 7 (specific to this ANM format)
    if (anmHeader->version != 7)
    {
        printf("ANM version is incorrect\n");
        return -1;
    }

    // If hasdata is 0, load an external texture file
    if (anmHeader->hasData == 0)
    {
        char* name = (char*)anmHeader + anmHeader->nameOffset;

        // Skip if name starts with '@' (special case)
        if (*name != '@')
        {
            char filePath[260];
            sprintf_s(filePath, "%s", name);
            size_t outSize;
            uint32_t* memoryMappedFile = (uint32_t*)openFile(filePath, &outSize, 1);
            if (memoryMappedFile == nullptr)
            {
                printf(" %s \n", name);
                return -1;
            }
            // Store the loaded file and size in the chunk data buffer
            anmLoaded->m_anmLoadedD3D[chunkIndex].m_srcData = memoryMappedFile;
            anmLoaded->m_anmLoadedD3D[chunkIndex].m_srcDataSize = outSize;
        }
    }
    return 1;
}

void AnmManager::setupRenderSquare()
{
    AnmManager* This = g_anmManager;
    RenderVertexSq* verts = reinterpret_cast<RenderVertexSq*>(&This->m_squareVertices[0]);

    verts[0].position = { -128.0f, -128.0f, 0.0f }; // Vertex 0: Bottom-Left
    verts[0].uv = { 0.0f, 0.0f };
    verts[1].position = { 128.0f, -128.0f, 0.0f }; // Vertex 1: Bottom-Right
    verts[1].uv = { 1.0f, 0.0f };
    verts[2].position = { -128.0f, 128.0f, 0.0f }; // Vertex 2: Top-Left
    verts[2].uv = { 0.0f, 1.0f };
    verts[3].position = { 128.0f, 128.0f, 0.0f }; // Vertex 3: Top-Right
    verts[3].uv = { 1.0f, 1.0f };
    memcpy(g_squareVertices, verts, sizeof(RenderVertexSq) * 4);

    HRESULT hr = g_supervisor.d3dDevice->CreateVertexBuffer(
        sizeof(RenderVertexSq) * 4,
        0,
        D3DFVF_XYZ | D3DFVF_TEX1,
        D3DPOOL_MANAGED,
        &This->m_squareVertexBuffer,
        NULL
    );

    if (FAILED(hr))
    {
        puts("setupRenderSquare failed to create vertex buffer!\n");
        return;
    }

    void* data = nullptr;
    if (SUCCEEDED(This->m_squareVertexBuffer->Lock(0, 0, &data, 0)))
    {

        memcpy(data, verts, sizeof(RenderVertexSq) * 4); // Copy 80 bytes from local array to VRAM
        This->m_squareVertexBuffer->Unlock();
    }

    g_supervisor.d3dDevice->SetStreamSource(
        0,
        This->m_squareVertexBuffer,
        0,
        sizeof(RenderVertexSq)
    );
}

AnmLoaded* AnmManager::preloadAnmFromMemory(AnmManager* This, int anmSlotIndex, const char* anmFilePath)
{
    printf("preloadAnmFromMemory: %s\n", anmFilePath);

    // Check if the slot index is within bounds (0-31)
    if (anmSlotIndex >= 32)
    {
        printf("Anm slot index %d is out of bounds!\n", anmSlotIndex);
        return nullptr;
    }

    // Resolve the file path
    char resolvedFilePath[260];
    sprintf_s(resolvedFilePath, "%s", anmFilePath);

    AnmHeader* header = (AnmHeader*)openFile(resolvedFilePath, nullptr, 0);
    if (!header)
    {
        printf("Anm header is NULL!\n");
        return nullptr;
    }

    // Allocate and initialize the AnmLoaded structure
    AnmLoaded* anmLoaded = (AnmLoaded*)game_new(sizeof(AnmLoaded));
    if (!anmLoaded)
    {
        printf("Unable to allocate anmLoaded!\n");
        return nullptr;
    }
    memset(anmLoaded, 0, sizeof(AnmLoaded));
    This->m_loadedAnms[anmSlotIndex] = anmLoaded;

    // Set initial fields
    anmLoaded->m_anmSlotIndex = anmSlotIndex;
    anmLoaded->m_header = header;
    strcpy_s(anmLoaded->m_filePath, anmFilePath);

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

    anmLoaded->m_numAnmLoadedD3Ds = processedCount;

    anmLoaded->m_anmLoadedD3D = (AnmLoadedD3D*)game_malloc(processedCount * sizeof(AnmLoadedD3D));
    memset(anmLoaded->m_anmLoadedD3D, 0, processedCount * sizeof(AnmLoadedD3D));

    //anmLoaded->m_keyframeData = new AnmLoadedSprite[numSprites]();
    anmLoaded->m_keyframeData = (AnmLoadedSprite*)game_malloc(numSprites * sizeof(AnmLoadedSprite));

    //anmLoaded->m_spriteData = new uint32_t[numScripts]();
    anmLoaded->m_spriteData = (uint32_t*)game_malloc(numScripts * sizeof(uint32_t));

    anmLoaded->m_numScripts = numScripts;
    anmLoaded->m_numSprites = numSprites;

    // Process each chunk
    int chunkIndex = 0;
    currentChunk = header;
    while (currentChunk != nullptr)
    {
        int result = openAnmLoaded(anmLoaded, currentChunk, chunkIndex);
        if (result < 0)
        {
            printf("Could not read ANM data.\n");
            game_free(anmLoaded->m_header);
            game_free(anmLoaded->m_keyframeData);
            game_free(anmLoaded->m_spriteData);
            game_free(anmLoaded);
            This->m_loadedAnms[anmSlotIndex] = nullptr;
            return nullptr;
        }
        chunkIndex++;
        if (currentChunk->nextOffset == 0)
            break;
        currentChunk = (AnmHeader*)((byte*)currentChunk + currentChunk->nextOffset);
    }
    return anmLoaded;
}

void AnmManager::markAnmLoadedAsReleasedInVmList(AnmManager* This, AnmLoaded* anmLoaded)
{
    AnmVmListNode* temp = This->m_primaryGlobalHead;
    while (temp)
    {
        AnmVmListNode* next = temp->next;
        AnmVm* entry = temp->entry;
        temp = next;
        if (entry->m_anmLoaded == anmLoaded)
            entry->m_flagsLow |= 0x4000000;
    }
    temp = This->m_secondaryGlobalHead;
    while (temp)
    {
        AnmVmListNode* next = temp->next;
        AnmVm* entry = temp->entry;
        temp = next;
        if (entry->m_anmLoaded == anmLoaded)
            entry->m_flagsLow |= 0x4000000;
    }
}

AnmVm* AnmManager::allocateVm()
{
    AnmManager* This = g_anmManager;
    int index = This->m_nextFastVmIndex;

    if (This->m_fastVmsIsAlive[index] != 0)
    {
        // Current slot busy. Move cursor to next and try again.
        index = This->getNextFastVmIndex(index);
        This->m_nextFastVmIndex = index;

        // Is the next slot also alive?
        if (This->m_fastVmsIsAlive[index] != 0)
        {
            // Both preferred slots are busy. Allocate an overflow VM from the heap.
            AnmVm* vm = (AnmVm*)game_new(sizeof(AnmVm));
            if (!vm)
            {
                puts("Could not allocate AnmVm!\n");
                return nullptr;
            }

            memset(vm, 0, sizeof(AnmVm));
            vm->m_spriteNumber = -1; // Inlined constructor
            AnmVm::initialize(vm);

            This->m_nextFastVmIndex = This->getNextFastVmIndex(This->m_nextFastVmIndex);

            return vm;
        }
    }

    AnmVm* vm = &This->m_fastVms[index];
    This->m_fastVmsIsAlive[index] = 1;
    This->m_nextFastVmIndex = This->getNextFastVmIndex(This->m_nextFastVmIndex);
    return vm;
}

void AnmManager::makeVmWithAnmLoaded(AnmLoaded* anmLoaded, int scriptNumber, int anmVmLayer, AnmId* anmId)
{
    AnmManager* This = g_anmManager;
    g_supervisor.enterCriticalSection(9);
    AnmVm* vm = allocateVm();
    vm->m_flagsLow |= 0x40000000;
    vm->m_layer = anmVmLayer;
    vm->loadAnmScript(vm, anmLoaded, scriptNumber);
    putInVmList(vm, anmId);
    g_supervisor.leaveCriticalSection(9);
}

// 0x445320
void AnmManager::releaseTextures()
{
    AnmManager* This = g_anmManager;
    for (int i = 0; i < NUM_ANM_LOADEDS; ++i)
    {
        AnmLoaded* anmLoaded = This->m_loadedAnms[i];
        if (anmLoaded == nullptr)
            continue;

        AnmLoadedD3D* anmLoadedD3Ds = anmLoaded->m_anmLoadedD3D;
        int m_numAnmLoadedD3Ds = anmLoaded->m_numAnmLoadedD3Ds;
        for (int j = 0; j < m_numAnmLoadedD3Ds; ++j)
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

void AnmManager::flushSprites(AnmManager* This)
{
    if (This->m_anmVertexBuffers.leftoverSpriteCount == 0)
        return;

    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
    g_supervisor.d3dDevice->SetFVF(0x144);

    g_supervisor.d3dDevice->DrawPrimitiveUP(
        D3DPT_TRIANGLELIST,
        This->m_anmVertexBuffers.leftoverSpriteCount * 2,
        This->m_anmVertexBuffers.spriteRenderCursor,
        sizeof(RenderVertex144)
    );

    ++This->m_refreshCounter;
    This->m_anmVertexBuffers.spriteRenderCursor = This->m_anmVertexBuffers.spriteWriteCursor;
    This->m_anmVertexBuffers.leftoverSpriteCount = 0;
}

// 0x454ec0
void AnmManager::blitTextureToSurface(AnmManager* This, BlitParams* blitParams)
{
    if (!This->m_loadedAnms[blitParams->anmLoadedIndex]->m_anmLoadedD3D[blitParams->anmLoadedD3dIndex].m_texture)
        return;

    flushSprites(This);

    IDirect3DSurface9* backBuffer = nullptr;
    HRESULT hr = g_supervisor.d3dDevice->GetBackBuffer(
        0,
        0,
        D3DBACKBUFFER_TYPE_MONO,
        &backBuffer
    );
    if (FAILED(hr))
        return;

    AnmLoadedD3D* anmLoadedD3d = This->m_loadedAnms[blitParams->anmLoadedIndex]->m_anmLoadedD3D;
    IDirect3DTexture9* texture = anmLoadedD3d[blitParams->anmLoadedD3dIndex].m_texture;

    IDirect3DSurface9* textureSurface = nullptr;
    hr = texture->GetSurfaceLevel(0, &textureSurface);
    if (FAILED(hr))
    {
        backBuffer->Release();
        return;
    }

    RECT srcRect{};
    srcRect.left = blitParams->srcRect.left;
    srcRect.top = blitParams->srcRect.top;
    srcRect.right = blitParams->srcRect.left + blitParams->srcRect.right;
    srcRect.bottom = blitParams->srcRect.top + blitParams->srcRect.bottom;

    RECT dstRect{};
    dstRect.left = blitParams->destRect.left;
    dstRect.top = blitParams->destRect.top;
    dstRect.right = blitParams->destRect.left + blitParams->destRect.right;
    dstRect.bottom = blitParams->destRect.top + blitParams->destRect.bottom;

    hr = D3DXLoadSurfaceFromSurface(
        textureSurface,
        nullptr,
        &dstRect,
        backBuffer,
        nullptr,
        &srcRect,
        2,
        0
    );

    if (SUCCEEDED(hr))
        texture->AddDirtyRect(nullptr);

    textureSurface->Release();
    backBuffer->Release();
}

// 0x4453b0
void AnmManager::createD3DTextures(AnmManager* This)
{
    for (int i = 0; i < 32; ++i)
    {
        AnmLoaded* loadedAnm = This->m_loadedAnms[i];
        if (!loadedAnm)
            continue;

        if (loadedAnm->m_numAnmLoadedD3Ds <= 0)
            continue;

        for (int j = 0; j < loadedAnm->m_numAnmLoadedD3Ds; ++j)
        {
            AnmLoadedD3D* anmLoadedD3D = &loadedAnm->m_anmLoadedD3D[j];
            if (anmLoadedD3D->m_flags & 1)
            {
                anmLoadedD3D->m_flags |= 1;
                g_supervisor.d3dDevice->CreateTexture(
                    g_supervisor.m_d3dPresetParameters.BackBufferWidth,
                    g_supervisor.m_d3dPresetParameters.BackBufferHeight,
                    1,
                    1,
                    g_supervisor.m_d3dPresetParameters.BackBufferFormat,
                    D3DPOOL_DEFAULT,
                    &anmLoadedD3D->m_texture,
                    NULL
                );

                anmLoadedD3D->m_bytesPerPixel = (g_supervisor.m_d3dPresetParameters.BackBufferFormat == D3DFMT_X8R8G8B8) * 2 + 2;
            }
        }
    }
}

// 0x450e20
int AnmManager::updateWorldMatrixAndProjectQuadCorners(AnmManager* This, AnmVm* vm)
{
    uint32_t flags;
    D3DXMATRIX* baseSpriteScaleMatrix;
    D3DXMATRIX* localTransformMatrix;
    D3DXMATRIX* lpMatrixCopy;
    Float3 vecBottomLeft;
    Float3 vecBottomRight;
    Float3 vecTopLeft;
    Float3 vecTopRight;
    D3DXMATRIX worldMatrix;
    D3DXMATRIX rotationMatrix;
    float rotX;
    float rotY;
    float rotZ;
    float entityPosX;
    float posX;
    float scaleX;

    flags = vm->m_flagsLow;
    if (((flags & 0x4000) == 0) && ((flags & 0xc) != 0))
    {
        scaleX = vm->m_scale.x;
        localTransformMatrix = &vm->m_localTransformMatrix; // Local transform (scale + rotation)
        baseSpriteScaleMatrix = &vm->m_baseScaleMatrix;     // Base scale from unit quad to sprite size
        lpMatrixCopy = localTransformMatrix;

        // Copy baseSpriteScaleMatrix to localTransformMatrix
        memcpy(localTransformMatrix, baseSpriteScaleMatrix, sizeof(D3DXMATRIX));

        // Apply VM scale to localTransformMatrix
        localTransformMatrix->m[0][0] = scaleX * localTransformMatrix->m[0][0];
        localTransformMatrix->m[1][1] = vm->m_scale.y * localTransformMatrix->m[1][1];

        // Clear rotation-applied flag
        vm->m_flagsLow = flags & 0xfffffff7;

        // Apply rotations if non-zero
        rotX = vm->m_rotation.x;
        if (rotX != 0.0)
        {
            D3DXMatrixRotationX(&rotationMatrix, vm->m_rotation.x);
            D3DXMatrixMultiply(&vm->m_localTransformMatrix, &vm->m_localTransformMatrix, &rotationMatrix);
        }

        rotY = vm->m_rotation.y;
        if (rotY != 0.0)
        {
            D3DXMatrixRotationY(&rotationMatrix, vm->m_rotation.y);
            D3DXMatrixMultiply(&vm->m_localTransformMatrix, &vm->m_localTransformMatrix, &rotationMatrix);
        }

        rotZ = vm->m_rotation.z;
        if (rotZ != 0.0)
        {
            D3DXMatrixRotationZ(&rotationMatrix, vm->m_rotation.z);
            D3DXMatrixMultiply(&vm->m_localTransformMatrix, &vm->m_localTransformMatrix, &rotationMatrix);
        }

        // Clear scale-applied flag
        vm->m_flagsLow &= 0xfffffffb;
    }

    // Build world matrix: local transform + translations
    entityPosX = vm->m_entityPos.x;
    posX = vm->m_pos.x;

    memcpy(&worldMatrix, &vm->m_localTransformMatrix, sizeof(D3DXMATRIX));

    worldMatrix.m[3][0] = entityPosX + posX + vm->m_offsetPos.x + worldMatrix.m[3][0];
    worldMatrix.m[3][1] = vm->m_entityPos.y + vm->m_pos.y + vm->m_offsetPos.y + worldMatrix.m[3][1];
    worldMatrix.m[3][2] = vm->m_entityPos.z + vm->m_pos.z + vm->m_offsetPos.z;

    // Determine quad corners based on horizontal alignment flags (bits 18-19)
    flags = vm->m_flagsLow >> 0x12 & 3;
    if (flags == 0) // Centered
    {
        vecBottomLeft.x = -128.0;
        vecTopLeft.x = -128.0;
        vecBottomRight.x = 128.0;
        vecTopRight.x = 128.0;
    }
    else if (flags == 1) // Left-aligned
    {
        vecBottomLeft.x = 0.0;
        vecTopLeft.x = 0.0;
        vecBottomRight.x = 256.0;
        vecTopRight.x = 256.0;
    }
    else if (flags == 2) // Right-aligned
    {
        vecBottomLeft.x = -256.0;
        vecTopLeft.x = -256.0;
        vecBottomRight.x = 0.0;
        vecTopRight.x = 0.0;
    }

    // Determine quad corners based on vertical alignment flags (bits 20-21)
    flags = vm->m_flagsLow >> 0x14 & 3;
    if (flags == 0) // Centered
    {
        vecBottomRight.y = -128.0;
        vecBottomLeft.y = -128.0;
        vecTopRight.y = 128.0;
        vecTopLeft.y = 128.0;
    }
    else if (flags == 1) // Top-aligned
    {
        vecBottomRight.y = 0.0;
        vecBottomLeft.y = 0.0;
        vecTopRight.y = 256.0;
        vecTopLeft.y = 256.0;
    }
    else if (flags == 2) // Bottom-aligned
    {
        vecBottomRight.y = -256.0;
        vecBottomLeft.y = -256.0;
        vecTopRight.y = 0.0;
        vecTopLeft.y = 0.0;
    }

    // Set Z to 0 for all corners (2D quad)
    vecBottomRight.z = 0.0;
    vecTopLeft.z = 0.0;
    vecBottomLeft.z = 0.0;
    vecTopRight.z = 0.0;

    // Project quad corners to screen space
    Float3 bottomLeftOut, bottomRightOut, topLeftOut, topRightOut;
    D3DXVec3Project(
        &bottomLeftOut,
        &vecBottomLeft,
        &g_supervisor.currentCam->viewport,
        &g_supervisor.currentCam->projectionMatrix,
        &g_supervisor.currentCam->viewMatrix,
        &worldMatrix
    );

    D3DXVec3Project(
        &bottomRightOut,
        &vecBottomRight,
        &g_supervisor.currentCam->viewport,
        &g_supervisor.currentCam->projectionMatrix,
        &g_supervisor.currentCam->viewMatrix,
        &worldMatrix
    );

    D3DXVec3Project(
        &topLeftOut,
        &vecTopLeft,
        &g_supervisor.currentCam->viewport,
        &g_supervisor.currentCam->projectionMatrix,
        &g_supervisor.currentCam->viewMatrix,
        &worldMatrix
    );

    D3DXVec3Project(
        &topRightOut,
        &vecTopRight,
        &g_supervisor.currentCam->viewport,
        &g_supervisor.currentCam->projectionMatrix,
        &g_supervisor.currentCam->viewMatrix,
        &worldMatrix
    );

    g_renderQuad144[0].pos.x = bottomLeftOut.x;
    g_renderQuad144[0].pos.y = bottomLeftOut.y;
    g_renderQuad144[0].pos.z = bottomLeftOut.z;
    g_renderQuad144[1].pos.x = bottomRightOut.x;
    g_renderQuad144[1].pos.y = bottomRightOut.y;
    g_renderQuad144[1].pos.z = bottomRightOut.z;
    g_renderQuad144[2].pos.x = topRightOut.x;
    g_renderQuad144[2].pos.y = topRightOut.y;
    g_renderQuad144[2].pos.z = topRightOut.z;
    g_renderQuad144[3].pos.x = topLeftOut.x;
    g_renderQuad144[3].pos.y = topLeftOut.y;
    g_renderQuad144[3].pos.z = topLeftOut.z;

    memcpy(&This->m_currentWorldMatrix, &worldMatrix, sizeof(D3DXMATRIX));
    return 0;
}

void AnmManager::setupRenderStateForVm(AnmManager* This, AnmVm* vm)
{
    uint8_t mode = (vm->m_flagsLow >> 4) & 0x7;
    if (This->m_renderStateMode != mode)
    {
        flushSprites(This);
        This->m_renderStateMode = mode;
        D3DBLEND srcBlend = D3DBLEND_ONE;
        D3DBLEND destBlend = D3DBLEND_ONE;

        switch (mode)
        {
        case 0:  // Normal alpha blending
            srcBlend = D3DBLEND_SRCALPHA;
            destBlend = D3DBLEND_INVSRCALPHA;
            break;
        case 1:  // Additive blending
            srcBlend = D3DBLEND_SRCALPHA;
            destBlend = D3DBLEND_ONE;
            break;
        case 2:  // Subtract blending
            srcBlend = D3DBLEND_ZERO;
            destBlend = D3DBLEND_INVSRCCOLOR;
            break;
        case 3:  // Replace
            srcBlend = D3DBLEND_ONE;
            destBlend = D3DBLEND_ZERO;
            break;
        case 4:
            srcBlend = D3DBLEND_INVDESTCOLOR;
            destBlend = D3DBLEND_INVSRCALPHA;
            break;
        case 5:
            srcBlend = D3DBLEND_DESTCOLOR;
            destBlend = D3DBLEND_ZERO;
            break;
        case 6:
            srcBlend = D3DBLEND_INVSRCCOLOR;
            destBlend = D3DBLEND_INVSRCALPHA;
            break;
        default:
            return;
        }
        g_supervisor.d3dDevice->SetRenderState(D3DRS_SRCBLEND, srcBlend);
        g_supervisor.d3dDevice->SetRenderState(D3DRS_DESTBLEND, destBlend);
    }
    bool usefilter = (vm->m_flagsLow & 0x80000000) != 0;
    if (This->m_usePointFilter != usefilter)
    {
        flushSprites(This);
        This->m_usePointFilter = usefilter;
        D3DTEXTUREFILTERTYPE filterType = This->m_usePointFilter ? D3DTEXF_POINT : D3DTEXF_LINEAR;
        g_supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, filterType);
        g_supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, filterType);
    }
    This->m_someCounter++;
}

void AnmManager::writeSprite(AnmManager* This, RenderVertex144* srcVertices)
{
    RenderVertex144* dest = This->m_anmVertexBuffers.spriteWriteCursor;

    // Quad (0,1,2,3) -> Triangle List (0,1,2) + (1,3,2)
    // The specific order depends on the cull mode (CW vs CCW).
    // Dest[0] = Src[0]
    // Dest[1] = Src[1]
    // Dest[2] = Src[2]
    // Dest[3] = Src[1]
    // Dest[4] = Src[3]
    // Dest[5] = Src[2]

    // Tri 1: TL, TR, BL
    dest[0] = srcVertices[0];
    dest[1] = srcVertices[1];
    dest[2] = srcVertices[2];

    // Tri 2: TR, BR, BL
    dest[3] = srcVertices[1];
    dest[4] = srcVertices[3];
    dest[5] = srcVertices[2];

    // Advance cursor by 6 vertices
    This->m_anmVertexBuffers.spriteWriteCursor += 6;
    This->m_anmVertexBuffers.leftoverSpriteCount++;
}

void AnmManager::drawVmSprite2D(AnmManager* This, uint32_t layer, AnmVm* anmVm)
{
    // Apply global offset
    for (int i = 0; i < 4; i++)
    {
        g_renderQuad144[i].pos.x += This->m_globalRenderQuadOffsetX;
        g_renderQuad144[i].pos.y += This->m_globalRenderQuadOffsetY;
    }

    // Pixel Alignment (Round to nearest pixel - 0.5 for texel center mapping in DX9)
    if (layer & 1)
    {
        g_renderQuad144[0].pos.x = roundf(g_renderQuad144[0].pos.x) - 0.5f;
        g_renderQuad144[1].pos.x = roundf(g_renderQuad144[1].pos.x) - 0.5f;
        g_renderQuad144[0].pos.y = roundf(g_renderQuad144[0].pos.y) - 0.5f;
        g_renderQuad144[2].pos.y = roundf(g_renderQuad144[2].pos.y) - 0.5f;

        // Propagate aligned positions to form the rect
        g_renderQuad144[1].pos.y = g_renderQuad144[0].pos.y;
        g_renderQuad144[2].pos.x = g_renderQuad144[0].pos.x;
        g_renderQuad144[3].pos.x = g_renderQuad144[1].pos.x;
        g_renderQuad144[3].pos.y = g_renderQuad144[2].pos.y;
    }

    // Apply UV Scrolling
    for (int i = 0; i < 4; i++)
    {
        g_renderQuad144[i].uv.x = anmVm->m_spriteUvQuad[i].x + anmVm->m_uvScrollPos.x;
        g_renderQuad144[i].uv.y = anmVm->m_spriteUvQuad[i].y + anmVm->m_uvScrollPos.y;
    }

    // Bounding Box Calculation
    float maxX = g_renderQuad144[1].pos.x;
    if (g_renderQuad144[0].pos.x > maxX) maxX = g_renderQuad144[0].pos.x;
    if (g_renderQuad144[2].pos.x > maxX) maxX = g_renderQuad144[2].pos.x;
    if (g_renderQuad144[3].pos.x > maxX) maxX = g_renderQuad144[3].pos.x;

    float minX = g_renderQuad144[1].pos.x;
    if (g_renderQuad144[0].pos.x < minX) minX = g_renderQuad144[0].pos.x;
    if (g_renderQuad144[2].pos.x < minX) minX = g_renderQuad144[2].pos.x;
    if (g_renderQuad144[3].pos.x < minX) minX = g_renderQuad144[3].pos.x;

    float maxY = g_renderQuad144[1].pos.y;
    if (g_renderQuad144[0].pos.y > maxY) maxY = g_renderQuad144[0].pos.y;
    if (g_renderQuad144[2].pos.y > maxY) maxY = g_renderQuad144[2].pos.y;
    if (g_renderQuad144[3].pos.y > maxY) maxY = g_renderQuad144[3].pos.y;

    float minY = g_renderQuad144[1].pos.y;
    if (g_renderQuad144[0].pos.y < minY) minY = g_renderQuad144[0].pos.y;
    if (g_renderQuad144[2].pos.y < minY) minY = g_renderQuad144[2].pos.y;
    if (g_renderQuad144[3].pos.y < minY) minY = g_renderQuad144[3].pos.y;

    // Viewport Culling
    D3DVIEWPORT9* vp = &g_supervisor.currentCam->viewport;
    float vpX = static_cast<float>(vp->X);
    float vpY = static_cast<float>(vp->Y);
    float vpW = static_cast<float>(vp->Width);
    float vpH = static_cast<float>(vp->Height);

    if (vpX <= maxX && (vpX + vpW) >= minX && vpY <= maxY && (vpY + vpH) >= minY)
    {
        AnmLoadedD3D* tex = anmVm->m_sprite->anmLoadedD3D;
        if (This->m_anmLoadedD3D != tex)
        {
            This->m_anmLoadedD3D = tex;
            flushSprites(This);
            g_supervisor.d3dDevice->SetTexture(0, This->m_anmLoadedD3D->m_texture);
        }

        if (This->m_haveFlushedSprites != 1)
        {
            flushSprites(This);
            This->m_haveFlushedSprites = 1;
        }

        D3DCOLOR c0 = g_renderQuad144[0].diffuse;
        D3DCOLOR c1 = g_renderQuad144[1].diffuse;
        D3DCOLOR c2 = g_renderQuad144[2].diffuse;
        D3DCOLOR c3 = g_renderQuad144[3].diffuse;

        if ((layer & 2) == 0)
        {
            if ((anmVm->m_flagsLow & 0x8000) == 0)
                c0 = anmVm->m_color0;
            else
                c0 = anmVm->m_color1;

            c1 = c0; c2 = c0; c3 = c0;

            if (This->m_rebuildColorFlag)
            {
                // Extract ARGB from c0
                uint32_t a = (c0 >> 24) & 0xFF;
                uint32_t r = (c0 >> 16) & 0xFF;
                uint32_t g = (c0 >> 8) & 0xFF;
                uint32_t b = c0 & 0xFF;

                // Modulate
                r = modulateColorComponent(r, This->m_scaleR);
                g = modulateColorComponent(g, This->m_scaleG);
                b = modulateColorComponent(b, This->m_scaleB);
                a = modulateColorComponent(a, This->m_scaleA);

                c0 = (a << 24) | (r << 16) | (g << 8) | b;
                c1 = c0; c2 = c0; c3 = c0;
            }
        }

        g_renderQuad144[0].diffuse = c0;
        g_renderQuad144[1].diffuse = c1;
        g_renderQuad144[2].diffuse = c2;
        g_renderQuad144[3].diffuse = c3;

        setupRenderStateForVm(This, anmVm);
        writeSprite(This, g_renderQuad144);
    }
}

// 0x44f490
uint32_t AnmManager::modulateColorComponent(uint16_t base, uint16_t factor)
{
    uint32_t i = (base & 0xff) * (factor & 0xff) >> 7;
    if (0xff < i)
        i = 0xff;
    return i;
}

int AnmManager::drawVmWithFog(AnmManager* This, AnmVm* vm)
{
    puts("Drawing vm with fog\n");
    if (vm->projectQuadCornersThroughCameraViewport(vm) != 0)
        return -1;

    float fogStart = g_supervisor.currentCam->fogStart;
    float fogEnd = g_supervisor.currentCam->fogEnd;
    float cullDistance = g_supervisor.cam0.fogStart;

    D3DCOLOR color = (vm->m_flagsLow & 0x8000) ? vm->m_color1 : vm->m_color0;

    float x = vm->m_pos.x + vm->m_entityPos.x + vm->m_offsetPos.x - g_supervisor.currentCam->offset.x;
    float y = vm->m_pos.y + vm->m_entityPos.y + vm->m_offsetPos.y - g_supervisor.currentCam->offset.y;
    float z = vm->m_pos.z + vm->m_entityPos.z + vm->m_offsetPos.z - g_supervisor.currentCam->offset.z;

    float dist = sqrtf(x * x + y * y + z * z);

    if (This->m_rebuildColorFlag != 0)
    {
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        uint8_t a = (color >> 24) & 0xFF;

        r = scaleChannel(This->m_scaleR, r);
        g = scaleChannel(This->m_scaleG, g);
        b = scaleChannel(This->m_scaleB, b);
        a = scaleChannel(This->m_scaleA, a);

        color = (a << 24) | (r << 16) | (g << 8) | b;
    }

    if (dist <= cullDistance)
        g_renderQuad144[0].diffuse = color;
    else
    {
        float denominator = fogStart - fogEnd;
        float factor = (cullDistance - dist) / denominator;

        if (factor >= 1.0f)
            return -1;

        int fogR = static_cast<int>(g_supervisor.cam0.fogR);
        int fogG = static_cast<int>(g_supervisor.cam0.fogG);
        int fogB = static_cast<int>(g_supervisor.cam0.fogB);

        uint8_t a = (color >> 24) & 0xFF;
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;


        int subR = static_cast<int>(static_cast<float>(r - fogR) * factor);
        int subG = static_cast<int>(static_cast<float>(g - fogG) * factor);
        int subB = static_cast<int>(static_cast<float>(b - fogB) * factor);

        r -= subR;
        g -= subG;
        b -= subB;

        a = static_cast<uint8_t>(static_cast<float>(a) * (1.0f - factor));
        color = (a << 24) | (r << 16) | (g << 8) | b;
        g_renderQuad144[0].diffuse = color;
    }

    g_renderQuad144[1].diffuse = g_renderQuad144[0].diffuse;
    g_renderQuad144[2].diffuse = g_renderQuad144[0].diffuse;
    g_renderQuad144[3].diffuse = g_renderQuad144[0].diffuse;

    drawVmSprite2D(This, 2, vm);
    return 0;
}

void AnmManager::applyRenderStateForVm(AnmManager* This, AnmVm* vm)
{
    IDirect3DDevice9* device = g_supervisor.d3dDevice;

    uint8_t blendMode = (vm->m_flagsLow >> 4) & 0x7;
    if (blendMode != This->m_renderStateMode)
    {
        flushSprites(This);
        This->m_renderStateMode = blendMode;

        D3DBLEND srcBlend, destBlend;
        switch (blendMode)
        {
        case 0:
            srcBlend = D3DBLEND_SRCALPHA;
            destBlend = D3DBLEND_INVSRCALPHA;
            break;
        case 1:
            srcBlend = D3DBLEND_SRCALPHA;
            destBlend = D3DBLEND_ONE;
            break;
        case 2:
            srcBlend = D3DBLEND_ZERO;
            destBlend = D3DBLEND_INVSRCCOLOR;
            break;
        case 3:
            srcBlend = D3DBLEND_ONE;
            destBlend = D3DBLEND_ZERO;
            break;
        case 4:
            srcBlend = D3DBLEND_INVDESTCOLOR;
            destBlend = D3DBLEND_INVSRCALPHA;
            break;
        case 5:
            srcBlend = D3DBLEND_DESTCOLOR;
            destBlend = D3DBLEND_ZERO;
            break;
        case 6:
            srcBlend = D3DBLEND_INVSRCCOLOR;
            destBlend = D3DBLEND_INVSRCALPHA;
            break;
        default:
            return;
        }
        device->SetRenderState(D3DRS_SRCBLEND, srcBlend);
        device->SetRenderState(D3DRS_DESTBLEND, destBlend);
    }

    D3DCOLOR color = (vm->m_flagsLow & 0x8000) ? vm->m_color1 : vm->m_color0;

    if (This->m_rebuildColorFlag)
    {
        uint32_t r = ((color >> 16) & 0xFF) * This->m_scaleR >> 7;
        r = (r > 255) ? 255 : r;

        uint32_t g = ((color >> 8) & 0xFF) * This->m_scaleG >> 7;
        g = (g > 255) ? 255 : g;

        uint32_t b = (color & 0xff) * This->m_scaleB >> 7;
        b = (b > 255) ? 255 : b;

        uint32_t a = (color >> 24) * This->m_scaleA >> 7;
        a = (a > 255) ? 255 : a;

        color = (a << 24) | (r << 16) | (g << 8) | b;
    }

    if (color != This->m_color)
    {
        flushSprites(This);
        This->m_color = color;
        device->SetRenderState(D3DRS_TEXTUREFACTOR, color);
    }

    bool usePointFilterNew = (vm->m_flagsLow & 0x80000000) != 0;
    if (usePointFilterNew != This->m_usePointFilter)
    {
        flushSprites(This);
        This->m_usePointFilter = usePointFilterNew;

        DWORD filter = usePointFilterNew ? D3DTEXF_POINT : D3DTEXF_LINEAR;
        device->SetSamplerState(0, D3DSAMP_MAGFILTER, filter);
        device->SetSamplerState(0, D3DSAMP_MINFILTER, filter);
    }
    This->m_someCounter++;
}

int AnmManager::drawVmTriangleStrip(AnmManager* This, AnmVm* vm, SpecialRenderData* specialRenderData, uint32_t vertexCount)
{
    uint8_t alpha = (vm->m_color0 >> 24) & 0xff;
    if ((vm->m_flagsLow & 0x3) != 0x3 || alpha == 0)
        return -1;

    if (This->m_anmVertexBuffers.leftoverSpriteCount != 0)
        flushSprites(This);

    AnmLoadedD3D* tex = vm->m_sprite->anmLoadedD3D;
    if (This->m_anmLoadedD3D != tex)
    {
        This->m_anmLoadedD3D = tex;
        g_supervisor.d3dDevice->SetTexture(0, tex->m_texture);
    }

    if (This->m_haveFlushedSprites != 3)
    {
        g_supervisor.d3dDevice->SetFVF(0x144);
        This->m_haveFlushedSprites = 3;
    }
    setupRenderStateForVm(This, vm);

    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
    g_supervisor.d3dDevice->DrawPrimitiveUP(
        D3DPT_TRIANGLESTRIP,
        vertexCount - 2,
        specialRenderData->vertices,
        sizeof(RenderVertex144)
    );
    return 0;
}

// 0x451e10
int AnmManager::drawVmTriangleFan(AnmManager* This, AnmVm* vm, SpecialRenderData* specialRenderData, uint32_t vertexCount)
{
    if (This->m_anmVertexBuffers.leftoverSpriteCount != 0)
        flushSprites(This);

    if (This->m_haveFlushedSprites != 3)
    {
        g_supervisor.d3dDevice->SetFVF(0x144);
        This->m_haveFlushedSprites = 3;
    }
    setupRenderStateForVm(This, vm);

    AnmLoadedD3D* tex = vm->m_sprite->anmLoadedD3D;
    if (This->m_anmLoadedD3D != tex)
    {
        This->m_anmLoadedD3D = tex;
        g_supervisor.d3dDevice->SetTexture(0, tex->m_texture);
    }

    flushSprites(This);
    g_supervisor.d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, 0);
    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
    g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
    g_supervisor.d3dDevice->DrawPrimitiveUP(
        D3DPT_TRIANGLEFAN,
        vertexCount - 2,
        specialRenderData->vertices,
        sizeof(RenderVertex144)
    );
    return 0;
}

// 0x4561e0
AnmVm* AnmManager::getVmById(AnmManager* This, int anmId)
{
    if (anmId == 0)
        return nullptr;

    AnmVmListNode* primaryVmList = This->m_primaryGlobalHead;
    while (primaryVmList)
    {
        if (primaryVmList->entry->m_id.id == anmId)
            return primaryVmList->entry;
        primaryVmList = primaryVmList->next;
    }

    AnmVmListNode* secondaryVmList = This->m_secondaryGlobalHead;
    while (secondaryVmList)
    {
        if (secondaryVmList->entry->m_id.id == anmId)
            return secondaryVmList->entry;
        secondaryVmList = secondaryVmList->next;
    }
    return nullptr;
}

void AnmManager::loadIntoAnmVm(AnmVm* vm, AnmLoaded* anmLoaded, int scriptNumber)
{
    AnmRawInstruction* instr;
    uint32_t isInitialized;

    if (anmLoaded->m_spriteData[scriptNumber] != 0 && anmLoaded->m_anmsLoading == 0)
    {
        vm->initialize(vm);
        vm->m_scriptNumber = static_cast<uint16_t>(scriptNumber);
        vm->m_flagsLow &= 0xfffff9ff;
        vm->m_anmFileIndex = static_cast<uint16_t>(anmLoaded->m_anmSlotIndex);
        vm->m_anmLoaded = anmLoaded;

        instr = (AnmRawInstruction*)anmLoaded->m_spriteData[scriptNumber];
        vm->m_beginningOfScript = instr;
        vm->m_currentInstruction = instr;
        vm->m_timeInScript.set(&vm->m_timeInScript, 0);
        vm->m_flagsLow &= 0xfffffffe;
        vm->run(vm);
        ++g_anmManager->m_allocatedVmCountMaybe;
        return;
    }
    memset(vm, 0, 0x434);
}

void AnmManager::putInVmList(AnmVm* vm, AnmId* anmId)
{
    AnmManager* This = g_anmManager;
    AnmVmListNode* curVm = &vm->m_globalListNode;
    curVm->entry = vm;
    vm->m_globalListNode.next = nullptr;
    vm->m_globalListNode.prev = nullptr;
    if (This->m_primaryGlobalHead == nullptr)
        This->m_primaryGlobalHead = curVm;

    else
    {
        AnmVmListNode* primaryVms = This->m_primaryGlobalTail;
        if (primaryVms->next != nullptr)
        {
            (vm->m_globalListNode).next = primaryVms->next;
            primaryVms->next->prev = curVm;
        }
        primaryVms->next = curVm;
        vm->m_globalListNode.prev = primaryVms;
    }
    This->m_primaryGlobalTail = curVm;

    // Strange but correct game logic
    ++This->m_id;
    if (This->m_id == 0)
        ++This->m_id;

    vm->m_id.id = This->m_id;
    anmId->id = This->m_id;
}

// 0x455b10
void AnmManager::spawnVmAtPosition(AnmLoaded* anmLoaded,AnmId* anmId, int scriptNumber, int layer, Float3* spawnLocation)
{
    AnmVm* vm;
    g_supervisor.enterCriticalSection(9);
    vm = allocateVm();
    vm->m_flagsLow |= 0x40000000;
    vm->m_layer = layer;
    vm->m_entityPos.x = spawnLocation->x + 32.0 + 192.0;
    vm->m_entityPos.y = spawnLocation->y + 16.0;
    vm->m_entityPos.z = spawnLocation->z;

    AnmVm::loadIntoAnmVm(vm,anmLoaded,scriptNumber);
    putInVmList(vm, anmId);
    g_supervisor.leaveCriticalSection(9);
}

void AnmManager::loadAnmScriptAndAddToList(AnmLoaded* anmLoaded, uint32_t scriptNumber, int layer, AnmId* outAnmId)
{
    g_supervisor.enterCriticalSection(9);
    AnmVm* vm = allocateVm();
    vm->m_flagsLow |= 0x40000000;
    vm->m_layer = layer;
    vm->loadAnmScript(vm, anmLoaded, scriptNumber);
    addVm(vm, outAnmId);
    g_supervisor.leaveCriticalSection(9);
}

// 0x4549e0
void AnmManager::releaseAnmLoaded(AnmManager* This, AnmLoaded* anmLoaded)
{
    if (!anmLoaded->m_header)
        return;
    This->markAnmLoadedAsReleasedInVmList(This, anmLoaded);

    if (anmLoaded->m_numAnmLoadedD3Ds > 0)
    {
        for (int i = 0; i < anmLoaded->m_numAnmLoadedD3Ds; ++i)
        {
            AnmLoadedD3D* entry = &anmLoaded->m_anmLoadedD3D[i];
            if (entry->m_texture)
            {
                entry->m_texture->Release();
                entry->m_texture = nullptr;
            }
            if (entry->m_srcData)
            {
                free(entry->m_srcData);
                entry->m_srcData = nullptr;
            }
        }
    }

    if (anmLoaded->m_anmLoadedD3D)
    {
        free(anmLoaded->m_anmLoadedD3D);
        anmLoaded->m_anmLoadedD3D = nullptr;
    }

    if (anmLoaded->m_keyframeData)
    {
        free(anmLoaded->m_keyframeData);
        anmLoaded->m_keyframeData = nullptr;
    }

    if (anmLoaded->m_spriteData)
    {
        free(anmLoaded->m_spriteData);
        anmLoaded->m_spriteData = nullptr;
    }

    if (anmLoaded->m_unknownHeapAllocated)
    {
        free(anmLoaded->m_unknownHeapAllocated);
        anmLoaded->m_unknownHeapAllocated = nullptr;
    }

    if (anmLoaded->m_header)
    {
        free(anmLoaded->m_header);
        anmLoaded->m_header = nullptr;
    }
}

void AnmManager::transformAndDraw(AnmManager* This, AnmVm* vm)
{
    // Pre-calculate world matrix and setup quad corners (likely in This->m_someBuffer or similar)
    updateWorldMatrixAndProjectQuadCorners(This, vm);

    float fogEnd = g_supervisor.currentCam->fogEnd;
    float fogStart = g_supervisor.currentCam->fogStart;
    float fogRange = fogEnd - fogStart;

    // Determine base color (Normal or Blend color)
    D3DCOLOR baseColor;
    if ((vm->m_flagsLow & 0x8000) == 0)
        baseColor = vm->m_color0;
    else
        baseColor = vm->m_color1;

    // Extract base color components
    int a = (baseColor >> 24) & 0xFF;
    int r = (baseColor >> 16) & 0xFF;
    int g = (baseColor >> 8) & 0xFF;
    int b = baseColor & 0xFF;

    Float3* srcPos = &This->m_squareVertices->position;

    for (int i = 0; i < 4; i++)
    {
        D3DXVECTOR4 pOut;

        // Transform vertex position by the current world matrix
        D3DXVec3Transform(&pOut, &srcPos[i], &This->m_currentWorldMatrix);

        // Calculate vector from Camera to Vertex
        float dy = pOut.x - g_supervisor.cam0.offset.x;
        float dz = pOut.y - g_supervisor.cam0.offset.y;
        float dx = pOut.z - g_supervisor.cam0.offset.z;
        float dist = sqrtf(dx * dx + dy * dy + dz * dz);

        if (dist <= g_supervisor.cam0.fogEnd)
            g_renderQuad144[i].diffuse = baseColor;
        else
        {
            // Calculate interpolation factor 'y' (0.0 to 1.0)
            float fogFactor = (g_supervisor.cam0.fogEnd - dist) / fogRange;

            // Clamp and apply fog
            if (fogFactor >= 1.0f)
                g_renderQuad144[i].diffuse = g_supervisor.currentCam->renderStateValue1;
            else
            {
                int deltaB = (int)roundf(((float)b - g_supervisor.cam0.fogB) * fogFactor);
                int finalB = b - deltaB;
                int deltaG = (int)roundf(((float)g - g_supervisor.cam0.fogG) * fogFactor);
                int finalG = g - deltaG;
                int deltaR = (int)roundf(((float)r - g_supervisor.cam0.fogR) * fogFactor);
                int finalR = r - deltaR;

                // Reassemble color
                g_renderQuad144[i].diffuse = D3DCOLOR_ARGB(a, finalR, finalG, finalB);
            }
        }
    }
    drawVmSprite2D(This, 2, vm);

    // Reset RHW to 1.0 for all vertices after drawing
    g_renderQuad144[3].rhw = 1.0f;
    g_renderQuad144[2].rhw = 1.0f;
    g_renderQuad144[1].rhw = 1.0f;
    g_renderQuad144[0].rhw = 1.0f;
}

int AnmManager::drawVmWithTextureTransform(AnmManager* This, AnmVm* vm)
{
    if ((vm->m_flagsLow & 1) && (vm->m_flagsLow & 2) && (vm->m_color0 >> 24) != 0)
    {
        if (This->m_anmVertexBuffers.leftoverSpriteCount != 0)
            flushSprites(This);

        uint32_t flags = vm->m_flagsLow;
        if (!(flags & 0x4000) && (flags & 0xC))
        {
            vm->m_localTransformMatrix = vm->m_baseScaleMatrix;
            vm->m_localTransformMatrix._11 *= vm->m_scale.x;
            vm->m_localTransformMatrix._22 *= vm->m_scale.y;
            vm->m_flagsLow &= ~0x8; // Clear scale dirty flag

            D3DXMATRIX tempMatrix;
            if (vm->m_rotation.x != 0.0f)
            {
                D3DXMatrixRotationX(&tempMatrix, vm->m_rotation.x);
                D3DXMatrixMultiply(&vm->m_localTransformMatrix, &vm->m_localTransformMatrix, &tempMatrix);
            }
            if (vm->m_rotation.y != 0.0f)
            {
                D3DXMatrixRotationY(&tempMatrix, vm->m_rotation.y);
                D3DXMatrixMultiply(&vm->m_localTransformMatrix, &vm->m_localTransformMatrix, &tempMatrix);
            }
            if (vm->m_rotation.z != 0.0f)
            {
                D3DXMatrixRotationZ(&tempMatrix, vm->m_rotation.z);
                D3DXMatrixMultiply(&vm->m_localTransformMatrix, &vm->m_localTransformMatrix, &tempMatrix);
            }
            vm->m_flagsLow &= ~0x4; // Clear rotation dirty flag
        }

        // Initialize World Matrix from Local Matrix
        D3DXMATRIX worldMatrix = vm->m_localTransformMatrix;

        int anchorX = (vm->m_flagsLow >> 18) & 3;
        float finalX = vm->m_entityPos.x + vm->m_pos.x + vm->m_offsetPos.x;

        if (anchorX == 1)
            finalX -= fabsf(vm->m_spriteSize.x * vm->m_scale.x * 0.5f);
        else if (anchorX == 2)
            finalX += fabsf(vm->m_spriteSize.x * vm->m_scale.x * 0.5f);

        worldMatrix._41 = finalX;

        int anchorY = (vm->m_flagsLow >> 20) & 3;
        float finalY = vm->m_entityPos.y + vm->m_pos.y + vm->m_offsetPos.y;

        if (anchorY == 1)
            finalY -= fabsf(vm->m_spriteSize.y * vm->m_scale.y * 0.5f);
        else if (anchorY == 2)
            finalY += fabsf(vm->m_spriteSize.y * vm->m_scale.y * 0.5f);

        worldMatrix._42 = finalY;
        worldMatrix._43 = vm->m_entityPos.z + vm->m_pos.z + vm->m_offsetPos.z;

        applyRenderStateForVm(This, vm);
        g_supervisor.d3dDevice->SetTransform(D3DTS_WORLD, &worldMatrix);

        // Texture Setup
        AnmLoadedD3D* anmLoadedD3D = vm->m_sprite->anmLoadedD3D;
        if (This->m_anmLoadedD3D != anmLoadedD3D)
        {
            This->m_anmLoadedD3D = anmLoadedD3D;
            g_supervisor.d3dDevice->SetTexture(0, anmLoadedD3D->m_texture);
        }

        // Texture Transform (Scroll/Matrix)
        float scrollX = vm->m_uvScrollPos.x;
        if (This->m_cachedSprite != vm->m_sprite || scrollX != 0.0f || vm->m_uvScrollPos.y != 0.0f)
        {
            This->m_cachedSprite = vm->m_sprite;

            // Copy base texture matrix
            D3DXMATRIX texMatrix = vm->m_textureMatrix;

            // Update translation components
            texMatrix._31 = vm->m_uvScrollPos.x + vm->m_spriteUvQuad[0].x;
            texMatrix._32 = vm->m_uvScrollPos.y + vm->m_spriteUvQuad[0].y;

            g_supervisor.d3dDevice->SetTransform(D3DTS_TEXTURE0, &texMatrix);
        }

        if (This->m_haveFlushedSprites != 2)
        {
            g_supervisor.d3dDevice->SetStreamSource(0, This->m_squareVertexBuffer, 0, 20);
            g_supervisor.d3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
            g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
            g_supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
            This->m_haveFlushedSprites = 2;
        }

        g_supervisor.d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
        return 0;
    }
    return -1;
}

void AnmManager::drawRect(AnmVm* vm, Rect3D* rect3d)
{
    switch (vm->m_flagsLow >> 0x16 & 0xf)
    {
    case 0:
        vm->writeSpriteCharactersWithoutRot(vm, &rect3d->bottomLeft, &rect3d->bottomRight, &rect3d->topRight, &rect3d->topLeft);
        break;
    case 1:
        vm->applyZRotationToQuadCorners(vm, &rect3d->bottomLeft, &rect3d->bottomRight, &rect3d->topRight, &rect3d->topLeft);
        break;
    case 2:
    case 3:
        vm->writeSpriteCharacters(vm, &rect3d->bottomLeft, &rect3d->bottomRight, &rect3d->topRight, &rect3d->topLeft);
        break;
    }
}

void AnmManager::setInterruptById(int vmId, uint16_t pendingInterruptFlag)
{
    AnmVm* vm = getVmById(g_anmManager, vmId);
    AnmVmListNode* vmList = vm->m_familyListNode.prev; // VERIFY
    vm->m_pendingInterrupt = pendingInterruptFlag;
    if (vm && vmList == nullptr) // ??
    {
        for (vmList = (vm->m_familyListNode).next; vmList != nullptr; vmList = vmList->next)
            vmList->entry->m_pendingInterrupt = pendingInterruptFlag;
    }
}


void AnmManager::setVmPosition(AnmVm* vm, Float3* position)
{
    vm->m_entityPos.x = position->x + 32.0 + 192.0;
    vm->m_entityPos.y = position->y + 16.0;
    vm->m_entityPos.z = position->z;

    AnmVmListNode* node = vm->m_familyListNode.prev;
    if (!node)
    {
        node = vm->m_familyListNode.next;
        while (node)
        {
            AnmVm* entry = node->entry;
            node = node->next;
            entry->m_entityPos.x = position->x + 32.0 + 192.0;
            entry->m_entityPos.y = position->y + 16.0;
            entry->m_entityPos.z = position->z;
        }
    }
}

void AnmManager::setVmPositionById(int anmId, Float3* position)
{
    AnmVm* vm = getVmById(g_anmManager, anmId);
    if (!vm)
        return;

    AnmVmListNode* node = vm->m_familyListNode.prev;
    if (!node)
    {
        node = vm->m_familyListNode.next;
        while (node)
        {
            AnmVm* entry = node->entry;
            node = node->next;
            entry->m_entityPos.x = position->x + 32.0 + 192.0;
            entry->m_entityPos.y = position->y + 16.0;
            entry->m_entityPos.z = position->z;
        }
    }
}

void AnmManager::drawVm(AnmManager* This, AnmVm* vm)
{
    uint8_t alpha = (vm->m_color0 >> 24) & 0xff;
    if ((vm->m_flagsLow & 0x3) != 0x3 || alpha == 0)
        return;
    uint32_t mode = vm->m_flagsLow >> 0x16 & 0xf;
    // if (mode == 4)
    //     printf("drawing vm with mode %d\n", mode);

    switch (mode)
    {
    case 0:
    {
        vm->writeSpriteCharactersWithoutRot(vm, &g_renderQuad144[0].pos, &g_renderQuad144[1].pos, &g_renderQuad144[2].pos, &g_renderQuad144[3].pos);
        drawVmSprite2D(This, 1, vm);
        break;
    }
    case 1:
    {
        if (vm->m_rotation.z == 0.0)
        {
            vm->writeSpriteCharactersWithoutRot(vm, &g_renderQuad144[0].pos, &g_renderQuad144[1].pos, &g_renderQuad144[2].pos, &g_renderQuad144[3].pos);
            drawVmSprite2D(This, 1, vm);
        }
        else
        {
            vm->applyZRotationToQuadCorners(vm, &g_renderQuad144[0].pos, &g_renderQuad144[1].pos, &g_renderQuad144[2].pos, &g_renderQuad144[3].pos);
            drawVmSprite2D(This, 0, vm);
        }
        break;
    }
    case 2:
    {
        vm->writeSpriteCharacters(vm, &g_renderQuad144[0].pos, &g_renderQuad144[1].pos, &g_renderQuad144[2].pos, &g_renderQuad144[3].pos);
        drawVmSprite2D(This, 0, vm);
        break;
    }
    case 3:
    {
        if (vm->m_rotation.z == 0.0)
        {
            vm->writeSpriteCharacters(vm, &g_renderQuad144[0].pos, &g_renderQuad144[1].pos, &g_renderQuad144[2].pos, &g_renderQuad144[3].pos);
            drawVmSprite2D(This, 0, vm);
        }
        else
        {
            vm->applyZRotationToQuadCorners(vm, &g_renderQuad144[0].pos, &g_renderQuad144[1].pos, &g_renderQuad144[2].pos, &g_renderQuad144[3].pos);
            drawVmSprite2D(This, 0, vm);
        }
        break;
    }
    case 4:
    {
        vm->projectQuadCornersThroughCameraViewport(vm);
        // drawVmSprite2D(This, 0, vm);
        break;
    }
    case 5:
    {
        updateWorldMatrixAndProjectQuadCorners(This, vm);
        drawVmSprite2D(This, 0, vm);
        g_renderQuad144[0].rhw = 1.0;
        g_renderQuad144[1].rhw = 1.0;
        g_renderQuad144[2].rhw = 1.0;
        g_renderQuad144[3].rhw = 1.0;
        break;
    }
    case 6:
        drawVmWithFog(This, vm);
        break;
    case 7:
        transformAndDraw(This, vm);
        break;
    case 8:
    {
#if 0
        //puts("mode8\n");
        using ltoFunc = Signature<int, AnmManager*, AnmVm*>;
        using ltoSig = Storage<
            EAX,         // Return      -> EAX (If void, use Void)
            Stack<0x4>,  // AnmManager* -> Stack[0x4]
            EBX          // AnmVm*      -> EBX
        >;
        static auto game_drawVmWithTextureTransform = createCustomCallingConvention<ltoSig, ltoFunc>(0x4513a0);
        game_drawVmWithTextureTransform(This, vm);
#else
        drawVmWithTextureTransform(This, vm);
#endif
        break;
    }
    case 9:
    case 0xc:
    case 0xd:
    {
        drawVmTriangleStrip(This, vm, vm->m_specialRenderData, vm->m_intVars[0] * 2);

        //using ltoFunc = Signature<int, AnmManager*, AnmVm*, void*, uint32_t>;
        //using ltoSig = Storage<
        //    Returns<RegCode::EAX>,
        //    ECX,
        //    EAX,
        //    Stack<0x4>,
        //    Stack<0x8>
        //>;
        //static auto game_drawVmTriangleStrip = createCustomCallingConvention<ltoSig, ltoFunc>(0x451a00);
        //game_drawVmTriangleStrip(This, vm, vm->m_specialRenderData, vm->m_intVars[0] * 2);

        break;
    }
    case 0xb:
        drawVmTriangleFan(This, vm, vm->m_specialRenderData, vm->m_intVars[0] * 2);
        break;
    default:
        break;
    }
}

// 0x445460
void AnmManager::blitTextures(AnmManager* This)
{
    for (int i = 0; i < 4; ++i)
    {
        BlitParams* blitParams = &This->m_blitParamsArray[i];
        if (blitParams->anmLoadedIndex >= 0)
        {
            This->blitTextureToSurface(This, blitParams);
            blitParams->anmLoadedIndex = -1;
        }
    }
}

ChainCallbackResult AnmManager::renderLayer(AnmManager* This, int layer)
{
    g_supervisor.enterCriticalSection(9);
    AnmVm* vm = &This->m_vmLayers[layer];
    while (vm->m_nextInLayerList)
    {
        if ((vm->m_flagsLow & 0x4000000) == 0)
        {
            if (vm->m_onDraw)
                vm->m_onDraw(vm);
            drawVm(This, vm);
        }
        vm = vm->m_nextInLayerList;
    }
    g_supervisor.leaveCriticalSection(9);
    return ChainCallbackResult::Continue;
}

ChainCallbackResult AnmManager::onTick1a(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    int vmLayer;
    AnmVm* vmLayers[33]{};
    AnmVmListNode* list;
    AnmVm* vm;

    g_supervisor.enterCriticalSection(9);

    vmLayer = 0;
    vm = This->m_vmLayers;
    do {
        vmLayers[vmLayer] = vm;
        vm->m_nextInLayerList = (AnmVm*)0x0;
        vmLayer = vmLayer + 1;
        vm = vm + 1;
    } while (vmLayer < 0x1d);

    list = This->m_primaryGlobalHead;
    while (list)
    {
        vm = list->entry;
        list = list->next;
        if ((vm->m_flagsLow & 0x4000000) == 0)
        {
            ChainCallbackResult result;
            if (vm->m_onTick)
                result = vm->m_onTick(vm);

            AnmVm::run(vm);

            if (vm->m_onTick && result == ChainCallbackResult::ContinueAndRemoveJob)
            {
                vmLayers[vm->m_layer]->m_nextInLayerList = vm;
                vmLayers[vm->m_layer] = vm;
                vm->m_nextInLayerList = nullptr;
            }
            else
                removeVm(This, vm);
        }
        else
            removeVm(This, vm);
        ++This->m_someTickCounter;
    }
    g_supervisor.leaveCriticalSection(9);
    return ChainCallbackResult::Continue;
}

ChainCallbackResult AnmManager::onTick08(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    void* onTickFunction;
    AnmVm* vmLayers;
    AnmVmListNode* list;
    AnmVm* vm;

    g_supervisor.enterCriticalSection(9);

    This->m_vmLayers[0x1d].m_nextInLayerList = nullptr;
    This->m_vmLayers[0x1e].m_nextInLayerList = nullptr;
    list = This->m_secondaryGlobalHead;
    This->m_someTickCounter = 0;
    while (list)
    {
        //vm = list->entry;
        //list = list->next;
        //if ((vm->m_flagsLow & 0x4000000) == 0)
        //{
        //    onTickFunction = vm->onTick;
        //    if ((code*)onTickFunction != (code*)0x0) {
        //        /* reusing variable */
        //        onTickFunction = (void*)(*(code*)onTickFunction)();
        //    }
        //    AnmVm::run(vm);
        //    if ((code*)onTickFunction == (code*)0x0)
        //    {
        //        if ((vm->m_layer != 0x1d) && (vm->m_layer != 0x1e)) {
        //            vm->m_layer = 0x1d;
        //        }
        //        /* ? */
        //        (&vmLayers)[vm->m_layer]->m_nextInLayerList = vm;
        //        (&vmLayers)[vm->m_layer] = vm;
        //        vm->m_nextInLayerList = nullptr;
        //    }
        //    else {
        //        removeVm(This, vm);
        //    }
        //}
        //else {
        //    removeVm(This, vm);
        //}
        //++This->m_someTickCounter;
    }
    g_supervisor.leaveCriticalSection(9);
    return ChainCallbackResult::Continue;
}

ChainCallbackResult AnmManager::on_draw_04_just_renders_layer_00(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 0);
}

ChainCallbackResult AnmManager::on_draw_06_just_renders_layer_01(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 1);
}

ChainCallbackResult AnmManager::on_draw_08_just_renders_layer_02(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 2);
}

ChainCallbackResult AnmManager::on_draw_0a_also_renders_layer_03(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    g_supervisor.currentCam = &g_supervisor.stageCam;
    g_supervisor.swapCameraTransformMatrices(&g_supervisor.stageCam);
    g_supervisor.d3dDevice->SetViewport(&g_supervisor.currentCam->viewport);
    g_supervisor.camIndex = 2;
    if (g_supervisor.d3dDisableFogFlag != 0)
    {
        flushSprites(g_anmManager);
        g_supervisor.d3dDisableFogFlag = 0;
        g_supervisor.d3dDevice->SetRenderState(D3DRS_FOGENABLE, 0);
    }
    return renderLayer(This, 3);
}

ChainCallbackResult AnmManager::on_draw_0c_just_renders_layer_04(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 4);
}

ChainCallbackResult AnmManager::on_draw_0f_just_renders_layer_05(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 5);
}

ChainCallbackResult AnmManager::on_draw_10_just_renders_layer_06(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 6);
}

ChainCallbackResult AnmManager::on_draw_11_just_renders_layer_07(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 7);
}

ChainCallbackResult AnmManager::on_draw_12_just_renders_layer_08(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 8);
}

ChainCallbackResult AnmManager::on_draw_13_just_renders_layer_09(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 9);
}

ChainCallbackResult AnmManager::on_draw_15_just_renders_layer_10(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 10);
}

ChainCallbackResult AnmManager::on_draw_17_just_renders_layer_11(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 11);
}

ChainCallbackResult AnmManager::on_draw_18_just_renders_layer_12(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 12);
}

ChainCallbackResult AnmManager::on_draw_1a_just_renders_layer_13(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 13);
}

ChainCallbackResult AnmManager::on_draw_1c_just_renders_layer_14(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 14);
}

ChainCallbackResult AnmManager::on_draw_20_just_renders_layer_15(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 15);
}

ChainCallbackResult AnmManager::on_draw_22_just_renders_layer_16(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 16);
}

ChainCallbackResult AnmManager::on_draw_24_just_renders_layer_17(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 17);
}

ChainCallbackResult AnmManager::on_draw_27_just_renders_layer_18(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 18);
}

ChainCallbackResult AnmManager::on_draw_28_also_renders_layer_19(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    g_supervisor.stageCam.m_globalRenderQuadOffsetX = 0.0;
    g_supervisor.stageCam.m_globalRenderQuadOffsetY = 0.0;
    This->m_globalRenderQuadOffsetX = 0.0;
    This->m_globalRenderQuadOffsetY = 0.0;
    return renderLayer(This, 19);
}

ChainCallbackResult AnmManager::on_draw_30_also_renders_layer_20(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    g_supervisor.currentCam = &g_supervisor.cam1;
    g_supervisor.swapCameraTransformMatrices(&g_supervisor.cam1);
    g_supervisor.d3dDevice->SetViewport(&g_supervisor.currentCam->viewport);
    g_supervisor.camIndex = 1;
    return renderLayer(This, 20);
}


ChainCallbackResult AnmManager::on_draw_31_just_renders_layer_21(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 21);
}

ChainCallbackResult AnmManager::on_draw_32_just_renders_layer_22(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 22);
}

ChainCallbackResult AnmManager::on_draw_3c_just_renders_layer_23(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 23);
}

ChainCallbackResult AnmManager::on_draw_3d_just_renders_layer_24(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 24);
}

ChainCallbackResult AnmManager::on_draw_3f_also_renders_layer_29(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    return renderLayer(This, 29);
}

ChainCallbackResult AnmManager::on_draw_3e_also_renders_layer_30(void* args)
{
    AnmManager* This = reinterpret_cast<AnmManager*>(args);
    g_supervisor.currentCam = &g_supervisor.cam0;
    g_supervisor.swapCameraTransformMatrices(&g_supervisor.cam0);
    g_supervisor.d3dDevice->SetViewport(&g_supervisor.currentCam->viewport);
    g_supervisor.camIndex = 0;
    ChainCallbackResult result = renderLayer(This, 30);
    g_supervisor.currentCam = &g_supervisor.cam1;
    g_supervisor.swapCameraTransformMatrices(&g_supervisor.cam1);
    g_supervisor.d3dDevice->SetViewport(&g_supervisor.currentCam->viewport);
    g_supervisor.camIndex = 1;
    return result;
}

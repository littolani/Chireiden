#include "GameConfig.h"

int NC_processGameConfig(Supervisor *supervisor)
{
  byte *th11ConfigFile;
  int status;
  int loopCounter;
  uint32_t *th11ConfigFileCopy;
  va_list vaList;
  uint32_t *start;
  char *errString;
  Supervisor *supervisorCopy;
  
  supervisorCopy = supervisor;
  GameConfig::GameConfig(&g_supervisor.gameCfg);
                    /* ???ok so we're using this as temporary storage */
  th11ConfigFile = FN_openFile("th11.cfg",(size_t *)&supervisor,1);
  if (th11ConfigFile == (byte *)0x0) {
    errString = "コンフィグデータが見つからないので初期化しました\r\n";
  }
  else {
    loopCounter = 0xf;
    th11ConfigFileCopy = (uint32_t *)th11ConfigFile;
    start = &g_supervisor.gameCfg.version;
                    /* take first 4 bytes of file */
    for (; loopCounter != 0; loopCounter = loopCounter + -1) {
      *start = *th11ConfigFileCopy;
      th11ConfigFileCopy = th11ConfigFileCopy + 1;
      start = start + 1;
    }
    _free(th11ConfigFile);
    if (((((g_supervisor.gameCfg.d < 2) && (g_supervisor.gameCfg.volumeMaybe < 3)) &&
         (g_supervisor.gameCfg.lifeCount < 2)) &&
        ((g_supervisor.gameCfg.mysteryField < 4 && (g_supervisor.gameCfg.colorMode16Bit < 3)))) &&
       ((g_supervisor.gameCfg.musicMode < 3 &&
        ((g_supervisor.gameCfg.version == 0x110003 && (supervisor == (Supervisor *)0x3c)))))) {
                    /* Copying because the fields had something odd going on */
      g_defaultGameConfig.shootButton = g_supervisor.gameCfg.shootButton;
      g_defaultGameConfig.bombButton = g_supervisor.gameCfg.bombButton;
      g_defaultGameConfig.focusButton = g_supervisor.gameCfg.focusButton;
      g_defaultGameConfig.menuButton = g_supervisor.gameCfg.menuButton;
      g_defaultGameConfig.u1 = g_supervisor.gameCfg.u1;
      g_defaultGameConfig.u2 = g_supervisor.gameCfg.u2;
      g_defaultGameConfig.refreshRate = g_supervisor.gameCfg.refreshRate;
      goto LoggingPortion;
    }
    errString = "コンフィグデータが異常でしたので再初期化しました\r\n";
  }
  FN_logToGlobalBuffer(&g_loggingBuffer,errString,vaList);
  GameConfig::GameConfig(&g_supervisor.gameCfg);
LoggingPortion:
  supervisorCopy->noVerticalSyncFlag = 0;
  if (((supervisorCopy->gameCfg).optionsFlag & 4) != 0) {
    FN_logToGlobalBuffer(&g_loggingBuffer,"フォグの使用を抑制します\r\n",vaList);
  }
  if (((supervisorCopy->gameCfg).optionsFlag & 1) != 0) {
    FN_logToGlobalBuffer
              (&g_loggingBuffer,"16Bit のテクスチャの使用を強制します\r\n",vaList);
  }
  if ((supervisorCopy->d3dPresetParameters).Windowed != 0) {
    FN_logToGlobalBuffer(&g_loggingBuffer,"ウィンドウモードで起動します\r\n",vaList);
  }
  if (((supervisorCopy->gameCfg).optionsFlag & 2) != 0) {
    FN_logToGlobalBuffer
              (&g_loggingBuffer,"リファレンスラスタライザを強制します\r\n",vaList)
    ;
  }
  if (((supervisorCopy->gameCfg).optionsFlag & 8) != 0) {
    FN_logToGlobalBuffer
              (&g_loggingBuffer,
               "パッド、キーボードの入力に DirectInput を使用しません\r\n",
               vaList);
  }
  if (((supervisorCopy->gameCfg).optionsFlag & 0x10) != 0) {
    FN_logToGlobalBuffer(&g_loggingBuffer,"ＢＧＭをメモリに読み込みます\r\n",vaList);
  }
  if (((supervisorCopy->gameCfg).optionsFlag & 0x20) != 0) {
    FN_logToGlobalBuffer(&g_loggingBuffer,"垂直同期を取りません\r\n",vaList);
    g_supervisor.noVerticalSyncFlag = 1;
  }
  if (((supervisorCopy->gameCfg).optionsFlag & 0x40) != 0) {
    FN_logToGlobalBuffer
              (&g_loggingBuffer,"文字描画の環境を自動検出しません\r\n",vaList);
  }
  status = FN_writeToFile("th11.cfg",0x3c,&g_supervisor.gameCfg);
  if (status != 0) {
    CL_logError("ファイルが書き出せません %s\r\n");
    CL_logError(
               "フォルダが書込み禁止属性になっているか、ディスクがいっぱいいっぱいになってませんか？\r\n"
               );
    return -1;
  }
  return 0;
}
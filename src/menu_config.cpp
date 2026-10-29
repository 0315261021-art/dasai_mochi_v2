#include "menu.h"
#include "settings.h"
#include <U8g2lib.h>

extern void toggleBluetooth();
extern void showMessageAndWait(const char* msg, int waitMs);
extern void saveSettings();

#define ITEM_COUNT 10
MenuItem dynamicMenuItems[ITEM_COUNT];

extern Menu* appMenu;
extern bool isBluetoothEnabled;
extern bool gifEnabled;
extern bool showClock;
extern bool menuMode;
extern MyAppConfig settings;
uint8_t gifDelaySec = settings.gifDelay / 1000;
int bluetoothMenu = isBluetoothEnabled;
uint8_t languageMenu = settings.language;
bool gifSoundMenu = settings.gif_sound;
void callback_language();

const char* menuLabel(int lang, const char* vi, const char* en) {
  return (lang == 1) ? en : vi;
}

void callback_save() {
  gifEnabled = false;
  vTaskDelay(50 / portTICK_PERIOD_MS);  // đảm bảo không có tranh chấp màn hình
  settings.gifDelay = gifDelaySec * 1000;
  settings.gif_sound = gifSoundMenu;
  saveSettings();
  vTaskDelay(20 / portTICK_PERIOD_MS);
  showMessageAndWait(menuLabel(languageMenu, "ĐÃ LƯU", "SAVED"), 1000);
  menuMode = false; // thoát khỏi menu
  if (isBluetoothEnabled != bluetoothMenu) {
      toggleBluetooth();
  }
  gifEnabled = !showClock;
}

void callback_exit(){
  menuMode = false; // thoát khỏi menu
  gifEnabled = !showClock;
}

void attachMenu(Menu& menu) {
  languageMenu = settings.language;
  gifDelaySec = settings.gifDelay / 1000;
  gifSoundMenu = settings.gif_sound;
  
  dynamicMenuItems[0] = { menuLabel(languageMenu, "Bluetooth", "Bluetooth"), MENU_TYPE_BOOL, &bluetoothMenu, 0, 1, nullptr };
  dynamicMenuItems[1] = { menuLabel(languageMenu, "Âm thanh GIF", "GIF Sound"), MENU_TYPE_BOOL, &gifSoundMenu, 0, 1, nullptr };
  dynamicMenuItems[2] = { menuLabel(languageMenu, "Âm bản", "Negative GIF"), MENU_TYPE_BOOL, &settings.isInverted, 0, 1, nullptr };
  dynamicMenuItems[3] = { menuLabel(languageMenu, "Lật màn hình", "Flip mode"), MENU_TYPE_BOOL, &settings.is_flip_mode, 0, 1, nullptr };
  dynamicMenuItems[4] = { menuLabel(languageMenu, "Tốc độ GIF", "GIF Speed"), MENU_TYPE_INT, &settings.gifSpeed, 0, 255, nullptr };
  dynamicMenuItems[5] = { "GIF Delay", MENU_TYPE_INT, &gifDelaySec, 1, 60, nullptr };
  dynamicMenuItems[6] = { menuLabel(languageMenu, "Âm lượng", "Volume"), MENU_TYPE_INT, &settings.volume, 0, 10, nullptr };
  dynamicMenuItems[7] = { menuLabel(languageMenu, "TG thông báo", "Noti time"), MENU_TYPE_INT, &settings.notifDuration, 1, 10, nullptr };
  dynamicMenuItems[8] = { menuLabel(languageMenu, "Lưu thiết lập", "Save settings"), MENU_TYPE_ACTION, nullptr, 0, 0, callback_save };
  dynamicMenuItems[9] = { menuLabel(languageMenu, "Thoát (không lưu)", "Exit (Don't save)"), MENU_TYPE_ACTION, nullptr, 0, 0, callback_exit };

  menu.setItems(dynamicMenuItems, ITEM_COUNT);
}

void callback_language() {
  // Gán tạm thời để preview label
  settings.language = languageMenu;

  // Gắn lại label mới theo ngôn ngữ
  if (appMenu) {
    attachMenu(*appMenu);     // cập nhật lại các label
    appMenu->update();        // vẽ lại ngay
  }
}
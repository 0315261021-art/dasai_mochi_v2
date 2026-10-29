#include "menu.h"
#include "Dongle_20.h"

Menu::Menu(U8G2& display)
  : u8g2(display), items(nullptr), itemCount(0), selectedIndex(0), scrollOffset(0), inEditMode(false) {}

void Menu::setItems(MenuItem* items, uint8_t count) {
  this->items = items;
  this->itemCount = count > MAX_MENU_ITEMS ? MAX_MENU_ITEMS : count;
  this->selectedIndex = 0;
  this->scrollOffset = 0;
}

void Menu::handleShortPress() {
  if (!inEditMode) {
    selectedIndex = (selectedIndex + 1) % itemCount;
    if (selectedIndex >= scrollOffset + VISIBLE_MENU_COUNT) {
      scrollOffset = selectedIndex - VISIBLE_MENU_COUNT + 1;
    } else if (selectedIndex < scrollOffset) {
      scrollOffset = selectedIndex;
    }
  } else {
    MenuItem& item = items[selectedIndex];
    if (item.type == MENU_TYPE_BOOL) {
      bool* val = (bool*)item.value;
      *val = !(*val);
      if (item.callback) item.callback();
    } else if (item.type == MENU_TYPE_INT) {
      uint8_t* val = (uint8_t*)item.value;
      *val = (*val < item.max) ? (*val + 1) : item.min;
    }
  }
}

void Menu::handleLongPress() {
  MenuItem& item = items[selectedIndex];
  if (inEditMode && item.callback && item.type == MENU_TYPE_INT) {
    item.callback();
  } else if (item.type == MENU_TYPE_ACTION) {
    if (item.callback) item.callback();
  } else {
    inEditMode = !inEditMode;
  }
}

void Menu::update() {
  draw();
}

void Menu::draw() {
  u8g2.clearBuffer();
  u8g2.enableUTF8Print();
  u8g2.setFont(Dongle_20);

  for (uint8_t i = 0; i < VISIBLE_MENU_COUNT && (scrollOffset + i) < itemCount; i++) {
    uint8_t idx = scrollOffset + i;
    drawItem(idx, idx == selectedIndex, i);
  }
  u8g2.sendBuffer();
}

void Menu::drawItem(uint8_t index, bool selected, uint8_t lineIndex) {
  char buffer[20];
  MenuItem& item = items[index];

  int y = 16 * (lineIndex + 1);
  int textY = y;
  int textX = 13;

  if (selected && inEditMode) {
    // Tô nền chữ nhật
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, y - 13, 128, 14);  // chữ cao ~12–14px

    u8g2.setDrawColor(0);  // chữ trắng trên nền đen
    u8g2.setCursor(textX, y);
    u8g2.print(item.label);

    u8g2.setCursor(100, y);
    if (item.type == MENU_TYPE_BOOL) {
      bool* val = (bool*)item.value;
      u8g2.print(*val ? "ON" : "OFF");
    } else if (item.type == MENU_TYPE_INT) {
      uint8_t* val = (uint8_t*)item.value;
      sprintf(buffer, "%d", *val);
      u8g2.print(buffer);
    }

    u8g2.setDrawColor(1);  // khôi phục lại trạng thái vẽ bình thường
  } else {
    // Chế độ bình thường (chưa chỉnh)
    if (selected) u8g2.drawStr(0, y + 1, "> ");
    u8g2.setCursor(textX, y);
    u8g2.print(item.label);

    u8g2.setCursor(100, y);
    if (item.type == MENU_TYPE_BOOL) {
      bool* val = (bool*)item.value;
      u8g2.print(*val ? "ON" : "OFF");
    } else if (item.type == MENU_TYPE_INT) {
      uint8_t* val = (uint8_t*)item.value;
      sprintf(buffer, "%d", *val);
      u8g2.print(buffer);
    } else if (item.type == MENU_TYPE_LANGUAGE) {
      // Hiển thị ngôn ngữ
      uint8_t* lang = (uint8_t*)item.value;
      u8g2.print(*lang == 0 ? "Vi" : "Eng");
    } 
  }
}

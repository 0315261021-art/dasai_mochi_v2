#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <U8g2lib.h>

#define MAX_MENU_ITEMS 12
#define VISIBLE_MENU_COUNT 4  // Số dòng hiển thị tối đa trên màn hình

typedef enum {
  MENU_TYPE_BOOL,
  MENU_TYPE_INT,
  MENU_TYPE_LANGUAGE,
  MENU_TYPE_ACTION
} MenuItemType;

typedef void (*MenuCallback)();

typedef struct {
  const char* label;
  MenuItemType type;
  void* value;
  uint8_t min;
  uint8_t max;
  MenuCallback callback;
} MenuItem;

class Menu {
  public:
    Menu(U8G2& display);
    void setItems(MenuItem* items, uint8_t count);
    void update();
    void handleShortPress();
    void handleLongPress();

  private:
    U8G2& u8g2;
    MenuItem* items;
    uint8_t itemCount;
    uint8_t selectedIndex;
    uint8_t scrollOffset;         // Vị trí bắt đầu hiển thị (cuộn)
    bool inEditMode;

    void draw();
    void drawItem(uint8_t index, bool selected, uint8_t lineIndex); // thêm lineIndex để biết dòng thứ mấy đang vẽ
};

#endif // MENU_H

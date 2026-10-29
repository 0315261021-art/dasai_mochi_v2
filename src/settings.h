// settings.h
#ifndef SETTINGS_H
#define SETTINGS_H
#define SETTINGS_MAGIC 0xABCD1234
#include <Arduino.h>
#include <vector> //

// Định nghĩa một kích thước tối đa cho mảng GIF được chọn,
// hoặc một cách để lưu trạng thái của từng GIF (đã chọn/chưa chọn)
#define MAX_SELECTED_GIFS 71 // Ví dụ: số lượng GIF tối đa mà sếp có (tầm 60 theo list của sếp)
// Cấu trúc để lưu trạng thái chọn của từng GIF
struct GifSelection {
    char name[32]; // Tên GIF, ví dụ "angry"
    bool selected;
};

struct MyAppConfig {
    uint32_t magicNumber;
    int gifSpeed;
    int gifDelay; // Có vẻ không dùng đến
    int buzzer_pin; // Số chân buzzer
    int touch_pin; // Số chân touch
    bool isInverted;
    char gifIntro[32]; 
    char gif_touch[32]; 
    char gif_double_touch[32]; 
    char gif_long_touch[32]; 
    bool gifSelected[MAX_SELECTED_GIFS]; // Mảng đánh dấu GIF nào được chọn
    
    int notifDuration; // Thời gian hiển thị thông báo (ms)

    int bluetooth_auto_off;
    char bluetoothName[16];
    char platform[8]; 

    char wifi_ssid[32]; // SSID WiFi
    char wifi_password[64]; // Mật khẩu WiFi
    char timezone[32]; // Múi giờ, ví dụ "GMT+7" hoặc "Asia/Ho_Chi_Minh"

    char display_type[16]; // "ssd1306" hoặc "sh1106"
    int is_flip_mode = 0;
    int i2cSDA = 20; // DF: 20
    int i2cSCL = 21; // DF: 21

    int I2S_LRCK_PIN = 5;
    int I2S_BCK_PIN = 6; 
    int I2S_DATA_PIN = 7; 
    int volume = 5; // Giả sử volume từ 0 đến 10

    int use_sound = 0; // Sử dụng I2S để phát âm thanh
    bool gif_sound = true; // Sử dụng âm thanh GIF
    int language = 0; // Ngôn ngữ mặc định (0 = Tiếng Việt, 1 = English)

    char intro_sound[1024];
    char notification_sound[128];
    // TOUCH SOUND
    char touch_sound[1024]; // Âm thanh khi chạm
    char double_touch_sound[1024]; // Âm thanh khi chạm đúp
    char long_touch_sound[1024]; // Âm thanh khi chạm lâu
    
};

extern MyAppConfig settings; //

void loadSettings(); //
void saveSettings(); //
void defaultSettings();
void defaultSettings2();
void defaultSettings3();
void handleSerialSettings(); // Xử lý cài đặt từ Serial Monitor
void sendAllSettings(); // Gửi tất cả cài đặt qua Serial Monitor
#endif
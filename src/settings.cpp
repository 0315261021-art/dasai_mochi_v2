// settings.cpp
#include "settings.h" 
#include <EEPROM.h> 
#include <Arduino.h>
#include "gif_data.h"
#define VERSION "4.2.7"
#include <time.h>
#include <sys/time.h>

MyAppConfig settings; 
String temp_intro_sound = "";
String temp_touch_sound = "";
String temp_double_touch_sound = "";
String temp_long_touch_sound = "";
String temp_notification_sound = "";

void loadSettings() {
    EEPROM.begin(sizeof(settings));
    EEPROM.get(0, settings);
    if (settings.magicNumber != SETTINGS_MAGIC) {
        Serial.println("Thiết lập cài đặt mặc định do magic number không hợp lệ.");
        defaultSettings(); 
        defaultSettings2(); 
        defaultSettings3();
        return;
    }
}
void defaultSettings() {
    settings.magicNumber = SETTINGS_MAGIC;
    settings.gifSpeed = 130;
    settings.gifDelay = 5000;
    settings.isInverted = false;
    settings.gif_sound = true;
    strcpy(settings.gifIntro, "gundam");
    strcpy(settings.gif_touch, "mac_dinh");
    strcpy(settings.gif_double_touch, "mac_dinh");
    strcpy(settings.gif_long_touch, "mac_dinh");
    // Đặt tất cả GIF được chọn mặc định là TRUE
    // Sếp hãy đảm bảo NUM_GIF_ENTRIES hoặc allGifPointers.size() là đúng số lượng GIF nhé.
    for (int i = 0; i < MAX_SELECTED_GIFS; i++) {
        settings.gifSelected[i] = true;
    }
    settings.language = 0; // Ngôn ngữ mặc định là Tiếng Việt
}

void defaultSettings2(){    
    strcpy(settings.intro_sound, "G4 100 20 C5 100 20 E5 100 20 G5 100 20 C6 100 20 D6 100 20 E6 200 200");
    strcpy(settings.notification_sound, "G4 100 20 C5 100 20 E5 100 20 G5 200 200");
    strcpy(settings.touch_sound, "C4 100 20 D4 100 20 E4 100 20 F4 200 200");
    strcpy(settings.double_touch_sound, "C5 100 20 D5 100 20 E5 100 20 F5 200 200");
    strcpy(settings.long_touch_sound, "C6 100 20 D6 100 20 E6 100 20 F6 200 200");
}
void defaultSettings3(){
    // Mặc định phần cứng
    settings.i2cSDA = 20; // Chân SDA mặc định
    settings.i2cSCL = 21; // Chân SCL mặc định
    settings.notifDuration = 3; // Thời gian hiển thị thông báo mặc định
    settings.bluetooth_auto_off = 3; // Thời gian hiển thị thông báo mặc định
    strcpy(settings.display_type, "ssd1306");
    settings.is_flip_mode = 0; // Chế độ hiển thị mặc định là không lật
    strcpy(settings.bluetoothName, "THE MOCHI");
    strcpy(settings.platform, "android");
    settings.buzzer_pin = 2;
    settings.touch_pin = 22;

    settings.I2S_LRCK_PIN = 5; // Chân LRCK mặc định
    settings.I2S_BCK_PIN = 6;  // Chân BCK mặc định
    settings.I2S_DATA_PIN = 7; // Chân DATA mặc định
    settings.volume = 5; // Giả sử volume từ 0 đến 10
    settings.use_sound = 1; // 0 = ko dùng , 1 = buzzer, 2 = I2S

    settings.wifi_ssid[0] = '\0'; // SSID WiFi mặc định là rỗng
    settings.wifi_password[0] = '\0'; // Mật khẩu WiFi mặc
    strcpy(settings.timezone, "UTC+7"); // Múi giờ mặc định là ICT (UTC+7)

}

void saveSettings() {
    EEPROM.begin(sizeof(settings));  // Cần thiết để bắt đầu
    EEPROM.put(0, settings);
    EEPROM.commit();
    EEPROM.end();  // Giải phóng nếu cần
    Serial.println("✅ Đã lưu cài đặt vào EEPROM.");
}

void handleSerialSettings() {
    static String input = "";

    while (Serial.available()) {
        input += (char)Serial.read();
    }

    int newLineIndex;
    while ((newLineIndex = input.indexOf('\n')) != -1) {
        String line = input.substring(0, newLineIndex);
        input = input.substring(newLineIndex + 1);

        line.trim();
        if (line.length() == 0) continue;

        if (line.startsWith("time:")) {
            String value = line.substring(5); // cắt sau "time:"
            value.trim();

            time_t timestamp = value.toInt();
            if (timestamp > 1000000000) { // khoảng sau năm 2001
                struct timeval now = { .tv_sec = timestamp };
                settimeofday(&now, NULL);
            } 
        }else if (line.startsWith("set:")) {
            String command = line.substring(4);

            if (command == "default1") {
                defaultSettings();
            } else if (command == "default2") {
                defaultSettings2();
            } else if (command == "default3") {
                defaultSettings3();
            } else if (command == "get") {
                sendAllSettings();
            } else if (command == "save") {
                saveSettings();
                Serial.println("Return:Đã lưu cài đặt.");
            } else if (command == "reset") {
                delay(500);
                ESP.restart();
                Serial.println("Return:Đã reset.");
            } else {
                Serial.println("Return:Lệnh không hợp lệ");
            }
        } else {
            int sep = line.indexOf(':');
            if (sep > 0) {
                String key = line.substring(0, sep);
                String value = line.substring(sep + 1);

                key.trim();
                value.trim();

                // ==== Xử lý chuỗi chia nhỏ ==== //
                if (key == "intro_sound") {
                    temp_intro_sound = value;
                } else if (key == "intro_sound_cont") {
                    temp_intro_sound += value;
                } else if (key == "intro_sound_end") {
                    strncpy(settings.intro_sound, temp_intro_sound.c_str(), sizeof(settings.intro_sound) - 1);
                    settings.intro_sound[sizeof(settings.intro_sound) - 1] = '\0';
                } else if (key == "touch_sound") {
                    temp_touch_sound = value;
                } else if (key == "touch_sound_cont") {
                    temp_touch_sound += value;
                } else if (key == "touch_sound_end") {
                    strncpy(settings.touch_sound, temp_touch_sound.c_str(), sizeof(settings.touch_sound) - 1);
                    settings.touch_sound[sizeof(settings.touch_sound) - 1] = '\0';
                } else if (key == "double_touch_sound") {
                    temp_double_touch_sound = value;
                } else if (key == "double_touch_sound_cont") {
                    temp_double_touch_sound += value;
                } else if (key == "double_touch_sound_end") {
                    strncpy(settings.double_touch_sound, temp_double_touch_sound.c_str(), sizeof(settings.double_touch_sound) - 1);
                    settings.double_touch_sound[sizeof(settings.double_touch_sound) - 1] = '\0';
                } else if (key == "long_touch_sound") {
                    temp_long_touch_sound = value;
                } else if (key == "long_touch_sound_cont") {
                    temp_long_touch_sound += value;
                } else if (key == "long_touch_sound_end") {
                    strncpy(settings.long_touch_sound, temp_long_touch_sound.c_str(), sizeof(settings.long_touch_sound) - 1);
                    settings.long_touch_sound[sizeof(settings.long_touch_sound) - 1] = '\0';
                } else if (key == "notification_sound") {
                    temp_notification_sound = value;
                } else if (key == "notification_sound_cont") {
                    temp_notification_sound += value;
                } else if (key == "notification_sound_end") {
                    strncpy(settings.notification_sound, temp_notification_sound.c_str(), sizeof(settings.notification_sound) - 1);
                    settings.notification_sound[sizeof(settings.notification_sound) - 1] = '\0';

                // ==== Các key bình thường khác ==== //
                } else if (key == "gif_speed") {
                    settings.gifSpeed = value.toInt();
                } else if (key == "gif_delay") {
                    settings.gifDelay = value.toInt();
                } else if (key == "isInverted") {
                    settings.isInverted = (value == "1" || value == "true");
                } else if (key == "gif_intro") {
                    value.toCharArray(settings.gifIntro, sizeof(settings.gifIntro));
                } else if (key == "gif_touch") {
                    value.toCharArray(settings.gif_touch, sizeof(settings.gif_touch));
                } else if (key == "gif_double_touch") {
                    value.toCharArray(settings.gif_double_touch, sizeof(settings.gif_double_touch));
                } else if (key == "gif_long_touch") {
                    value.toCharArray(settings.gif_long_touch, sizeof(settings.gif_long_touch));
                } else if (key == "gif_selected_bin") {
                    int len = min((int)value.length(), MAX_SELECTED_GIFS);
                    for (int i = 0; i < len; i++) {
                        settings.gifSelected[i] = (value.charAt(i) == '1');
                    }
                    loadSelectedGifs();
                } else if (key == "notifDuration") {
                    settings.notifDuration = value.toInt();
                } else if (key == "bluetooth_auto_off") {
                    settings.bluetooth_auto_off = value.toInt();
                } else if (key == "bluetoothName") {
                    value.toCharArray(settings.bluetoothName, sizeof(settings.bluetoothName));
                } else if (key == "platform") {
                    value.toCharArray(settings.platform, sizeof(settings.platform));
                } else if (key == "display_type") {
                    value.toCharArray(settings.display_type, sizeof(settings.display_type));
                } else if (key == "is_flip_mode") {
                    settings.is_flip_mode = value.toInt();
                } else if (key == "buzzer_pin") {
                    settings.buzzer_pin = value.toInt();
                } else if (key == "touch_pin") {
                    settings.touch_pin = value.toInt();
                } else if (key == "sda_pin") {
                    settings.i2cSDA = value.toInt();
                } else if (key == "scl_pin") {
                    settings.i2cSCL = value.toInt();
                } else if (key == "I2S_LRCK_PIN") {
                    settings.I2S_LRCK_PIN = value.toInt();
                } else if (key == "I2S_BCK_PIN") {
                    settings.I2S_BCK_PIN = value.toInt();
                } else if (key == "I2S_DATA_PIN") {
                    settings.I2S_DATA_PIN = value.toInt();
                } else if (key == "volume") {
                    settings.volume = value.toInt();
                } else if (key == "use_sound") {
                    settings.use_sound = value.toInt();
                } else if (key == "gif_sound") {
                    settings.gif_sound = (value == "1" || value == "true");
                } else if (key == "language") {
                    if (value == "0" || value == "1") {
                        settings.language = value.toInt();
                    } 
                } else if (key == "wifi_ssid") {
                    value.toCharArray(settings.wifi_ssid, sizeof(settings.wifi_ssid));
                } else if (key == "wifi_password") {
                    value.toCharArray(settings.wifi_password, sizeof(settings.wifi_password));  
                } else if (key == "timezone") {
                    value.toCharArray(settings.timezone, sizeof(settings.timezone));
                } else {
                    Serial.println("Khóa không hợp lệ: " + key);
                }
            }
        }
        delay(5);
    }
}


String getGifSelectedBitString() {
    String result = "gif_selected:";
    for (int i = 0; i < MAX_SELECTED_GIFS; i++) {
        result += settings.gifSelected[i] ? '1' : '0';
    }
    return result;
}

void sendAllSettings() {
  Serial.println("version:" + String(VERSION));
    
  // Gửi từng cài đặt theo format field_name:value
  Serial.println("gif_speed:" + String( settings.gifSpeed));
  Serial.println("gif_delay:" + String( settings.gifDelay));
  Serial.println("gif_negative:" + String(settings.isInverted ? "true" : "false"));
  Serial.println("gif_intro:" + String(settings.gifIntro));
  Serial.println("gif_touch:" + String(settings.gif_touch));
  Serial.println("gif_double_touch:" + String(settings.gif_double_touch));
  Serial.println("gif_long_touch:" + String(settings.gif_long_touch));
  Serial.println(getGifSelectedBitString());

  // MUSIC
  Serial.println("intro_sound:" + String(settings.intro_sound));
  Serial.println("notification_sound:" + String(settings.notification_sound));
  Serial.println("touch_sound:" + String(settings.touch_sound));
  Serial.println("double_touch_sound:" + String(settings.double_touch_sound));
  Serial.println("long_touch_sound:" + String(settings.long_touch_sound));

  // PLATFORM
    Serial.println("notifDuration:" + String(settings.notifDuration));
    Serial.println("bluetooth_auto_off:" + String(settings.bluetooth_auto_off));
    Serial.println("bluetoothName:" + String(settings.bluetoothName));
    Serial.println("platform:" + String(settings.platform));


  // Phần cứng
  Serial.println("scl_pin:" + String(settings.i2cSCL));
  Serial.println("sda_pin:" + String(settings.i2cSDA));
  Serial.println("buzzer_pin:" + String(settings.buzzer_pin));
  Serial.println("touch_pin:" + String(settings.touch_pin));
  Serial.println("display_type:" + String(settings.display_type));
    Serial.println("is_flip_mode:" + String(settings.is_flip_mode));
    Serial.println("I2S_LRCK_PIN:" + String(settings.I2S_LRCK_PIN));
    Serial.println("I2S_BCK_PIN:" + String(settings.I2S_BCK_PIN));
    Serial.println("I2S_DATA_PIN:" + String(settings.I2S_DATA_PIN));
    Serial.println("volume:" + String(settings.volume));
    Serial.println("use_sound:" + String(settings.use_sound));
    Serial.println("gif_sound:" + String(settings.gif_sound ? "true" : "false"));
    Serial.println("language:" + String(settings.language));
    Serial.println("wifi_ssid:" + String(settings.wifi_ssid));
    Serial.println("wifi_password:" + String(settings.wifi_password));
    Serial.println("timezone:" + String(settings.timezone));

}
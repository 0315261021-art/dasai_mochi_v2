#include "sound.h"
#include "settings.h"
#include "SPIFFS.h"
#include "driver/i2s.h"
#include <cstdint>
#include <cstdlib>

static inline float randf() {
    return rand() / (RAND_MAX + 1.0f);
}

TaskHandle_t soundTaskHandle = NULL;
String queuedSound = "";
volatile bool isPlaying = false;


#define I2S_NUM      I2S_NUM_0
#define BUF_SIZE     1024

// Định nghĩa thông số WAV hiện tại
static uint16_t wavBits = 16;
static bool     wavStereo = false;


struct NoteFreq {
  const char* name;
  int freq;
};

NoteFreq notes[] = {
  {"B0", 31}, {"C1", 33}, {"CS1", 35}, {"D1", 37}, {"DS1", 39}, {"E1", 41},
  {"F1", 44}, {"FS1", 46}, {"G1", 49}, {"GS1", 52}, {"A1", 55}, {"AS1", 58}, {"B1", 62},
  {"C2", 65}, {"CS2", 69}, {"D2", 73}, {"DS2", 78}, {"E2", 82}, {"F2", 87}, {"FS2", 93},
  {"G2", 98}, {"GS2", 104}, {"A2", 110}, {"AS2", 117}, {"B2", 123},
  {"C3", 131}, {"CS3", 139}, {"D3", 147}, {"DS3", 156}, {"E3", 165}, {"F3", 175}, {"FS3", 185},
  {"G3", 196}, {"GS3", 208}, {"A3", 220}, {"AS3", 233}, {"B3", 247},
  {"C4", 262}, {"CS4", 277}, {"D4", 294}, {"DS4", 311}, {"E4", 330}, {"F4", 349}, {"FS4", 370},
  {"G4", 392}, {"GS4", 415}, {"A4", 440}, {"AS4", 466}, {"B4", 494},
  {"C5", 523}, {"CS5", 554}, {"D5", 587}, {"DS5", 622}, {"E5", 659}, {"F5", 698}, {"FS5", 740},
  {"G5", 784}, {"GS5", 831}, {"A5", 880}, {"AS5", 932}, {"B5", 988},
  {"C6", 1047}, {"CS6", 1109}, {"D6", 1175}, {"DS6", 1245}, {"E6", 1319}, {"F6", 1397}, {"FS6", 1480},
  {"G6", 1568}, {"GS6", 1661}, {"A6", 1760}, {"AS6", 1865}, {"B6", 1976},
  {"C7", 2093}, {"CS7", 2217}, {"D7", 2349}, {"DS7", 2489}, {"E7", 2637}, {"F7", 2794}, {"FS7", 2960},
  {"G7", 3136}, {"GS7", 3322}, {"A7", 3520}, {"AS7", 3729}, {"B7", 3951},
  {"C8", 4186}, {"CS8", 4435}, {"D8", 4699}, {"DS8", 4978}
};

int getFrequency(const String& noteName) {
  for (auto& note : notes) {
    if (noteName.equalsIgnoreCase(note.name)) {
      return note.freq;
    }
  }
  return 0;
}


void setupI2S(uint32_t sample_rate, i2s_bits_per_sample_t bits_per_sample, bool stereo) {
  // Gỡ driver cũ (nếu có)
  i2s_driver_uninstall(I2S_NUM);

  i2s_config_t cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = sample_rate,
    .bits_per_sample = bits_per_sample,
    .channel_format = stereo
        ? I2S_CHANNEL_FMT_RIGHT_LEFT
        : I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 512,
    .use_apll = true,
    .tx_desc_auto_clear = true
  };

  i2s_pin_config_t pins = {
    .bck_io_num   = settings.I2S_BCK_PIN,
    .ws_io_num    = settings.I2S_LRCK_PIN,
    .data_out_num = settings.I2S_DATA_PIN,
    .data_in_num  = I2S_PIN_NO_CHANGE
  };

  esp_err_t err = i2s_driver_install(I2S_NUM, &cfg, 0, nullptr);
  if (err != ESP_OK) {
   Serial.printf("i2s_driver_install failed: %d\n", err);
    return;
  }
  i2s_set_pin(I2S_NUM, &pins);
}

void tone_i2s(int freq, int duration_ms) {
  // Thiết lập I2S ở sample rate 44100 Hz, 16-bit, mono
  const uint32_t sample_rate = 44100;
  const uint16_t amplitude = (uint16_t)(32767 * 0.5f);
  setupI2S(sample_rate, I2S_BITS_PER_SAMPLE_16BIT, false);

  size_t samples_per_period = sample_rate / freq;
  size_t total_samples = (size_t)sample_rate * duration_ms / 1000;
  static int16_t buf[BUF_SIZE];
  size_t sent = 0;

  while (sent < total_samples) {
    size_t chunk = min((size_t)BUF_SIZE, total_samples - sent);
    for (size_t i = 0; i < chunk; i++) {
      size_t idx = (sent + i) % samples_per_period;
      buf[i] = (idx < samples_per_period/2) ? amplitude : -amplitude;
    }
    size_t bytes_written = 0;
    i2s_write(I2S_NUM, buf, chunk * sizeof(int16_t), &bytes_written, portMAX_DELAY);
    sent += chunk;
  }

  // Xoá buffer DMA để tắt âm
  i2s_zero_dma_buffer(I2S_NUM);
}

void soundTask(void* param) {
  while (true) {
    if (isPlaying && queuedSound.length() > 0) {
      String input = queuedSound;
      input.trim();
      
      // Debug: In ra chuỗi để kiểm tra
      Serial.println("Đang phát: " + input);

      if (input.indexOf(".mp3") != -1 && settings.use_sound == 2) {
        input += ",";  // thêm dấu phẩy cuối để dễ xử lý
        int idxStart = 0;
        
        while (idxStart < input.length()) {
          int commaPos = input.indexOf(',', idxStart);
          if (commaPos == -1) break;

          String name = input.substring(idxStart, commaPos);
          name.trim();  // loại bỏ khoảng trắng nếu có

          if (name.endsWith(".mp3")) {
            String filename = name.substring(0, name.length() - 4);        
            filename += ".wav";
            filename = "/" + filename;

            playWav(filename.c_str());
          }

          idxStart = commaPos + 1;
        }

        isPlaying = false;
        queuedSound = "";
      }
      int idx = 0;
      
      // Kiểm tra xem có khoảng trắng không để quyết định format
      bool hasSpaces = input.indexOf(' ') != -1;
      
      if (hasSpaces) {
        // FORMAT 1: "C5 200 30 D5 200 30" (có khoảng trắng)
        input += " "; // thêm space cuối để xử lý dễ hơn
        
        while (idx < input.length()) {
          int space1 = input.indexOf(' ', idx);
          if (space1 == -1) break;
          
          String note = input.substring(idx, space1);
          idx = space1 + 1;
          
          int space2 = input.indexOf(' ', idx);
          if (space2 == -1) break;
          
          String durationStr = input.substring(idx, space2);
          idx = space2 + 1;
          
          int space3 = input.indexOf(' ', idx);
          String restStr = "0";
          
          if (space3 != -1) {
            restStr = input.substring(idx, space3);
            idx = space3 + 1;
          } else {
            // Không có rest, lấy đến hết chuỗi
            int nextSpace = input.indexOf(' ', idx);
            if (nextSpace != -1) {
              restStr = input.substring(idx, nextSpace);
              idx = nextSpace + 1;
            } else {
              break;
            }
          }
          
          int freq = getFrequency(note);
          int duration = durationStr.toInt();
          int rest = restStr.toInt();    
          
          if (freq > 0 && duration > 0) {
            if(settings.use_sound == 2) {
              // Sử dụng I2S để phát âm thanh
              tone_i2s(freq, duration);
              delay(duration + rest);
            } else {
              // Sử dụng tone() để phát âm thanh
              tone(settings.buzzer_pin, freq, duration);
              delay(duration + rest);
            }
          }
        }
      } else {
        // FORMAT 2: "C5200D5200E5200" (không có khoảng trắng)
        while (idx < input.length()) {
          bool matched = false;
          
          // Thử tìm note dài nhất khớp trước (để tránh C# bị nhận thành C)
          String longestMatch = "";
          int longestMatchIndex = -1;
          int longestMatchLength = 0;
          
          for (int i = 0; i < sizeof(notes) / sizeof(NoteFreq); i++) {
            String noteName = notes[i].name;
            int len = noteName.length();
            
            if (idx + len <= input.length() && 
                input.substring(idx, idx + len).equalsIgnoreCase(noteName) &&
                len > longestMatchLength) {
              longestMatch = noteName;
              longestMatchIndex = i;
              longestMatchLength = len;
            }
          }
          
          if (longestMatchIndex != -1) {
            idx += longestMatchLength;
            
            // Lấy số duration phía sau note
            int durStart = idx;
            while (idx < input.length() && isDigit(input.charAt(idx))) {
              idx++;
            }
            
            if (durStart < idx) { // Có duration
              String durationStr = input.substring(durStart, idx);
              int freq = notes[longestMatchIndex].freq;
              int duration = durationStr.toInt();
              
              if (freq > 0 && duration > 0) {
                if(settings.use_sound == 2) {
                  tone_i2s(freq, duration);
                  delay(duration);
                } else {
                tone(settings.buzzer_pin, freq, duration);
                  delay(duration);
                }
              }
              matched = true;
            }
          }
          
          if (!matched) {
            idx++; // tránh lặp vô hạn
          }
        }
      }
      
      isPlaying = false;
      queuedSound = "";
    }
    delay(10);
  }
}

void initSoundTask() {
  xTaskCreatePinnedToCore(
    soundTask,
    "soundTask",
    8192,
    NULL,
    1,
    &soundTaskHandle,
    1
  );
}

void playSoundAsync(const String& str) {
  queuedSound = str;
  isPlaying = true;
}

bool isSoundPlaying() {
  return isPlaying;
}

int getSoundDurationMs(const String& melody) {
  String input = melody;
  input.trim();
  input += " ";

  int idx = 0;
  int total = 0;

  while (idx < input.length()) {
    // Lấy note
    int space1 = input.indexOf(' ', idx);
    if (space1 == -1) break;
    idx = space1 + 1;

    // Lấy duration
    int space2 = input.indexOf(' ', idx);
    if (space2 == -1) break;
    String durationStr = input.substring(idx, space2);
    idx = space2 + 1;

    // Lấy delay (pause) nếu có
    int space3 = input.indexOf(' ', idx);
    String delayStr;
    if (space3 == -1) {
      delayStr = "0";
      idx = input.length(); // kết thúc
    } else {
      delayStr = input.substring(idx, space3);
      idx = space3 + 1;
    }

    int duration = durationStr.toInt();
    int pause = delayStr.toInt();
    total += duration + pause;
  }

  return total;
}


bool beginWav(File &f, uint32_t &dataOffset) {
  uint8_t riff_hdr[44];
  if (f.read(riff_hdr, 44) != 44) return false;
  if (memcmp(riff_hdr, "RIFF", 4) || memcmp(riff_hdr + 8, "WAVE", 4)) return false;

  uint32_t sr   = *(uint32_t*)(riff_hdr + 24);
  uint16_t bits = *(uint16_t*)(riff_hdr + 34);
  uint16_t ch   = *(uint16_t*)(riff_hdr + 22);

  // Lưu thông tin định dạng
  wavBits   = bits;
  wavStereo = (ch > 1);

  // Tìm vị trí data chunk
  uint8_t chunk_hdr[8];
  uint32_t pos = 36;
  while (pos + 8 < f.size()) {
    f.seek(pos);
    if (f.read(chunk_hdr, 8) != 8) return false;
    uint32_t len = *(uint32_t*)(chunk_hdr + 4);
    if (!memcmp(chunk_hdr, "data", 4)) {
      dataOffset = pos + 8;
      break;
    }
    pos += 8 + len;
  }

  // Thiết lập I2S luôn 16-bit output
  setupI2S(sr, I2S_BITS_PER_SAMPLE_16BIT, wavStereo);

  //Serial.printf("WAV format: SR=%u Hz, bits=%u, ch=%u\n", sr, wavBits, wavStereo ? 2 : 1);
  return true;
}
void play4bitPCM(const uint8_t* inBuf, size_t bytesRead,
                 int16_t* outBuf, float volume, size_t& sampleCount) {
    // 1) Chuẩn bị LUT ánh xạ nibble → int16
    static bool lutInit = false;
    static int16_t lut[16];
    if (!lutInit) {
        for (int i = 0; i < 16; i++) {
            lut[i] = (int16_t)(((i / 15.0f) * 2.0f - 1.0f) * 32767.0f);
        }
        lutInit = true;
    }

    size_t outIndex = 0;
    for (size_t i = 0; i < bytesRead; ++i) {
        uint8_t b = inBuf[i];
        // Xử lý cả hai nibble (cao rồi thấp)
        for (int shift : {4, 0}) {
            uint8_t nib = (b >> shift) & 0x0F;
            // 2) Lấy giá trị cơ bản từ LUT
            int32_t sample = lut[nib];

            // 3) TPDF Dither: u1+u2−1 ∈ [−1..1] rồi scale bằng 1/15 LSB
            float dither = (randf() + randf() - 1.0f) * (1.0f/15.0f);
            sample = (int32_t)((sample + dither * 32767.0f) * volume);

            // 4) Clamp
            if (sample >  32767) sample =  32767;
            if (sample < -32768) sample = -32768;

            outBuf[outIndex++] = (int16_t)sample;
        }
    }
    sampleCount = outIndex;
}

void playWav(const char* path) {
  File f = SPIFFS.open(path);
  if (!f) { Serial.println("Không mở được file"); return; }
  uint32_t ofs;
  if (!beginWav(f, ofs)) { Serial.println("Không phải WAV PCM"); return; }
  f.seek(ofs);

  uint8_t  inBuf[BUF_SIZE];
  static int16_t outBuf[BUF_SIZE * 2]; // dự phòng gấp đôi cho stereo
  size_t   bytesRead = 0, bytesWritten = 0;
  const size_t bytePerSample = wavBits / 8;

  while ((bytesRead = f.read(inBuf, BUF_SIZE)) > 0) {
    size_t sampleCount = bytesRead / bytePerSample;

    // Chuyển qui mẫu về int16 và áp (float)settings.volume
    if (wavBits == 4) {
      play4bitPCM(inBuf, bytesRead, outBuf, (float)settings.volume, sampleCount);
    } else if (wavBits == 8) {
      // 8-bit PCM unsigned -> signed int16
      for (size_t i = 0; i < sampleCount; i++) {
        int16_t s = ((int)inBuf[i] - 128) << 8;
        int32_t v = (int32_t)s * (float)settings.volume;
        if (v > 32767) v = 32767;
        if (v < -32768) v = -32768;
        outBuf[i] = (int16_t)v;
      }
    } else {
      // 16-bit PCM signed little-endian
      int16_t* samples = (int16_t*)inBuf;
      for (size_t i = 0; i < sampleCount; i++) {
        int32_t v = (int32_t)samples[i] * (float)settings.volume;
        if (v > 32767) v = 32767;
        if (v < -32768) v = -32768;
        outBuf[i] = (int16_t)v;
      }
    }

    if (!wavStereo) {
      // Mono
      i2s_write(I2S_NUM, outBuf, sampleCount * sizeof(int16_t), &bytesWritten, portMAX_DELAY);
    } else {
      // Nhân đôi sang stereo
      for (size_t i = 0; i < sampleCount; i++) {
        outBuf[2 * i    ] = outBuf[i];
        outBuf[2 * i + 1] = outBuf[i];
      }
      i2s_write(I2S_NUM, outBuf, sampleCount * 2 * sizeof(int16_t), &bytesWritten, portMAX_DELAY);
    }
  }

  f.close();
}


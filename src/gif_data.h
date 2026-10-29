// gif_data.h
#ifndef GIF_DATA_H
#define GIF_DATA_H

#include <Arduino.h> // Để sử dụng String, v.v.
#include <vector> //
#include "U8g2lib.h" // Cần để có định nghĩa AnimatedGIF

#define ANIMATED_GIF_DEFINED
typedef struct {
    const uint8_t frame_count;      // Số lượng khung hình
    const uint16_t width;           // Chiều rộng GIF
    const uint16_t height;          // Chiều cao GIF
    const uint16_t* delays;         // Con trỏ đến mảng thời gian delay
    const uint8_t (* frames)[1024]; // Con trỏ đến mảng dữ liệu khung hình
} AnimatedGIF;

// Khai báo extern cho các con trỏ GIF
extern const AnimatedGIF gundam_gif;
extern const AnimatedGIF angry_gif;
extern const AnimatedGIF angry2_gif;
extern const AnimatedGIF angry3_gif;
extern const AnimatedGIF aot_gif;
extern const AnimatedGIF bee_gif;
extern const AnimatedGIF cry_gif;
extern const AnimatedGIF cry2_gif;
extern const AnimatedGIF cuoi_khinh_bi_gif;
extern const AnimatedGIF chong_mat_gif;
extern const AnimatedGIF dasai_gif;
extern const AnimatedGIF dasai2_gif;
extern const AnimatedGIF mac_dinh_gif;
extern const AnimatedGIF demon_slayer_gif;
extern const AnimatedGIF den_giao_thong_gif;
extern const AnimatedGIF den_pha_gif;
extern const AnimatedGIF devil_gif;
extern const AnimatedGIF devil_2_gif;
extern const AnimatedGIF devil_eyes_gif;
extern const AnimatedGIF distracted_gif;
extern const AnimatedGIF egg_gif;
extern const AnimatedGIF gian_du_gif;
extern const AnimatedGIF yakura_gif;
extern const AnimatedGIF hand_some_gif;
extern const AnimatedGIF happy_2_gif;
extern const AnimatedGIF hat_xi_gif;
extern const AnimatedGIF hat_xi_2_gif;
extern const AnimatedGIF hehe_gif;
extern const AnimatedGIF intro_gif;
extern const AnimatedGIF keep_it_up_gif;
extern const AnimatedGIF leu_leu_gif;
extern const AnimatedGIF love_gif;
extern const AnimatedGIF love2_gif;
extern const AnimatedGIF mochi_gif;
extern const AnimatedGIF mochidoo_gif;
extern const AnimatedGIF music_gif;
extern const AnimatedGIF neon_gif;
extern const AnimatedGIF noname_gif;
extern const AnimatedGIF nheo_mat_gif;
extern const AnimatedGIF nhin_ben_phai_gif;
extern const AnimatedGIF nhin_ben_trai_gif;
extern const AnimatedGIF nhin_xuong_gif;
extern const AnimatedGIF police_gif;
extern const AnimatedGIF pong_gif;
extern const AnimatedGIF rain_gif;
extern const AnimatedGIF sakura_gif;
extern const AnimatedGIF samurai_gif;
extern const AnimatedGIF scared_gif;
extern const AnimatedGIF sleepy_gif;
extern const AnimatedGIF smile_gif;
extern const AnimatedGIF speed_gif;
extern const AnimatedGIF star_gif;
extern const AnimatedGIF sung_nuoc_gif;
extern const AnimatedGIF sung_nuoc_2_gif;
extern const AnimatedGIF sushi_gif;
extern const AnimatedGIF turbo_gif;
extern const AnimatedGIF thong_long_gif;
extern const AnimatedGIF thu_nho_gif;
extern const AnimatedGIF up_size_down_gif;
extern const AnimatedGIF UwU_gif;
extern const AnimatedGIF wheels_gif;
extern const AnimatedGIF xi_khoi_gif;
extern const AnimatedGIF xi_lua_gif;
extern const AnimatedGIF speed2_gif;
extern const AnimatedGIF yelling_gif;
extern const AnimatedGIF car_gif;
extern const AnimatedGIF intro_3_gif;
extern const AnimatedGIF LB_gif;
extern const AnimatedGIF LB_intro_gif;
extern const AnimatedGIF LB_star_gif;
extern const AnimatedGIF speed3_gif;




// Cấu trúc để ánh xạ tên GIF với con trỏ của nó
struct GifEntry {
    const char* name;
    const AnimatedGIF* ptr;
};

// Khai báo mảng chứa tất cả các GIF và tên của chúng
extern const GifEntry allGifEntries[];
extern const int NUM_GIF_ENTRIES;
extern std::vector<const AnimatedGIF*> allGifPointers;
void loadSelectedGifs();
#endif
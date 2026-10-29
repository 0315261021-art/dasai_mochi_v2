#include "gif_data.h"
#include "settings.h"


// Định nghĩa các GIF và tên của chúng
const GifEntry allGifEntries[] = {
    {"gundam", &gundam_gif},
    {"angry", &angry_gif},
    {"angry2", &angry2_gif},
    {"angry3", &angry3_gif},
    {"aot", &aot_gif},
    {"bee", &bee_gif},
    {"cry", &cry_gif},
    {"cry2", &cry2_gif},
    {"cuoi_khinh_bi", &cuoi_khinh_bi_gif},
    {"chong_mat", &chong_mat_gif},
    {"dasai", &dasai_gif},
    {"dasai2", &dasai2_gif},
    {"mac_dinh", &mac_dinh_gif},
    {"demon_slayer", &demon_slayer_gif},
    {"den_giao_thong", &den_giao_thong_gif},
    {"den_pha", &den_pha_gif},
    {"devil", &devil_gif},
    {"devil_2", &devil_2_gif},
    {"devil_eyes", &devil_eyes_gif},
    {"distracted", &distracted_gif},
    {"egg", &egg_gif},
    {"gian_du", &gian_du_gif},
    {"yakura", &yakura_gif},
    {"hand_some", &hand_some_gif},
    {"happy_2", &happy_2_gif},
    {"hat_xi", &hat_xi_gif},
    {"hat_xi_2", &hat_xi_2_gif},
    {"hehe", &hehe_gif},
    {"intro", &intro_gif},
    {"keep_it_up", &keep_it_up_gif},
    {"leu_leu", &leu_leu_gif},
    {"love", &love_gif},
    {"love2", &love2_gif},
    {"mochi", &mochi_gif},
    {"mochidoo", &mochidoo_gif},
    {"music", &music_gif},
    {"neon", &neon_gif},
    {"noname", &noname_gif},
    {"nheo_mat", &nheo_mat_gif},
    {"nhin_ben_phai", &nhin_ben_phai_gif},
    {"nhin_ben_trai", &nhin_ben_trai_gif},
    {"nhin_xuong", &nhin_xuong_gif},
    {"police", &police_gif},
    {"pong", &pong_gif},
    {"rain", &rain_gif},
    {"sakura", &sakura_gif},
    {"samurai", &samurai_gif},
    {"scared", &scared_gif},
    {"sleepy", &sleepy_gif},
    {"smile", &smile_gif},
    {"speed", &speed_gif},
    {"star", &star_gif},
    {"sung_nuoc", &sung_nuoc_gif},
    {"sung_nuoc_2", &sung_nuoc_2_gif},
    {"sushi", &sushi_gif},
    {"turbo", &turbo_gif},
    {"thong_long", &thong_long_gif},
    {"thu_nho", &thu_nho_gif},
    {"up_size_down", &up_size_down_gif},
    {"UwU", &UwU_gif},
    {"wheels", &wheels_gif},
    {"xi_khoi", &xi_khoi_gif},
    {"xi_lua", &xi_lua_gif},
    {"speed2", &speed2_gif},
    {"yelling", &yelling_gif},
    {"car", &car_gif},
    {"intro_3", &intro_3_gif},
    {"LB", &LB_gif},
    {"LB_intro", &LB_intro_gif},
    {"LB_Star", &LB_star_gif},
    {"speed3", &speed3_gif}
};
const int NUM_GIF_ENTRIES = sizeof(allGifEntries) / sizeof(allGifEntries[0]);

void loadSelectedGifs() {
    allGifPointers.clear(); 
    for (int i = 0; i < NUM_GIF_ENTRIES; i++) {
        if (i < MAX_SELECTED_GIFS && settings.gifSelected[i]) {
            allGifPointers.push_back(allGifEntries[i].ptr);
        }
    }
    if (allGifPointers.empty()) {
        for (int i = 0; i < NUM_GIF_ENTRIES; i++) {
            if (strcmp(allGifEntries[i].name, "mac_dinh") == 0) {
                allGifPointers.push_back(allGifEntries[i].ptr);
                break;
            }
        }
    }
}
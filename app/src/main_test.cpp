#include "miniaudio.h"
#include <iostream>

int main() {
    ma_engine engine;
    ma_result result = ma_engine_init(NULL, &engine);

    if (result != MA_SUCCESS) {
        std::cerr << "[T-10] FALLO: ma_engine_init = " << result << "\n";
        return 1;
    }

    std::cout << "[T-10] miniaudio OK: engine inicializado\n";
    std::cout << "[T-10] Sample rate: " << ma_engine_get_sample_rate(&engine) << " Hz\n";

    ma_engine_uninit(&engine);
    std::cout << "[T-10] PASA: init y uninit sin crashes\n";
    return 0;
}

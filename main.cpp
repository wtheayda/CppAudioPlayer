#include <iostream>
#include <SDL2/SDL.h>

// Ses çalma durumunu ve verilerini tutacak yapı
struct AudioData {
    Uint8* pos;
    Uint32 length;
};

// Ses çalma sırasında çağrılacak callback fonksiyonu
void MyAudioCallback(void* userdata, Uint8* stream, int len) {
    AudioData* audio = (AudioData*)userdata;

    if (audio->length == 0) {
        return;
    }

    Uint32 length = (Uint32)len;
    length = (length > audio->length ? audio->length : length);

    SDL_memcpy(stream, audio->pos, length);

    audio->pos += length;
    audio->length -= length;
}

int main(int argc, char* argv[]) {
    // 1. SDL Ses sistemini başlat
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL başlatılamadı! Hata: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 2. WAV dosyasını yükle
    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8* wavBuffer;
    
    // NOT: Buraya çalmak istediğiniz dosyanın tam yolunu yazın veya dosyayı projenin yanına koyun.
    if (SDL_LoadWAV("ornek.wav", &wavSpec, &wavBuffer, &wavLength) == NULL) {
        std::cerr << "WAV dosyası yüklenemedi! Hata: " << SDL_GetError() << std::endl;
        std::cerr << "Lutfen 'ornek.wav' adinda bir dosyanin proje klasorunde oldugundan emin olun." << std::endl;
        return 1;
    }

    // Callback fonksiyonunu ayarla
    AudioData audio;
    audio.pos = wavBuffer;
    audio.length = wavLength;

    wavSpec.callback = MyAudioCallback;
    wavSpec.userdata = &audio;

    // 3. Ses aygıtını aç
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    if (deviceId == 0) {
        std::cerr << "Ses aygıtı açılamadı! Hata: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 4. Oynatmayı başlat (Pause'u kaldır)
    SDL_PauseAudioDevice(deviceId, 0);

    std::cout << "Ses çalınıyor..." << std::endl;

    // Ses bitene kadar bekle (Basit bir döngü)
    while (audio.length > 0) {
        SDL_Delay(100);
    }

    // 5. Temizlik
    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavBuffer);
    SDL_Quit();

    std::cout << "Tamamlandı." << std::endl;
    return 0;
}
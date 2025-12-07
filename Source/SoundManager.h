#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <xaudio2.h>
#include <mmreg.h>

class SoundManager {
public:
    static SoundManager& Instance();

    void Initialize();
    void Finalize();

    void LoadSE(const std::string& key, const std::wstring& filepath);
    void PlaySE(const std::string& key, float volume = 1.0f);
    void StopAll();  // 追加：全ボイス停止＆破棄
    // ★ ここを空っぽではなく「毎フレーム呼ぶ」清掃処理に変更
    void Update();

    // 互換のダミーはこのままでもOK
    void LoadBGM(const std::string&, const std::wstring&) {}
    void PlayBGM(const std::string&, float = 1.0f) {}
    void StopBGM(const std::string&) {}

private:

    SoundManager() = default;

    IXAudio2* xAudio = nullptr;
    IXAudio2MasteringVoice* masterVoice = nullptr;

    struct SoundData {
        std::vector<BYTE> fmtBytes;   // fmt チャンク
        std::vector<BYTE> buffer;     // data チャンク
    };
    std::unordered_map<std::string, SoundData> sounds;

    // ★ アクティブな SourceVoice を保持
    std::vector<IXAudio2SourceVoice*> activeVoices;
};

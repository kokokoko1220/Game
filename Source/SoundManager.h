#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <xaudio2.h>
#include <mmreg.h>   // WAVEFORMAT 系（保険で追加）

class SoundManager {
public:
    static SoundManager& Instance();

    // 基本
    void Initialize();
    void Finalize();

    // 効果音（多重再生OK）
    void LoadSE(const std::string& key, const std::wstring& filepath);
    void PlaySE(const std::string& key, float volume = 1.0f);

    // --- 互換用のダミー（以前のコードが呼んでいてもビルドが通るように） ---
    void Update() {} // 何もしない
    void LoadBGM(const std::string&, const std::wstring&) {} // 何もしない
    void PlayBGM(const std::string&, float = 1.0f) {}        // 何もしない
    void StopBGM(const std::string&) {}                      // 何もしない

private:
    SoundManager() = default;

    IXAudio2* xAudio = nullptr;
    IXAudio2MasteringVoice* masterVoice = nullptr;

    struct SoundData {
        // ★ ここを変更：拡張WAV対応のため fmt チャンクを丸ごと保持
        std::vector<BYTE> fmtBytes;   // fmt chunk (WAVEFORMATEX/WAVEFORMATEXTENSIBLE 両対応)
        std::vector<BYTE> buffer;     // data chunk
    };
    std::unordered_map<std::string, SoundData> sounds;
};

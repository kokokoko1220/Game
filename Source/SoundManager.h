#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <xaudio2.h>
#include <mmreg.h>

class SoundManager {
public:
    static SoundManager& Instance();

    // ライフサイクル
    void Initialize();   // アプリ起動時に1回だけ
    void Finalize();     // アプリ終了時に1回だけ

    // 効果音
    void LoadSE(const std::string& key, const std::wstring& filepath);
    void PlaySE(const std::string& key, float volume = 1.0f);

    // 毎フレーム掃除 / 全停止（シーン遷移前）
    void Update();       // 再生終了した SourceVoice を破棄
    void StopAll();      // すべての SourceVoice を停止・破棄

    // シーン切替の安全フラグ（切替中はAPI無視）
    void Suspend() { suspended_ = true; }
    void Resume() { suspended_ = false; }

    // （互換ダミー：不要なら削除OK）
    void LoadBGM(const std::string&, const std::wstring&) {}
    void PlayBGM(const std::string&, float = 1.0f) {}
    void StopBGM(const std::string&) {}

private:
    SoundManager() = default;

    // XAudio2
    IXAudio2* xAudio = nullptr;
    IXAudio2MasteringVoice* masterVoice = nullptr;

    // ★ これが無くてエラーになっていました
    bool coinit_ = false;     // CoInitialize 済みか
    bool suspended_ = false;  // シーン切替中フラグ

    struct SoundData {
        std::vector<BYTE> fmtBytes; // fmt チャンク（WAVEFORMATEX/EXTENSIBLE 両対応）
        std::vector<BYTE> buffer;   // data チャンク
    };
    std::unordered_map<std::string, SoundData> sounds;

    // 再生中の SourceVoice を保持（Update で掃除）
    std::vector<IXAudio2SourceVoice*> activeVoices;
};

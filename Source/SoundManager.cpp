#include "SoundManager.h"

#include <Windows.h>   // HRESULT, CoInitializeEx など
#include <fstream>
#include <cstring>

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "ole32.lib")

SoundManager& SoundManager::Instance() {
    static SoundManager inst;
    return inst;
}

void SoundManager::Initialize() {
    if (!coinit_) {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        coinit_ = SUCCEEDED(hr) || hr == S_FALSE;
    }
    if (!xAudio) {
        if (FAILED(XAudio2Create(&xAudio, 0))) return;
        if (FAILED(xAudio->CreateMasteringVoice(&masterVoice))) return;
    }
}

void SoundManager::StopAll() {
    // XAudio2 未初期化なら配列だけ掃除
    if (!xAudio) { activeVoices.clear(); return; }

    // エンジン停止中はコールバック/内部処理が止まる → 破棄が安全
    xAudio->StopEngine();

    for (auto*& sv : activeVoices) {
        if (!sv) continue;
        sv->Stop(0);                // 再生停止
        sv->FlushSourceBuffers();   // キューを空に
        sv->DestroyVoice();         // 破棄
        sv = nullptr;
    }
    activeVoices.clear();

    xAudio->StartEngine();          // 復帰
}


void SoundManager::Finalize() {
    StopAll();
    if (masterVoice) { masterVoice->DestroyVoice(); masterVoice = nullptr; }
    if (xAudio) { xAudio->Release(); xAudio = nullptr; }
    if (coinit_) { CoUninitialize(); coinit_ = false; }
}

void SoundManager::Update() {
    if (!xAudio) { activeVoices.clear(); return; }
    for (size_t i = 0; i < activeVoices.size(); ) {
        IXAudio2SourceVoice* sv = activeVoices[i];
        XAUDIO2_VOICE_STATE st{};
        sv->GetState(&st, XAUDIO2_VOICE_NOSAMPLESPLAYED);
        if (st.BuffersQueued == 0) {
            sv->DestroyVoice();
            activeVoices.erase(activeVoices.begin() + i);
        }
        else {
            ++i;
        }
    }
}

// --------- WAV ロード（fmt/data をそのまま保持） ---------
void SoundManager::LoadSE(const std::string& key, const std::wstring& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) return;

    struct RIFF { char id[4]; uint32_t size; char wave[4]; } riff{};
    file.read(reinterpret_cast<char*>(&riff), sizeof(riff));
    if (std::memcmp(riff.id, "RIFF", 4) || std::memcmp(riff.wave, "WAVE", 4)) return;

    SoundData sd;
    bool gotFmt = false, gotData = false;

    while (file) {
        struct CHUNK { char id[4]; uint32_t size; } ch{};
        file.read(reinterpret_cast<char*>(&ch), sizeof(ch));
        if (!file) break;

        if (std::memcmp(ch.id, "fmt ", 4) == 0) {
            sd.fmtBytes.resize(ch.size);
            file.read(reinterpret_cast<char*>(sd.fmtBytes.data()), ch.size);
            gotFmt = true;
        }
        else if (std::memcmp(ch.id, "data", 4) == 0) {
            sd.buffer.resize(ch.size);
            file.read(reinterpret_cast<char*>(sd.buffer.data()), ch.size);
            gotData = true;
        }
        else {
            file.seekg(ch.size, std::ios::cur);
        }
    }

    if (!gotFmt || !gotData) return;
    sounds[key] = std::move(sd);
}

void SoundManager::PlaySE(const std::string& key, float volume) {
    if (suspended_) return;
    auto it = sounds.find(key);
    if (it == sounds.end() || !xAudio) return;

    const auto& sd = it->second;
    if (sd.fmtBytes.empty() || sd.buffer.empty()) return;

    const WAVEFORMATEX* wf = reinterpret_cast<const WAVEFORMATEX*>(sd.fmtBytes.data());
    IXAudio2SourceVoice* sv = nullptr;
    if (FAILED(xAudio->CreateSourceVoice(&sv, wf))) return;

    XAUDIO2_BUFFER buf{};
    buf.AudioBytes = static_cast<UINT32>(sd.buffer.size());
    buf.pAudioData = sd.buffer.data();
    buf.Flags = XAUDIO2_END_OF_STREAM;

    sv->SetVolume(volume);
    if (FAILED(sv->SubmitSourceBuffer(&buf))) { sv->DestroyVoice(); return; }
    if (FAILED(sv->Start())) { sv->DestroyVoice(); return; }

    activeVoices.push_back(sv);
    voicesByKey[key].push_back(sv);           // ★ ココ
}

void SoundManager::PlaySELoop(const std::string& key, float volume) {
    if (suspended_) return;
    auto it = sounds.find(key);
    if (it == sounds.end() || !xAudio) return;

    const auto& sd = it->second;
    if (sd.fmtBytes.empty() || sd.buffer.empty()) return;

    const WAVEFORMATEX* wf = reinterpret_cast<const WAVEFORMATEX*>(sd.fmtBytes.data());
    IXAudio2SourceVoice* sv = nullptr;
    if (FAILED(xAudio->CreateSourceVoice(&sv, wf))) return;

    XAUDIO2_BUFFER buf{};
    buf.AudioBytes = static_cast<UINT32>(sd.buffer.size());
    buf.pAudioData = sd.buffer.data();
    buf.LoopBegin = 0;
    buf.LoopLength = 0;
    buf.LoopCount = XAUDIO2_LOOP_INFINITE;
    buf.Flags = XAUDIO2_END_OF_STREAM;

    sv->SetVolume(volume);
    if (FAILED(sv->SubmitSourceBuffer(&buf))) { sv->DestroyVoice(); return; }
    if (FAILED(sv->Start())) { sv->DestroyVoice(); return; }

    activeVoices.push_back(sv);
    voicesByKey[key].push_back(sv);           // ★ ココ
}

void SoundManager::StopByKey(const std::string& key) {
    auto it = voicesByKey.find(key);
    if (it == voicesByKey.end()) return;

    auto& vec = it->second;
    for (auto* sv : vec) {
        if (!sv) continue;
        sv->Stop(0);
        sv->FlushSourceBuffers();
        sv->DestroyVoice();
        // activeVoices 側からも取り除く
        auto ait = std::find(activeVoices.begin(), activeVoices.end(), sv);
        if (ait != activeVoices.end()) activeVoices.erase(ait);
    }
    vec.clear();
    voicesByKey.erase(it);
}

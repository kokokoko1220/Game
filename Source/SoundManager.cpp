#include "SoundManager.h"
#include <windows.h>
#include <mmreg.h>
#include <fstream>
#include <cstring>

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "ole32.lib")

static void DBG(const char* msg) {
    OutputDebugStringA(msg);
    OutputDebugStringA("\n");
}

SoundManager& SoundManager::Instance() {
    static SoundManager inst;
    return inst;
}

void SoundManager::Initialize() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) { DBG("CoInitializeEx failed"); }

    hr = XAudio2Create(&xAudio, 0);
    if (FAILED(hr)) { DBG("XAudio2Create failed"); return; }

    hr = xAudio->CreateMasteringVoice(&masterVoice);
    if (FAILED(hr)) { DBG("CreateMasteringVoice failed"); }
}

void SoundManager::Finalize() {
    if (masterVoice) { masterVoice->DestroyVoice(); masterVoice = nullptr; }
    if (xAudio) { xAudio->Release(); xAudio = nullptr; }
    CoUninitialize();
}

void SoundManager::Update() {
    // 再生が終わった SourceVoice を破棄
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

// ---------------- WAV ロード（拡張WAV対応） ----------------
void SoundManager::LoadSE(const std::string& key, const std::wstring& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) { DBG("WAV open failed"); return; }

    struct RIFF { char id[4]; uint32_t size; char wave[4]; } riff{};
    file.read(reinterpret_cast<char*>(&riff), sizeof(riff));
    if (std::memcmp(riff.id, "RIFF", 4) || std::memcmp(riff.wave, "WAVE", 4)) {
        DBG("not RIFF/WAVE");
        return;
    }

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
            file.seekg(ch.size, std::ios::cur); // スキップ
        }
    }

    if (!gotFmt || !gotData) {
        DBG("fmt or data missing");
        return;
    }

    sounds[key] = std::move(sd);
    DBG(("WAV loaded: " + key).c_str());
}

// ---- 再生コールバック（終端でソースボイス破棄） ----
struct VoiceContext { IXAudio2SourceVoice* voice = nullptr; };

struct VoiceCallback : public IXAudio2VoiceCallback {
    void OnVoiceProcessingPassStart(UINT32) override {}
    void OnVoiceProcessingPassEnd() override {}
    void OnStreamEnd() override {}
    void OnBufferStart(void*) override {}
    void OnBufferEnd(void* pBufferContext) override {
        auto* ctx = reinterpret_cast<VoiceContext*>(pBufferContext);
        if (ctx && ctx->voice) ctx->voice->DestroyVoice();
        delete ctx;
        delete this;
    }
    void OnLoopEnd(void*) override {}
    void OnVoiceError(void*, HRESULT) override {}
};

void SoundManager::PlaySE(const std::string& key, float volume) {
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

    // ★ 配列に保持（終了は Update で破棄）
    activeVoices.push_back(sv);
}


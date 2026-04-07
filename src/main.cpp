#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/OptionsLayer.hpp>

using namespace geode::prelude;

static float applyExponentialCurve(float linear) {
    if (linear <= 0.0f) return 0.0f;
    if (linear >= 1.0f) return 1.0f;

    float exponent = Mod::get()->getSettingValue<double>("exponent");
    return std::pow(linear, exponent);
}

static void replaceSliderCallback(CCNode* layer, const char* id,
    CCObject* target, cocos2d::SEL_MenuHandler cb) {
    if (auto* slider = typeinfo_cast<Slider*>(layer->getChildByIDRecursive(id))) {
        if (auto* thumb = slider->getThumb()) {
            thumb->setTarget(target, cb);
        }
    }
}

static void handleMusicSlider(SliderThumb* thumb) {
    float linear = thumb->getValue();
    float curved = applyExponentialCurve(linear);

    if (Mod::get()->getSettingValue<bool>("logging")) {
        log::info("Music: slider={:.1f}% -> actual={:.1f}%", linear * 100.f, curved * 100.f);
    }

    auto* engine = FMODAudioEngine::sharedEngine();
    GameManager::get()->m_bgVolume = linear;
    engine->setBackgroundMusicVolume(curved);
    engine->m_musicVolume = linear;
}

static void handleSfxSlider(SliderThumb* thumb) {
    float linear = thumb->getValue();
    float curved = applyExponentialCurve(linear);

    if (Mod::get()->getSettingValue<bool>("logging")) {
        log::info("SFX: slider={:.1f}% -> actual={:.1f}%", linear * 100.f, curved * 100.f);
    }

    auto* engine = FMODAudioEngine::sharedEngine();
    GameManager::get()->m_sfxVolume = linear;
    engine->setEffectsVolume(curved);
    engine->m_sfxVolume = linear;
}

class $modify(MyPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        replaceSliderCallback(this, "music-slider",
            this, menu_selector(MyPauseLayer::onMusicSlider));
        replaceSliderCallback(this, "sfx-slider",
            this, menu_selector(MyPauseLayer::onSfxSlider));
    }

    void onMusicSlider(CCObject* sender) {
        handleMusicSlider(static_cast<SliderThumb*>(sender));
    }

    void onSfxSlider(CCObject* sender) {
        handleSfxSlider(static_cast<SliderThumb*>(sender));
    }
};

class $modify(MyOptionsLayer, OptionsLayer) {
    void customSetup() {
        OptionsLayer::customSetup();
        replaceSliderCallback(this, "music-slider",
            this, menu_selector(MyOptionsLayer::onMusicSlider));
        replaceSliderCallback(this, "sfx-slider",
            this, menu_selector(MyOptionsLayer::onSfxSlider));
    }

    void onMusicSlider(CCObject* sender) {
        handleMusicSlider(static_cast<SliderThumb*>(sender));
    }

    void onSfxSlider(CCObject* sender) {
        handleSfxSlider(static_cast<SliderThumb*>(sender));
    }
};

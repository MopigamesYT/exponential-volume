#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/SliderTouchLogic.hpp>

using namespace geode::prelude;

enum class SliderType { None, Music, SFX };
static SliderType s_lastSliderType = SliderType::None;
static float s_lastLinear = 0.f;
static float s_lastCurved = 0.f;

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

    auto* engine = FMODAudioEngine::sharedEngine();
    GameManager::get()->m_bgVolume = linear;
    engine->setBackgroundMusicVolume(curved);
    engine->m_musicVolume = linear;

    s_lastSliderType = SliderType::Music;
    s_lastLinear = linear;
    s_lastCurved = curved;
}

static void handleSfxSlider(SliderThumb* thumb) {
    float linear = thumb->getValue();
    float curved = applyExponentialCurve(linear);

    auto* engine = FMODAudioEngine::sharedEngine();
    GameManager::get()->m_sfxVolume = linear;
    engine->setEffectsVolume(curved);
    engine->m_sfxVolume = linear;

    s_lastSliderType = SliderType::SFX;
    s_lastLinear = linear;
    s_lastCurved = curved;
}

class $modify(SliderTouchLogic) {
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
        SliderTouchLogic::ccTouchEnded(touch, event);

        if (s_lastSliderType != SliderType::None &&
            Mod::get()->getSettingValue<bool>("logging")) {
            const char* name = s_lastSliderType == SliderType::Music ? "Music" : "SFX";
            log::info("{}: slider={:.1f}% -> actual={:.1f}%",
                name, s_lastLinear * 100.f, s_lastCurved * 100.f);
        }
        s_lastSliderType = SliderType::None;
    }
};

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

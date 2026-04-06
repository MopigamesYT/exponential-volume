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

// Recursively find all Slider instances in a node tree
static void findSliders(CCNode* node, std::vector<Slider*>& out) {
    if (!node) return;
    if (auto* slider = typeinfo_cast<Slider*>(node)) {
        out.push_back(slider);
    }
    if (auto* children = node->getChildren()) {
        for (int i = 0; i < children->count(); i++) {
            findSliders(static_cast<CCNode*>(children->objectAtIndex(i)), out);
        }
    }
}

static void replaceSliderCallbacks(CCNode* layer, CCObject* target,
    cocos2d::SEL_MenuHandler musicCb, cocos2d::SEL_MenuHandler sfxCb) {
    std::vector<Slider*> sliders;
    findSliders(layer, sliders);

    int idx = 0;
    for (auto* slider : sliders) {
        auto* thumb = slider->getThumb();
        if (!thumb) continue;
        if (idx == 0) {
            thumb->setTarget(target, musicCb);
        } else if (idx == 1) {
            thumb->setTarget(target, sfxCb);
        }
        idx++;
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

// Log on slider release
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
        replaceSliderCallbacks(
            this, this,
            menu_selector(MyPauseLayer::onMusicSlider),
            menu_selector(MyPauseLayer::onSfxSlider)
        );
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
        replaceSliderCallbacks(
            this, this,
            menu_selector(MyOptionsLayer::onMusicSlider),
            menu_selector(MyOptionsLayer::onSfxSlider)
        );
    }

    void onMusicSlider(CCObject* sender) {
        handleMusicSlider(static_cast<SliderThumb*>(sender));
    }

    void onSfxSlider(CCObject* sender) {
        handleSfxSlider(static_cast<SliderThumb*>(sender));
    }
};

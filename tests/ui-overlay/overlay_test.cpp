#include <RmlUi/Core.h>
#include <cstdlib>
#include <iostream>

#include "../../src/dusk/ui/overlay_attributes.hpp"

static void check(bool result, const char* message) {
    if (!result) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

class ObservedElement : public Rml::Element {
public:
    ObservedElement() : Element("fps") {}
    int attributeChanges = 0;
    int propertyChanges = 0;
    void updateProperties() { UpdateProperties(1.f, {1920.f, 1080.f}); }

protected:
    void OnAttributeChange(const Rml::ElementAttributes& attrs) override {
        ++attributeChanges;
        Element::OnAttributeChange(attrs);
    }
    void OnPropertyChange(const Rml::PropertyIdSet& properties) override {
        ++propertyChanges;
        Element::OnPropertyChange(properties);
    }
};

int main() {
    using namespace dusk::ui::detail;
    Rml::FontEngineInterface fonts;
    Rml::SetFontEngineInterface(&fonts);
    check(Rml::Initialise(), "RmlUi initialization failed");
    {
        ObservedElement fps;
        set_overlay_attribute(fps, "open", "");
        set_overlay_attribute(fps, "corner", "bottom-left");
        set_overlay_bottom(fps, 12.f);
        fps.updateProperties();
        const int attrs = fps.attributeChanges, properties = fps.propertyChanges;
        for (int i = 0; i < 1000; ++i) {
            set_overlay_attribute(fps, "open", "");
            set_overlay_attribute(fps, "corner", "bottom-left");
            set_overlay_bottom(fps, 12.f);
            fps.updateProperties();
        }
        check(fps.attributeChanges == attrs, "Unchanged HUD attributes invoked callbacks");
        check(fps.propertyChanges == properties, "Unchanged bottom style invalidated properties");
        set_overlay_attribute(fps, "corner", "top-right");
        set_overlay_bottom(fps, std::nullopt);
        check(fps.GetAttribute<Rml::String>("corner", "") == "top-right", "Corner change lost");
        check(!fps.GetLocalProperty(Rml::PropertyId::Bottom),
            "Bottom override remained after corner change");
        set_overlay_bottom(fps, 45.f);
        check(fps.GetLocalProperty(Rml::PropertyId::Bottom)->Get<float>() == 45.f,
            "Pipeline avoidance height lost");
        fps.RemoveAttribute("open");
        set_overlay_attribute(fps, "open", "");
        check(fps.HasAttribute("open"), "Reopening HUD failed");

        ObservedElement progress;
        set_overlay_attribute(progress, "value", 0.25f);
        const int first = progress.attributeChanges;
        set_overlay_attribute(progress, "value", 0.25f);
        check(progress.attributeChanges == first, "Unchanged progress invalidated HUD");
        set_overlay_attribute(progress, "value", 0.5f);
        check(progress.GetAttribute<float>("value", 0.f) == 0.5f, "Progress update lost");
    }
    Rml::Shutdown();
    std::cout << "HUD mutation guards passed\n";
}

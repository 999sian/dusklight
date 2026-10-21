#pragma once

#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/Property.h>
#include <optional>

namespace dusk::ui::detail {

template <typename T>
void set_overlay_attribute(Rml::Element& element, const Rml::String& name, const T& value) {
    const auto* current = element.GetAttribute(name);
    if (current == nullptr || *current != Rml::Variant(value)) {
        element.SetAttribute(name, value);
    }
}

inline void set_overlay_bottom(Rml::Element& element, std::optional<float> bottom) {
    if (!bottom) {
        element.RemoveProperty(Rml::PropertyId::Bottom);
        return;
    }
    const Rml::Property value(*bottom, Rml::Unit::PX);
    const auto* current = element.GetLocalProperty(Rml::PropertyId::Bottom);
    if (current == nullptr || *current != value) {
        element.SetProperty(Rml::PropertyId::Bottom, value);
    }
}

}  // namespace dusk::ui::detail

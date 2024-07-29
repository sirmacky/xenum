A set of advanced enum generator macros for c++20 to make enum generation and management more consistent and powerful by generating XEnumTraits alongside the enum declaration

Example usage
```
ImplementXEnum(ExampleEnum,
    XValue(a),               // 0
    XValue(b, 3),            // 3
    XValue(d),               // 4
    XAlias(e, d),            // 4
    XFlag(f, 3),             // 1 << 3
    XFlag(g, 4),             // 1 << 4
    XFlag(h, 5),             // 1 << 5
    XValue(x),               // 33
    XFlagState(y, f | g),    // 24
);
```

Generates an enum class and type traits override that satisfies the following criteria
```
struct XEnumTraits<ExampleEnum>
{
    static constexpr bool is_valid = true;
    static constexpr const char* const name;
    static constexpr size_t Count;
    static constexpr std::array<MyEnum, Count> Values;
    static constexpr const char* CStrValues[Count];
    static constexpr std::string ToString(EnumType value, const std::string& fallback = "<unknown>");
    static constexpr const char* ToCString(EnumType value, const char* fallback = "<unknown>");
    static constexpr int IndexOf(EnumType value);
};
```

This can be used to generate convenience functions like logging, or imgui combo boxes
```
    template <XEnumValue T>
    bool Combo(const char* const label, T& value, ImGuiComboFlags flags = 0)
    {
        auto origValue = value;

        if (ImGui::BeginCombo(label, XEnumTraits<T>::ToCString(value), flags))
        {
            for (const auto& option : XEnumTraits<T>::Values)
            {
                bool is_selected = option == value;
                if (ImGui::Selectable(XEnumTraits<T>::ToCString(option), is_selected))
                    value = option;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        return origValue != value;
    }

    template <typename T> requires XEnumTraits<T>::is_valid
    bool Combo(T& value, ImGuiComboFlags flags = 0)
    {
        return Combo(XEnumTraits<T>::name, value, flags);
    }
```
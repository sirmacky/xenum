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

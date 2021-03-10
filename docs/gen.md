# C++ code generation

ACE provides a Model-to-C++ compiler, called `ace-compile`, to enable tools to
interact with configuration files. It also provides helper tools to instantiate
the generated class based on configuration instances. If a configuration parsing
class has been successfully instantiated, the tool developer has the guarantee
that all accessed configuration option values are valid according to the defined
model.

## File organization

`ace-compile` generates three types of files: model class interfaces
`<ModelName>Instance.ac.h`; model class declarations `<ModelName>.ac.h`; and
model class definitions `<ModelName>.ac.cpp`. Model class interfaces define pure
virtual interfaces that contain the accessor prototypes of a given model. Only
interfaces are used within the definition of `class` type accessor prototypes.

## Checkers and getters

For each option, the compiler generates two accessors:

* a checker: `bool has_<OptionName>() const` if the arity of the option is either `?` or `*`
* a getter: `OptionType <OptionName>() const`

The value of `OptionType` depends on the arity and the type of the option:

* `?` or `1`: `OptionType` is either `BaseType` or `BaseType const &`
* `*` or `+`: `OptionType` is `std:vector<BaseType> const` or `std::vector<BaseType::Ref> const &`

The value of `BaseType` depends on the kind of type involved. It may be either
`bool`, `int`, `float`, `std::string`, or `<ModelName>Interface`. `BaseType
const &` is used for `std::string` and `<ModelOption>Interface` types.
`std::vector<BaseType::Ref> const &` is used for `<ModelOption>Interface`.

## Handling inheritance

The inclusion of external models is handled using multiple inheritance of pure
virtual interfaces. The final implementation of the derived models instantiates
itself all the configuration values. As a consequence, the declaration and
definition files of models that only serve as base model in a model definition
are not generated. When to included models define the same `class` option, the
option is merged in the final model if and only if either the `class` models are
identical or if one is a direct descendent of the other. In the later case, the
compiler leverages C++ co-variant return types to implement the option
accessors.

## The case of the Plugin type

The code generation of the `plugin` kind of type is handle a bit differently
that the other types. The `plugin` C++ type generated is a map from
`std::string` to `MODEL::Ref` objects, containing the instantiated plugins.
`MODEL::Ref` is a reference to the instantiated object casted into its base
type. It's up to the user to up-cast that reference to the expected type.
Example:

```json
"options": {
  "kind": "plugin", "arity": "1",
  "model": "Base.json"
}
```

The generated accessor:

```cpp
std::map<std::string, Base::Ref> const & options() const;
```

For two triggered models ModelA and ModelB, the generic way to access the components is:

```cpp
IModelA const & modelA = std::static_pointer_cast<IModelA const &>(options().at("a"));
IModelB const & modelB = std::static_pointer_cast<IModelB const &>(options().at("b"));
```

To avoid verbose casting, a helper templated function is also generated to
access generic components. The helper's name is the name of the options suffixed
with `_at`:

```json
IModelA const & modelA = options_at<IModelA>("a");
IModelB const & modelB = options_at<IModelB>("b");
```

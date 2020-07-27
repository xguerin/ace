# Types

Types are JSON objects that define supported data types in configuration instances. A valid type require a bunch of attributes to be set. Five basic attributes are common to all types:

* `kind` : the kind of type to define, one of `boolean`, `integer`, `float`, `string`, `uri`, `class`, `select`
* `arity` : the arity of the option, one of `?`, `1`, `*`, `+`, `M:N`
* `doc` : a one-liner explaining the meaning of the option
* `deprecated` : mark the option as deprecated and provide a one-liner to explain how to replace it
* `default`: the default value to use if the option is omitted in the instance
* `inherit`: whether the option value can be inherited from the same option present at an upper level of the instance
* `hook`: whether the option value are based on another option's values

`kind`, and `doc` are mandatory attribute. `default`, `inherit`, and `hook` may or may not be specified by the author. `default` takes a value of the same kind as the type. `inherit` is either `true` or `false`, being `false` by default. The `arity` and `hook` attributes are mutually exclusive.

## Arity

ACE types can have either one of four kinds or arities, specified in the `arity` attribute:

* `?`, meaning either one or no value (equivalent to optional);
* `1`, meaning one and only one value (equivalent to required);
* `*`, meaning any number of values (equivalent to 0:N); and
* `+`, meaning at least one value (equivalent to 1:N).
* `M:N`, with either bound optional.

Marking a type as optional means that the type is **not required** by the application to operate properly. By extension, it means that no value at all for that type will be used by the application. Therefore, specifying a `default` field to an optional type is meaningless **except** if a dependency susceptible to promote the type to required exists in the model.

## Ranged and enumerated

Ranged and enumerated types are types that support the `range` attribute and the `either` (enumeration) attribute.

### The range attribute

The `range` attribute specifies a range within which the value must be defined:

```json
"closed": "[ X, Y ]",
"open": "( X, Y )",
"open-left": "( X, Y ]",
"open-right": "[ X, Y )"
```

### The either attribute

The `either` attribute specifies a list in which the value must be defined:

```json
"either": [ V0, V1, V2, V3 ]
```

## Value hook

A `hook` is a string composed of three parts separated by `:`: `"SOURCE:MATCH:EXPANSION"`. `SOURCE` is the path of the option to be used as a value source. `MATCH` is a regular expression that define sub-pattern matches that will be later expanded into the `EXPANSION` field. The match/expansion syntax is similar to this of `sed`.

Example:

```json
"option1": {
  "kind": "string", "arity": "1",
   "doc": "an option", "default": "hello_world"
} // -> hello_world

"option2": {
  "kind": "string", "hook": "@.option1:([a-zA-Z]+)_([a-zA-Z]+):\\1, \\2 !",
  "doc": "a second option"
 } // -> hello, world !
```

## Kinds

| Type      | Range | Enumeration |  Specifics                                                 |
|-----------|-------|-------------|------------------------------------------------------------|
| `boolean` | No    | Yes         |                                                            |
| `cpuid`   | Yes   | Yes         |  Match value with # of active cpus available on the system |
| `class`   | No    | No          |                                                            |
| `enum`    | No    | Yes         |                                                            |
| `file`    | No    | Yes         |  block, char, dir, fifo, regular                           |
| `float`   | Yes   | Yes         |  double                                                    |
| `ipv4`    | No    | Yes         |  XXX.XXX.XXX.XXX, XXX as integer                           |
| `integer` | Yes   | Yes         |  int64_t                                                   |
| `mac`     | No    | Yes         |  MAC address XX.XX.XX.XX.XX.XX, XX as hexadecimal          |
| `plugin`  | No    | No          |                                                            |
| `select`  | No    | No          |                                                            |
| `string`  | No    | Yes         |                                                            |
| `uri`     | No    | Yes         |  ipv4, file, http, ftp, smbcast                            |

### Boolean

The `boolean` kind of type describes a boolean type. Accepted values for that type are `true` and `false`.

```json
"option": {
  "kind": "boolean", "arity": "1",
  "doc": "a boolean option"
}
```

### Integer

The `integer` kind of type describes a 64-bit signed integer type.

```json
"option": {
  "kind": "integer", "arity": "1",
  "doc": "an integer option"
}
```

### Float

The `float` kind of type describes a 64-bit signed float type.

```json
"option": {
  "kind": "float", "arity": "1",
  "doc": "a float option"
}
```

It also support positive infinity `+inf` and negative infinity `-inf` values.

### CPUID

The `cpuid` kind of type describes a CPU identifier type. When used, extra checks are performed to ensure that the CPUID refers to a valid CPU number. Supported values are any integer ranging from -1 (any CPU) to `SYSTEM_MAX_CPU` - 1. Like integers, this option accept `range`, `either`, and `default`:

```json
"option": {
  "kind": "cpuid", "arity": "1",
  "default": -1, "doc": "a CPUID option",
  "either": [ -1, 0, 1 ]
}
```

### File

The `file` kind of type describes a filesystem file. When used, extra checks are performed to ensure that the file is corresponds to an allowed  list of sub-types and can be accessed using specified mode. It requires one extra attribute: `mode`. The `mode` attribute is a string that contains the access mode to use. The accepted values are equivalent to these of `fopen`.

```json
"option": {
  "kind": "file", "arity": "1",
  "doc": "a file option",
  "either": [ "fifo", "regular" ],
  "mode": "r+"
}
```

The sub-types of file recognized are: `block`, `character`, `directory`, `fifo`, and `regular`.

### String

The `string` kind of type describes a string type. It supports one extra attribute: `either`. The `either` attribute is a N-value array that defines the set of acceptable values for that type:

```json
"option": {
  "kind": "string", "arity": "1",
  "doc": "a string option", "either": [ "hello", "world" ]
}
```

### MAC

The `mac` kind of type describes a MAC address type. It supports one extra attribute: `either`. The `either` attribute is a N-value array that defines the set of acceptable values for that type:

```json
"option": {
  "kind": "mac", "arity": "1",
  "doc": "a MAC option",
  "either": [ "00:11:22:33:44:55", "66:77:88:99:AA:BB" ]
}
```

### IPv4

The `ipv4` kind of type describes a IPv4 address type. It supports one extra attribute: `either`. The `either` attribute is a N-value array that defines the set of acceptable values for that type:

```json
"option": {
  "kind": "ipv4", "arity": "1",
  "doc": "an IPv4 option",
  "either": [ "192.168.1.1", "10.0.1.1" ]

```

### URI

The `uri` kind of type describes a URI type. When used, extra checks are performed to ensure that the URI is valid. Supported schemes are `file` and `http`. It supports one extra attribute: `either`. The `either` attribute is a N-value array that defines the set of acceptable schemes:

```json
"option": {
  "kind": "uri", "arity": "1",
  "doc": "an URI option", "either": [ "file" ]
}
```

### Enum

The `enum` kind of type describes an integer-based enumeration. It supports one extra attribute: `bind`. The `bind` attribute is a N-value map that defines the set of acceptable values:

```json
"option": {
  "kind": "enum", "arity": "1",
  "bind": {
    "ENUM_VAL_A": 0,
    "ENUM_VAL_B": 1,
    "ENUM_VAL_C": 2,
    "ENUM_VAL_D": 3,
  },
  "doc": "an enum option"
}
```

The generated code looks like this:

```cpp
typedef enum _OptionType {
  ENUM_VAL_A = 0,
  ENUM_VAL_B = 1,
  ENUM_VAL_C = 2,
  ENUM_VAL_D = 3,
} OptionType;
```

### Class

The `class` kind of type describes an option in the model that is defined by an external model. Options of that type are usually implemented as sub-sections in the configuration instance. It requires one extra attribute: `model`. The `model` attribute is a string that contains the path and name of the model to use:

```json
"option": {
  "kind": "class", "arity": "1",
  "doc": "a class option", "model": "my_path/my_model.json"
}
```

### Select

The `select` kind of type describes a selector of options that are dynamically generated using values from the configuration instance. The values of that option are used as name for options which types are taken from the `templates` section of the model. It requires two extra attributes: `template` and `size`. The `template` attribute is a string that contains the name of the template to use for the option. The `size` attribute is an arity attribute that defines how many entries in the selector are expected:

```json
"strings": {
  "kind": "select", "arity": "?", "size": "+",
  "doc": "a select option", "template": "my_string_template"
}
```

The example above would dynamically generate options with type `my_string_template` and names defined as values in the `select` option. See the instance example below:

```json
"strings": {
  "A_string": "hello",
  "B_string": "world"
}
```

It supports one extra attribute: `either`. The `either` attribute is a N-value array that defines the valid values for that type.

```json
"strings": {
  "kind": "select", "arity": "?",
   "doc": "a select option",
   "either": [ "foo", "bar" ],
   "template": "my_string_template"
}
```

The example above defines a `select` option that generate new options of type `my_string_template` and that allow only the values `hello` and `world`:

```json
"strings": {
  "foo": "hello",
  "bar": "world"
}
```

### Plugin

The `plugin` kind type describes an selector objects that are dynamically generated using values from the configuration instance. These objects must inherit from the same base model.  Plugins are dynamically loaded at run time and options are automatically computed using the `trigger` element of the model header, and the option are made available if and only if the models in the mapping are found in the model path. It requires two extra attributes: `model`, and `target-arity`. The `model` attribute is a string that contains the path and name of the model to use as a base model for the plugins.  By default, the expected arity of the target class is `1`. The attribute `target-arity` can be used to alter that default arity to `+`:

```json
"plugins": {
  "kind": "plugin", "arity": "?", "doc": "a plugin option",
  "model": "my/base/Model.json",
  "target-arity": "+"
}
```

The new arity will be applied to all elements of the plugin.

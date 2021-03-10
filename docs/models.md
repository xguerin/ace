# Models

## Top-level structure

Models are JSON objects that contain three entries: `header`, `body` and
`templates`. The `header` entry contains information about the model, such as
its author's name and e-mail, the its version string, and a line of
documentation explaining the model's purpose. The `body` entry contains the
definitions of the options expected by the model. The `templates` entry contains
type definitions that can be injected into the model based on instance values.
This injection is supported for options of type `select`.

```json
"header": {
  "package": [ "my", "package" ],
  "author": { "name": "John Doe", "email": "jdoe@acme.com" },
  "version": "1.0",
  "namespace": [ "my", "ns" ],
  "doc": "ACME configuration model"
}
```

The `package` and `namespace` entries are related to C++ code generation. The
`package` entry defines the compilation unit path of the configuration model.
The compilation subsystem must provide an include path the can resolve the
package path. The `namespace` entry defines the namespace to use for the
generated classes.

## Inheritance

A model can extend external pre-existing models. When extending models, the
options defined in these models are imported and merged in the newly defined
models. Extended models can in turns extend models themselves. To extends from
existing models, a model need to declare path and name of extended models in its
headers using the `include` clause as follows:

```json
"header": {
  "name": "ACME_EX",
  "author": { "name": "John Doe", "email": "jdoe@acme.com" },
  "version": "1.0",
  "include": [ "a_path/a_model.json", ... ],
  "doc": "ACME_EX configuration model"
}
```

Multiple extended models may be defining options that bear the same name. In
that case, ACE checks if the option types are compatible by kind, arity, and
dependencies. If the types are compatible, the options are merged into one. If
the types conflict ACE warns the user of the conflicts. Thanks to the
flexibility of C++, ACE can merge options of type `class` if either the declared
model are identical, or if one of the model is a direct descendent of the other.

## Paths

ACE uses the [JSONPath](http://goessner.net/articles/JsonPath) notation to represent paths.

## Triggers

Models that are supposed to work in conjunction with plugin types can declare
"trigger" patterns that will be used to load them depending on the option path
in the instance.

```json
"header": {
  "name": "ACME_EX",
  "author": { "name": "John Doe", "email": "jdoe@acme.com" },
  "version": "1.0",
  "include": [ "plugin/base/model.json", ... ],
  "trigger": [ "$.*.acme", ... ],
  "doc": "ACME_EX configuration model"
}
```

In the above example, the defined trigger allow that model to be loaded when the
path match `$.*.acme` and the plugin use `plugin/base/model.json` as a base
model. The path matched by the trigger is the full JSON path of the option
instance.

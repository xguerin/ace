#### About

ACE is a model-based configuration authoring and validation environment. It's aim is to bring determinism to tool configuration by externalizing the configuration definition into a separate model file. The model file can be seen as the expected layout of the configuration and declares valid options along with their types, arities, and documentations. It is used to externally check the validity of a configuration instance, browse legit options, and generate the necessary source code to access the options values.

#### How it works

ACE requires tool programmers to define a model for their configuration. Models contain declarations of valid options along with their types, arities, and documentation. Models are self-describing, meaning that a full configuration parser and full documentation can be generated form a single model definition. Models can inherit from others and option merging is supported depending on the option type. The language chosen to define models is JSON.

#### Supported configuration file formats

ACE currently support the JSON file format as well as Python and Lua scripts:

  * JSON: the root type must be an object
  * Python: the script must declare a dictionary named `config` in its global namespace
  * Lua: the script must declare a table named `config` is its global namespace

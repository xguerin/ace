# About

ACE is a model-based configuration authoring and validation environment. It aims
at bringing determinism to tool configuration by externalizing the configuration
definition into a separate model file. The model file is used to check the
validity of a configuration instance, browse available options, and generate the
necessary source code to access the options values.

## Installation

### Prepare

Grab [the source code](https://github.com/xguerin/ace). ACE has several
dependencies, listed below.

* [X] [lemon](http://www.hwaci.com/sw/lemon)
* [X] [ragel](http://www.colm.net/open-source/ragel)
* [X] [TCLAP](http://tclap.sourceforge.net/)
* [X] [RE2](https://github.com/google/re2)
* [X] [GTest](https://github.com/google/googletest) if testing is enabled
* [ ] [hjson](https://hjson.github.io) for HJSON support
* [X] [jansson](http://www.digip.org/jansson) for JSON support
* [ ] Python 3.x for Python support
* [ ] Lua 5.2.4 for Lua support
* [X] [yaml-cpp](https://github.com/jbeder/yaml-cpp) for YAML support

### Building

ACE uses [CMake](https://cmake.org) as its build system. To build the project,
simply run the following commands:

```bash
cd ace
mkdir build
cd build
cmake ..
make
make install
```

### Tweaking the build process

The `cmake` command accepts the following variables (default is shown **bold**):

* `ACE_PLUGIN_HJSON`: enables the HJSON file plugin. Accepts **`ON`** or `OFF`.
* `ACE_PLUGIN_JSON`: enables the JSON file plugin. Accepts **`ON`** or `OFF`.
* `ACE_PLUGIN_LUA`: enables the LUA file plugin. Accepts **`ON`** or `OFF`.
* `ACE_PLUGIN_PYTHON`: enables the PYTHON file plugin. Accepts **`ON`** or `OFF`.
* `ACE_PLUGIN_TOML`: enables the TOML file plugin. Accepts **`ON`** or `OFF`.
* `ACE_PLUGIN_YAML`: enables the YAML file plugin. Accepts **`ON`** or `OFF`.
* `ACE_PLUGIN_TESTS`: builds the test suite. Accepts `ON` or **`OFF`**.

The standard `cmake` variable `CMAKE_INSTALL_PREFIX` shall be used to alter the
installation prefix:

```bash
cd ace
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/opt ..
make
make install
```

## Getting started

### Model definition

The first step is to define a model for our example configuration, saved into a
`Model.json` file:

```json
{
  "header": {
    "author": { "name": "John Doe", "email": "jdoe@acme.com" },
    "version": "1.0",
    "namespace": [ "my", "ns" ],
    "doc": "ACME configuration model"
  },
  "body": {
    "intItem": {
      "kind": "integer", "arity": "?",
      "doc": "an integer option"
    },
    "stringItem": {
      "kind": "string", "arity": "1",
      "doc": "a string option"
    },
    "floatItem": {
      "kind": "float", "arity": "*",
      "doc": "a float option"
    },
    "booleanItem": {
      "kind": "boolean", "arity": "+",
      "doc": "a boolean option"
    }
  }
}
```

This model defines 4 configuration items: a integer item `intItem`, a string
item `stringItem`, a float item `floatItem`, and a boolean item `booleanItem`.
Beside its kind, each item declares its arity and documnetation.

### Model transpilation

The second step is to transpile that model into a set of helper classes that can
be used in a C++ application.

```
ace-compile Model.json
```

The `ace-compile` command produces 3 files: an interface header file
`IModel.ac.h`, an implementation header file `Model.ac.h`, and an implementation
source file `Model.ac.cpp`.

### Code integration

The third step is to make use of the generated files in our example application.
ACE provides a set of helper functions that simplify that task:

```cpp
#include <Model.ac.h>
#include <ace/engine/Master.h>
#include <ace/model/Helper.h>
#include <tclap/CmdLine.h>
#include <string>

using namespace ace::model::Helper;

int main(int argc, char *argv[]) {
  // Define CLI arguments

  TCLAP::CmdLine cmd("ACME Tool", ' ', "0.1");
  TCLAP::ValueArg<std::string> cfgPath("c", "config", "Configuration file",
                                       true, "", "string");
  cmd.add(cfgPath);
  cmd.parse(argc, argv);

  // Instantiate the configuration
  //
  auto cfg = parseFile<my::ns::Model>(cfgPath.getValue(), false, argc, argv);
  if (cfg == nullptr) return -1;

  // Use it !
  //
  auto const & acme = *cfg;
  if (acme.has_intItem()) {
    std::cout << "int = " << acme.intItem() << std::endl;
  }
  for (auto const & f : acme.floatItem()) {
    std::cout << "float = " << f << std::endl;
  }
  for (auto const & b : acme.booleanItem()) {
    std::cout << "bool = " << b << std::endl;
  }
  std::cout << "str = " << acme.stringItem() << std::endl;

  return 0;
}
```

Please note that the code above makes use of `TCLAP` to parse the command line
arguments. This is an arbitrary choice and you are free to use whichever CLI
option parser you wish.

### Testing

Finally, we are ready to test our application. First, let's compile it using the
following Makefile:

```makefile
CFLAGS = -g3 -x c++ -std=c++11 -I . -I $(ACE_INCLUDE_PATH) 
LFLAGS = -L $(ACE_LIB_PATH) -rpath $(ACE_LIB_PATH) 

all: clean ace bin

ace:
	ace-compile -I . Model.json

bin:
	c++ $(CFLAGS) -c main.cpp
	c++ $(CFLAGS) -c Model.ac.cpp
	c++ $(LFLAGS) -o main main.o Model.ac.o -lace

clean:
	rm -f *.ac.* *.o main
```

Next, let's define a configuration file, in TOML, called `config.toml`:

```toml
intItem = 17
stringItem = "Hello!"
floatItem = 3.14
booleanItem = [ true, false, false ]
```

Finally, let's run our application:

```
./main -c config.toml
```

The expected output it:

```
int = 17
float = 3.14
bool = 1
bool = 0
bool = 0
str = Hello!
```

Since ACE also supports JSON, Python and Lua, we can also use these
configuration files:

```json
{
  "intItem" : 17,
  "stringItem" : "Hello!",
  "floatItem" : 3.14,
  "booleanItem" : [ true, false, false ],
  "selectItem" : "hello",
  "hello" : "world"
}
```

a Python configuration file:

```python
config = {
  'intItem' : 17,
  'stringItem' : 'Hello!',
  'floatItem' : 3.14,
  'booleanItem' : [ True, False, False ],
  'selectItem': 'hello',
  'hello': 'world'
}
```

or a Lua configuration file:

```lua
config = {
  intItem = 17,
  stringItem = 'Hello!',
  floatItem = 3.14,
  booleanItem = { true, false, false }
  selectItem = 'hello',
  hello = 'world'
}
```

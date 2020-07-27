# Miscellaneous

## Alterations

It is possible to use the command line arguments to alter the configuration. To use this feature, simple use the `parseFile` or `parseString` helper API, passing as a third argument a vector of string with the following format: `X.Y.Z=VALUE`.

### Example

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
                                       true, "", "string", cmd);
  TCLAP::MultiArg<std::string> defPath("D", "define", "A=B statements", false,
                                       "string", cmd);
  cmd.parse(argc, argv);

  // Instantiate the configuration
  //
  auto cfg = parseFile<my::ns::Model>(cfgPath.getValue(), defPath.getValue(),
                                      false, argc, argv);
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

### Usage

```bash
./acme -c config_file.ini -D stringItem= -D stringItem="hello, world"
```

### Syntax

The `parseFile` or `parseString` helper expect a vector of strings with the following format: `X.Y.Z=VALUE`. `X.Y.Z` represents a path in the configuration tree. `VALUE` is the value that needs to be assigned to that path. The format used for the value is the same as the one use in the INI format. Altering the tree works as follows:

* Specifying a empty value for a path erase that value in the existing tree: `-D X.Y.Z=`
* Specifying one value for a path sets that value in the existing tree: `-D X.Y.Z=VALUE`
* Setting multiple values for a path appends these values to that path: `-D X.Y.Z=VALUE1 -D X.Y.Z=VALUE2 -D X.Y.Z=VALUE3`

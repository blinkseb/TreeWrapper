# TreeWrapper

TreeWrapper is a small, lightweight and easy to use wrapper around [ROOT TTree](https://root.cern.ch/root/html/TTree.html). Its clear syntax allows to create and read TTree very easily.
### Version
0.1

### Installation

Download the latest version from [github project releases](https://github.com/blinkseb/TreeWrapper/releases), and extract the folder where you want

#### Within CMSSW

Create a standard package hierachy in the `src` directory of your CMSSW release, something like `src/Utilities/TreeWrapper` and extract the source code in this directory.

Build with `scram`

```sh
$ scram b -j4
```
#### Standalone

The library can also be used in standalone. A `configure` script is provided.

```sh
$ ./configure --prefix=INSTALL_LOCATION
$ make
$ make install
```

### Usage

Usage is very simple. Include the header `TreeWrapper.h` in your source file.

#### Read mode

Below is an example of how to read a TTree with the wrapper

```C++
#include <TreeWrapper.h>

[...]

TTree* tree_ = file->Get("tree");

// Create a new TreeWrapper. The constructor has one parameter, a pointer to a TTree to wrap
TreeWrapper tree(tree_);

// Read branch "triggers" from tree, which is stored as a vector of string
// Very important: the read method returns a const reference!
const std::vector<std::string>& triggers = tree["triggers"].read<std::vector<std::string>>();

// Iterates over the whole tree
while (tree.next()) {
    std::cout << "triggers size = " << triggers.size() << std::endl;
}
```

#### Write mode

Below is an example of how to write a TTree with the wrapper

```C++
#include <TreeWrapper.h>

[...]

TTree* tree_ = new TTree("tree");

// Create a new TreeWrapper. The constructor has one parameter, a pointer to a TTree to wrap
TreeWrapper tree(tree_);

// Write branch "triggers" to the tree, which is stored as a vector of string
// Very important: the write method returns a reference!
std::vector<std::string>& triggers = tree["triggers"].write<std::vector<std::string>>();

// Iterates over the events or whatever
for (i = O; i < events; i++) {
    // Do some stuff
    triggers.clear();
    
    triggers.push_back("first trigger");
    triggers.push_back("second trigger");
    
    // Fill the tree
    tree.fill();
}
```

### Documentation

The main class is `TreeWrapper`, located in the header `interface/TreeWrapper.h`. It works like a map, where each key is string representing a branch from the tree. You can access each branch with the `[]` operator. To create or read a branch named `branch` in the tree, just do:

```C++
TreeWrapper tree(tree_);
Leaf& leaf = tree["branch"];
```

Each branch is represented by the `Leaf` class, located in the header `interface/Leaf.h`. Each leaf has two functions, `read` and `write`. The method signature is the same for both methods:

```C++
template <typename T> const T& read();
template <typename T> T& write();
```

Each method returns a (const) reference of type `T`. **It's very important to keep the reference around if you want the library to work**. Do not do:

```C++
const float branchValue = tree["branch"].read<float>();
```

but do (notice the usage of the reference!):
```C++
const float& branchValue = tree["branch"].read<float>();
```

#### Automatic reset

A very nice feature of the wrapper is the automatic reset of the branches after a `fill()`. You don't need to reset your variables at the beginning of each event, it's done for you automatically. For the moment, `reset` means assigning `0` to the branch variable, or calling `clear()` if the branch variable is a `std::vector`. If you need a default behaviour, you have to write your own `resetter`: you must specialized the provided `ResetterT` struct with your own type. Suppose you use a `std::string` for one branch. You'll need to declare:

```C++

template <>
struct ResetterT<std::string>: Resetter {
    public:
        ResetterT(std::string& data)
            : m_data(data) {
            }

        virtual void reset() {
            m_data = "";
        }

    private:
        T& m_data;
};

```

License
----

MIT

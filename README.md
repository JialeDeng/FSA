# FSA
Fuzzing with optimized grammar-aware mutation strategies. FSA is short for *fuzzing with subtree-pool-based random mutation and AST-node-level power schedule*. It is built mostly based on the famous **AFL** (http://lcamtuf.coredump.cx/afl/) and **Superion** (https://github.com/zhunki/Superion).

The parsing ability of FSA is provided by **ANTLR** (https://www.antlr.org/), which can automatically generate a parser given the grammar file. 


## Building

#### 0. Pre Install

- Debian 8.3.0 (Ubuntu also works)
- cmake 3.16.0
- clang 7.0.1

#### 1. Build ANTLR runtime

To build FSA, we first need to build the ANTLR runtime. The ANTLR runtime is located in tree_mutation folder.

```
cd /path_to_FSA/tree_mutation/
cmake ./
make
```

If a missing uuid error is raised, you need to install uuid first.

```
sudo apt-get install uuid-dev
```

#### 2. Generate Lexer, Parser, Visitor automatically (Optional)

The project already included xml and javascript parsers generated as follows. If you need to support other input grammar, you caan refer to the following instructions.

add antlr-4.7-complete.jar to classpath

xml:

```
java -jar antlr-4.7-complete.jar -o path -visitor -no-listener -Dlanguage=Cpp XMLLexer.g4 
java -jar antlr-4.7-complete.jar -o path -visitor -no-listener -Dlanguage=Cpp XMLParser.g4
```

javascript:

```
java -jar antlr-4.7-complete.jar -o path -visitor -no-listener -Dlanguage=Cpp ECMAScript.g4 
```


#### 3. Build tree mutator

The XML parser is located in tree_mutation/xml_parser folder.

```
cd /path_to_FSA/tree_mutation/xml_parser

for f in *.cpp; do g++ -I ../runtime/src/ -c $f -fPIC -std=c++11; done

g++ -shared -std=c++11 *.o ../dist/libantlr4-runtime.a  -o libTreeMutation.so
```

#### 4. Build AFL

Compile afl-clang-fast and afl-clang-fast++, which are used to instrument fuzzing targets.

```
# install llvm+clang
PREFIX=/path/to/install/llvm ./install_llvm.sh

# install FSA
cd /path_to_FSA/
make
```

## Fuzzing
#### 0. Environment
```
CC=/path/to/FSA/afl-clang-fast
CXX=/path/to/FSA/afl-clang-fast++
```

#### 1. libxml 2
```
# build libxml 2
git clone https://github.com/GNOME/libxml2.git

```

#### 2. Sablotron
```

```

#### 3. JerryScript

```
# build JerryScript
python ./tools/build.py --clean --debug --compile-flag=-fsanitize=address --compile-flag=-m32 --compile-flag=-fno-omit-frame-pointer --compile-flag=-fno-common --jerry-libc=off --static-link=off --lto=off --error-message=on --system-allocator=on

# start fuzzing
./afl-fuzz -i ~/jerry_seeds/ -o ~/jerry_out/ ~/jerryscript/build/bin/jerry @@
```

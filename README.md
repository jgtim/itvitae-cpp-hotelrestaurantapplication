# molveno-resort-systeem
A system to improve the quality of life of Molveno Resort employees.

## Run tests locally

### Windows (after having used the official Qt installer)

```powershell
cmake -S . -B build -DBUILD_TESTING=ON -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/msvc2022_64"
cmake --build build --config Debug --parallel
ctest --test-dir build -C Debug --output-on-failure
```

### macOS (Homebrew)

Install cmake, ninja, and qt if you haven't already:

```zsh
brew install cmake ninja qt
```

Run the build:

```zsh
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### Linux (Debian or Ubuntu)

Install cmake, ninja, and the Qt 6.11.0 Desktop package from the [official Qt installer](https://www.qt.io/download-open-source). Then install the remaining build tools:

```sh
sudo apt-get update
sudo apt-get install -y cmake ninja-build build-essential
```

Run the build:

```sh
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON -DCMAKE_PREFIX_PATH="$HOME/Qt/6.11.0/gcc_64"
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

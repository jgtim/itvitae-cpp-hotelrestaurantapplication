# molveno-resort-systeem
A system to improve the quality of life of Molveno Resort employees.

## Run tests locally

### macOS (Homebrew)

```zsh
brew install cmake ninja qt
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### Linux (Debian or Ubuntu)

```sh
sudo apt-get update
sudo apt-get install -y cmake ninja-build build-essential qt6-base-dev qt6-base-private-dev
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### Windows (official Qt installer)

```powershell
cmake -S . -B build -DBUILD_TESTING=ON -DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/msvc2022_64"
cmake --build build --config Debug --parallel
ctest --test-dir build -C Debug --output-on-failure
```

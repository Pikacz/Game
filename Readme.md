# MacOS 

## Setup:

1. Install Xcode with command line tools
2. Install Visual Studio Code
3. Install extension Run On Save (https://marketplace.visualstudio.com/items?itemName=emeraldwalk.RunOnSave)
4. Install extension Code Spell Checker (https://marketplace.visualstudio.com/items?itemName=streetsidesoftware.code-spell-checker)
5. Install extension clagd (https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)
6. Install extension Todo Tree (https://marketplace.visualstudio.com/items?itemName=Gruntfuggly.todo-tree)

## Building:

Project is configured to automatically build on source code change.

For compilation errors check visual studio code "Output->Run On Save" tab. 

To manually build use ./misc/macOS/build_if_needed.sh

To run built executable call ./build/macOS/game
Note that after compilation executable would be automatically kill if needed

# IDE for contract

This is IDE built to create & debug contract.

Also link to ub & hx to register & call contract onchain.

Both test and formal chain provided.

## Compiler Requirements

### Compiler C# Contract

[.net framework 4.5](https://www.microsoft.com/en-us/download/details.aspx?id=42642) or newer version is needed. Set environment path CSC as the csc.exe.

### Compiler java or kotlin Contract

[JDK8](https://docs.oracle.com/javase/8/docs/technotes/guides/install/install_overview.html)  is needed.

## Compiler Flow

1) Create or import contract source code(eg. new contract file with the template built-in).
2) Select the contract file in left-file-window, right clicked it or trigger the Compiler action in Debug menu or just press F7 shortcuts.
3) Select the .gpc file created right after step2, see the bytecode in editor or use the convenient function of the IDE to register & call contract onchain.(not necessarily)


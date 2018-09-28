# IDE for contract

This is IDE built to create & debug contract.

Also link to ub & hx &ctc to register & call contract onchain.

Both test and formal chain provided.

## Operate System

Winows 7 with Service Pack 1 or newer.

RAM: >=2G; Memory: >=10G; CPU i5 or above;


## Build & Install

Build from source require qt5.9 or later.
This will generate the executable file Copy and ChainIDE. Copy both to the main folder.

### Update

The update function is built in the IDE within help menu.



## Compiler Requirements( Usage of IDE)

### Compiler C# Contract

[.net framework 4.6](https://www.microsoft.com/en-us/download/details.aspx?id=48130) or newer version is needed.

### Compiler java or kotlin Contract

[JDK8](https://docs.oracle.com/javase/8/docs/technotes/guides/install/install_overview.html)  is needed.
Make sure the commond "java -version" is available.

## Compiler Flow

1) Create or import contract source code(eg. new contract file with the template built-in).
2) Select the contract file in left-file-window, right clicked it or trigger the Compiler action in Debug menu or just press F7 shortcuts.
3) Select the .gpc file created right after step2, see the bytecode in editor or use the convenient function of the IDE to register & call contract onchain.(not necessarily)


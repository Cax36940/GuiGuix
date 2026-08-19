# GuiGuix

**Experimental Graphical User Interface for Guix**

This project implements a GUI for Guix.
This project is experimental and is mainly intended to explore Guix
functionality through a graphical interface.

## Features

### Profile creation

* Profile is created to an input path.
* Profile is not created if it already exists.
* Profile is created empty (no installed package).

### Profile deletion

* There is no popup when pressing delete. It deletes the symlink that makes the profile.
* Don't delete a profile that you want to keep and don't have a manifest of.

### Search for a package

* Search is done on the packages available in the current Guix version.
* Search is case-insensitive.
* Search is done through string matching of each word typed (no regex).
* Search is done through package name, synopsis and description.

### Install a package on a profile

* Search works the same way as the one in the package tab, so it's not just the name even if it's what's displayed.

### Remove a package from a profile

### Open a terminal running a shell using a given profile

* A sandbox directory is created and set as home in the shell. Using Guix features, it separates the execution environment from the main one as much as possible.
* You can access this environment from the main one using the sandbox directory of the right profile.

## Installation

### Requirements

By default, this project uses X11 and has not been tested on Wayland. It will probably not link properly, but you can modify the `Makefile` to link with the required libraries.

These libraries are used by Raylib, so you can check the [Raylib build and installation guide](https://github.com/raysan5/raylib#build-and-installation).

This project also requires Guix to be installed: [Guix Installation](https://guix.gnu.org/manual/1.5.0/en/html_node/Binary-Installation.html).

### Installation

1. Clone the repo.
2. Run `make`.

## Launch

The executable is located in the `build` folder.

```bash
./build/main
```

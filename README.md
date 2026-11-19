# VCV Rack Plugins Repository

This repository contains source files for two VCV Rack plugins and a template for development.

## Contents
*   `cbites/`: Source files for the 'cbites' plugin.
*   `iirs/`: Source files for the 'iirs' plugin.
*   `template/`: Template files for new plugin development.
*   `procedura.txt`: Guide on setting up the compiling environment.

## Compilation Guide
For instructions on setting up the compiling environment and building the plugins, please refer to `procedura.txt`.

**Setup Steps (Summary from `procedura.txt`):**
1.  Ensure you have the Rack SDK installed and configured (e.g., exporting `RACK_DIR` in your shell configuration).
2.  Use the `helper.py` script to create plugins and modules.
3.  Use `make` to compile and `make install` to build the final plugins.

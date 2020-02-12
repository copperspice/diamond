## Diamond

### Introduction

Diamond is a cross platform compact editor designed for programmers. It was developed using the CopperSpice
libraries and was written in C++.

The Diamond project began as a replacement for the Crimson and Emerald Editors, both of these were Windows
only programs and are no longer supported. Diamond has evolved to become a programmers editor with a focus
on developing C++ appplications.


###### Key Features

* View multiple documents in movable tabs
* Auto Load previously open Files
* Configuration saved as a JSON format
* Define preset Folders
* Insert Date, Time, and Symbols
* Search and Replace
* Advanced Find
* Syntax Highlighting based on File Type
* Horizontal or Vertical split Window
* Spell Check
* Customize printed Header and Footer
* Show Spaces and Tabs
* User definable standard and custom Keyboard Shortcuts
* Record, Save, and Play back Macros


### Building

Diamond prebuilt binaries are available for Linux, Mac OS X, and Windows.

https://download.copperspice.com/diamond/binary/

To build Diamond from source a current copy of the CopperSpice libraries, a C++17 compiler, and a C++17 standard
library are required. Diamond uses the following CopperSpice libraries are required: CsCore, CsGui, CsNetwork.
Diamond also uses the CsPrinterDriver plugin which is copied to "your_path/diamond/printerdrivers" folder during
the install.

CMake build files are provided with the Diamond source distribution.


##### Additional Dependency

The diamond program uses the Hunspell library which is used for spell checking. When building Diamond from  source
code ensure you have Hunspell installed on your computer. For Windows developers the full source code is available on
our Diamond source download page. For other platforms we suggest installing the Hunspell package

* Debian, Ubuntu
   * `apt-get install libhunspell-dev`


* Fedora, Centos
   * `dnf install hunspell-devel`


* Mac OSX
   * `brew install hunspell`


### Documentation

Full documentation for Diamond is available on our download page.

|URL      |Description|
|---------|-----------|
|https://download.copperspice.com/diamond/documentation| Download (tar and zip formats)|


### Presentations

Our YouTube channel videos are about modern C++, build systems, CopperSpice, DoxyPress, and other topics related to
our open source work.

https://www.youtube.com/copperspice


Links to technical presentations recorded at CppNow and CppCon:

www.copperspice.com/presentations.html


### Authors

* **Barbara Geller**
* **Ansel Sermersheim**


### License

This application is released under the GPL 2 license. For more information refer to the LICENSE file provided with this
project.


### References

* Website: www.copperspice.com
* Email:   info@copperspice.com

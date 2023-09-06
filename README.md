![Jaut Banner](./resources/banner.png)


![GitHub Workflow Status (with event)](https://img.shields.io/github/actions/workflow/status/ElandaSunshine/Jaut/cmake-multi-platform.yml?style=flat-square&logo=cmake&logoColor=blue&label=CTest)
![GitHub tag (with filter)](https://img.shields.io/github/v/tag/ElandaSunshine/Jaut?filter=!v-*&style=flat-square&label=Version)
[![Codacy grade](https://img.shields.io/codacy/grade/722125803a844559a0e81855b20a45d0?style=flat-square&logo=codacy&label=Code%20Quality)
](https://app.codacy.com/gh/ElandaSunshine/Jaut/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade&label=Grade)

> [!WARNING]
> This is still in heavy development, use this at your own risk. Many things are still not working as expected, and some of the code formatting is still off. Namely the gui module will not work in its current state.

> [!NOTE]
> This repository is still in beta, it is not advisable to use it in production just yet.

# Introduction
JUCE, a wonderful framework, a vast collection of amazing tools.  
It's main focus has always been on audio, but lately evolved into much more than just for sound engineers; everyone can do almost everything with it and it constantly grows bigger and bigger.

However, it hasn't got everything and many many cool things it provides aren't that super duper great thing everyone is craving or just isn't there to begin with.  
This is the reason why this module-bundle came to exist.

JAUT aims at developers that are missing that feeling of completeness and don't like to bother too much with adding more and more 3rd Party externals just to get one thing going.  
All we try to accomplish is a central collection of useful stuff.

# Motivation
The origin-story of this collection couldn't be any more trivial.
During work on some projects, I came to realise that most of the things that flow into any of them are not unique cases.  
A lot of the used stuff might be used in the next project, and the next, and the... you get the gist.

So wouldn't it be a wonderful thing if all these projects had some sort of anchor-point?  
Is there a way to, such that I don't have to copy everything over and over again?  
Well, I love to break it to you:  
A big fat **YES**; the answer is a common toolset.

# Structure
At first sight, it might be a bit complicated to look over this bundle's structure, however, soon you will find out it is not that big of a deal.  
So let's have a deep dive into the various folders and bits this repo is comprised of.[^stc]

## Folders
This project comes with plenty of files and folders, so it's no surprise that some of them might not be that obvious to decipher, but most of them should be familiar to you:

- _**/cmake**_  
Main entry point for CMake scripts the project uses, all files inside this folder will be .cmake files used by the project.
  - _**/**_  
Contains all build and project related CMake scripts like find() related module definitions, install scripts, package definitions ect.
  - _**/tools**_  
Additional CMake scripts that are not used but provided by this package; to be used on the user side of things

- _**/docs**_  
Serving all documentation related needs for this repository.
  - _**/doxygen**_  
Any Doxygen related files and scripts needed to generate the source reference of this bundle.
  - _**/wiki**_  
The bundle's Wiki pages in raw format, for a fully rendered view refer to this repo's [Wiki Page](https://github.com/ElandaSunshine/Jaut/wiki)
- _**/examples**_  
Providing a set of examples, showcasing how some of the things are used and how you can use them too the best way possible.
- _**/modules**_  
The actual source code, split across several [JUCE Modules](https://github.com/juce-framework/JUCE/blob/master/docs/JUCE%20Module%20Format.md).
This will be the most important folder and where to look when you need something specific code-wise.

  See: [Modules](#modules)
- _**/resources**_  
Assets this some of this project's tools use to function properly or provide additional features.
- _**/test**_  
Testing facilities for this bundle, you probably won't ever need to open this folder except if you want to take inspiration on how to use things if the provided examples didn't fit your needs.

# Installation
The entire bundle is backed by a CMake module, so there is not much to do.
However, you will still need to be aware of one or the other thing.

## Dependencies
### JUCE
Since this repository is a juce module-bundle, it is just natural to fetch JUCE first before doing anything with it.  
Learn more here:  
https://github.com/juce-framework/juce

### Integrated
Some of the dependencies that this module bundle requires are directly embedded into the modules.
The following dependencies are hardwired into the codebase:
- **FMT**  
  Homepage: https://github.com/fmtlib/fmt  
  Path: modules/jaut_logger/detail  
  Provides the logging module with additional and optional formatting capabilities.

- **MS SafeInt**  
  Homepage: https://github.com/dcleblanc/SafeInt  
  Path: modules/jaut_core/detail/  
  Provides the capability of dealing with integer overflows, mainly used in the core module by the `jaut::Number` class.

### Optionals
There is also a few optional libraries that are used to enable a subset of additional functionality.
These are not necessary but needed when making use of these tools.

- **yaml-cpp**  
  Homepage: https://github.com/jbeder/yaml-cpp  
  Depends: `jaut::YamlParser`  
  Provides the JAUT configuration tools with a yaml parser.

## Setup
### CMake
It couldn't be any easier. Thanks to the JUCE module system, all you need to do is adding this bundle as a subdirectory. (after adding JUCE)  
You can then selectively link the module, you want to use, to your target:
```cmake
add_subdirectory(path/to/jaut)
target_link_libraries(YourTarget
    VISIBILITY
        jaut::jaut_core
        jaut::jaut_gui
        # ect.
        )
```

### Projucer
Add the module of interest to the module section of the Projucer. (the little '+' in the corner of the module list)

# Usage
Since JAUT is a bigger collection of tools and utilities, it wouldn't make much sense to give details about every single thing in this section. 
Thankfully, this repository also provides a wiki and a reference which sheds some light on the individual parts of this toolset which both can be found here:  
- [Wiki](https://github.com/ElandaSunshine/jaut/wiki)  
- [Documentation](https://elandasunshine.github.io/Jaut/docs/reference)

Also take a look at the `examples` and `test` folders for indicators on how to possibly use a tool.

# Known Issues
Here is a list of major known issues with this module bundle that take some more time to fix or can't be fixed due to limitations:
| Description | Comment | Issue |
|---|---|---|
| Module `jaut_gui` does not work | This is a major issue that is currently being worked on and will likely be fixed with any of the next bundle updates. For now it's best to ignore this module and focus on the other ones instead. | [#2](https://github.com/ElandaSunshine/Jaut/issues/2) |

# License
## JAUT
You may copy, distribute and modify the software as long as you track changes/dates in source files. Any modifications to or software including (via compiler) GPL-licensed code must also be made available under the GPL along with build & install instructions.

## FMT & MS SafeInt
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# Contact
If you need more information or would like to contact us, you can find us on our Discord:  
[![ElandaSunshine Community](https://discordapp.com/api/guilds/781531690383179826/widget.png?style=banner2)](https://discord.com/invite/jzRyAtnJBc)

[^stc]: Any of these things are subjects to change. It is best to keep that in mind and stay tuned.

![Jaut Banner](https://i.imgur.com/jre9B3S.png)

# WARNING
This is still in heavy development, use this at your own risk.
Many things are still not working as expected, and some of the code formatting is still off.
Namely the gui module will not work in its current state.

# JAUT: JUCE Augmented Utility Toolbox
JUCE, a wonderful framework, a vast collection of amazing tools.  
It's main focus has always been on audio, but lately evolved into much more than just for sound engineers; everyone can do almost everything with it and it constantly grows bigger and bigger.

However, it hasn't got everything and many many cool things it provides aren't that super duper great thing everyone is craving or just isn't there to begin with.  
This is the reason why this module-bundle came to exist.

JAUT aims at developers that are missing that feeling of completeness and don't like to bother too much with adding more and more 3rd Party externals just to get one thing going.  
All we try to accomplish is a central collection of useful stuff.

# Table of Contents
<details><summary>Click to expand</summary>

- [JAUT: JUCE Augmented Utility Toolbox](#jaut-juce-augmented-utility-toolbox)
- [Table of Contents](#table-of-contents)
- [Motivation](#motivation)
- [Installation](#installation)
  - [Dependencies](#dependencies)
    - [JUCE](#juce)
    - [Optionals](#optionals)
  - [Setup](#setup)
    - [CMake](#cmake)
    - [Projucer](#projucer)
- [Usage](#usage)
- [License](#license)
- [Contact](#contact)
</details>

# Motivation
The origin-story of this collection couldn't be any more trivial.
During work on some projects, I came to realise that most of the things that flow into any of them are not unique cases.  
A lot of the used stuff might be used in the next project, and the next, and the... you get the gist.

So wouldn't it be a wonderful thing if all these projects had some sort of anchor-point?  
Is there a way to, such that I don't have to copy everything over and over again?  
Well, I love to break it to you:  
A big fat **YES**; the answer is a common toolset.

# Installation
The entire bundle is backed by a CMake module, so there is not much to do.
However, you will still need to be aware of one or the other thing.

## Dependencies
### JUCE
Since this repository is a juce module-bundle, it is just natural to fetch JUCE first before doing anything with it.  
Learn more here:  
https://github.com/juce-framework/juce

### Optionals
There is also a few optonal libraries that are used to enable a subset of additional functionality.
These are not necessary but needed when making use of these tools.

- `jaut::YamlParser` depends on:   https://github.com/jbeder/yaml-cpp  
  Provides the JAUT configuration tools with a yaml parser

- `jaut::LogSinkFmt...` classes depend on:   https://github.com/fmtlib/fmt  
  Provides the JAUT logger classes with formatting capabilities

## Setup
### CMake
It couldn't be any easier. Thanks to the JUCE module system, all you need to do is adding this bundle as a subdirectory. (after adding JUCE)

You can then selectively link the module, you want to use, to your target.  

```cmake
add_subdirectory(path/to/jaut)
target_link_libraries(YourTarget
    VISIBILITY
        jaut::jaut_core
        jaut::jaut_gui
        # ect.
        )
```
That's it!  
You can now continue with your work. :)

### Projucer
Add the module of interest to the module section pf the Projucer. (the little '+' in the corner of the module list)

# Usage
Since JAUT is a bigger collection of tools and utilities, it wouldn't make much sense to give details about every single thing in this section.

Thankfully, this repository also provides a wiki and a reference which sheds some light on the individual parts of this toolset which can be found here:  
[Wiki](https://github.com/ElandaSunshine/jaut/wiki)  
[Documentation](https://elandasunshine.github.io/jaut)

# License
You may copy, distribute and modify the software as long as you track changes/dates in source files. Any modifications to or software including (via compiler) GPL-licensed code must also be made available under the GPL along with build & install instructions.

# Contact
If you need more information or would like to contact us, you can find us on our Discord:  
![ElandaSunshine Community](https://discordapp.com/api/guilds/781531690383179826/widget.png?style=banner2)

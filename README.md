<!--
SPDX-FileCopyrightText: 2021-2023 Daniel Aimé Valcour <fosssweeper@gmail.com>

SPDX-License-Identifier: MIT
-->

<!--
    Copyright (c) 2021-2023 Daniel Aimé Valcour

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in
    the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
    the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
    FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-->

![Screenshot](https://user-images.githubusercontent.com/60055347/162460417-d6114d2f-2386-4eae-ad15-e2db4062a078.png)

# roguelike.h

 [![REUSE status](https://api.reuse.software/badge/git.fsfe.org/reuse/api)](https://api.reuse.software/info/git.fsfe.org/reuse/api)

roguelike.h is a header only emulated terminal rendering library with support for Opengl33. Documentation is provided in the header file at the top of the file and for each function.

This library is designed for participants in the annual 7DRL game jam. If you use roguelike.h in your project, feel free to talk about it in an issue thread!

## Features

- A performant batched terminal rendering system that works on Mac, Windows, and Linux platforms.
- Support for custom glyph atlasses with as many glyphs as memory can contain across multiple texture pages.
- floating point fullcolor background and foreground colors per tile.
- Ability to render tiles on top of each other, with tiles rendered FIFO in the order that they are pushed into the terminal.
- Ability to render tiles offset from gridspace positions.
- Ability to render tiles with custom width and height per tile.

## Running The Example

Want to try the example project? You can set it up easily using bash console.

    git clone https://github.com/Journeyman-dev/roguelike.h
    cd roguelike.h
    cmake -S . -B ./build/ -D RLH_BUILD_EXAMPLE=ON -D RLH_EXAMPLE_AUTO_FETCH=ON
    cmake --build ./build/
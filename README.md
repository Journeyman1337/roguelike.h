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

# roguelike.h

roguelike.h is a header only emulated terminal rendering library with support for Opengl33. Documentation is provided in the header file at the top of the file and for each function.

This library is designed for participants in the annual 7DRL game jam. If you use roguelike.h in your project, feel free to talk about it in an issue thread!

# Features

- A performant batched terminal rendering system that is similliar to the renderer employed by the video game Dwarf Fortress:
  - The entire terminal is rendered in a single draw call.
  - Draw data is passed to the GPU in a giant buffer, which contains elements per tile rather than per vertex.
  - The data buffer is compacted as much as possible to improve latency. It contains only 18 bytes per tile.
  - On the GPU, most calculation is done per vertex rather than per fragment (pixel), which is more efficient.
- Support for custom glyph atlasses with up to 65535 tiles of custom sizes across multiple texture pages.
- 32 bit fullcolor background and foreground colors per tile.
- Ability to render tiles on top of each other, with tiles rendered FIFO in the order that they are pushed into the terminal.
- Ability to render tiles offset from gridspace positions.
- Ability to render tiles with custom width and height per tile.

# Screenshot

![Screenshot](https://user-images.githubusercontent.com/60055347/162460417-d6114d2f-2386-4eae-ad15-e2db4062a078.png)

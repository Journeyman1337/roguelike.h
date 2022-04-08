Header only roguelike rendering library. Support for Opengl33.

# Features
 * A performant batched rendering system that is similliar to the renderer employed by the video game Dwarf Fortress:
	* The entire terminal is rendered in a single draw call.
	* Draw data is passed to the GPU in a giant buffer, which contains elements per tile rather than per vertex.
	* The data buffer is compacted as much as possible to improve latency. It contains only 18 bytes per tile.
	* On the GPU, most calculation is done per vertex rather than per fragment (pixel), which results in much less work for the GPU to do.
 * Support for custom glyph atlasses with up to 65535 tiles of custom sizes across multiple texture pages.
 * 32 bit fullcolor background and foreground colors per tile.
 * Ability to render tiles on top of each other, with tiles rendered FIFO in the order that they are pushed into the terminal.
 * Ability to render tiles offset from gridspace positions.
 * Ability to render tiles with custom width and height per tile.
# Screenshot
![Screenshot](https://user-images.githubusercontent.com/60055347/162460417-d6114d2f-2386-4eae-ad15-e2db4062a078.png)

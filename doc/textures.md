## Texturing + Graphics Data ##

### Byte Ordering: Big Endian vs. Little Endian ###

In order to maximize data transfer speeds, graphics data (e.g. pixel colors,
fragment depth/buffer values, etc.) is often stored in bus-width byte arrays
(e.g. 32-bit integers on 32-bit machines). This formatting choice makes the
endianness of the host machine (i.e. the ordering of individual bytes within
a byte array) highly relevant as it effects how this data needs described when
transferred to the graphics processor. Fortunately, endianness is pretty simple
to account for since there are only two primary formats:

- **Big Endian**: For a given byte array at address `b` of length `l`, `b` has
  the most significant byte and `b+l-1` has the least significant byte (i.e.
  the byte significance is in descending order).
  ```
  int big_endian = 1; // => 0x00000001
  ```
- **Little Endian**: For a given byte array at address `b` of length `l`, `b` has
  the least significant byte and `b+l-1` has the most significant byte (i.e.
  the byte significance is in ascending order).
  ```
  int lil_endian = 1; // => 0x01000000
  ```

Generally speaking, modern commercial computers use *little endian* byte ordering,
but it's important to verify this ordering before passing any data to the GPU.

### OpenGL Formatting: Types + Formats ###

When transferring pixel information to OpenGL, three different pieces of data
formatting specification need to be provided: the `format`, the `type`, and the
`internalformat`. The first two of these parameters dictate the format of the
data on the host machine, and the final determines how the data will be stored
when it's transferred to the graphics processor. Generally speaking, it's desirable
to make the graphics byte ordering/structure identical to the host formatting to
eliminate the need for data transformations, but this in turn requires tuning all
data formats to match the data format found on the host machine. This will require
a complete analysis and understanding of both the host machine (e.g. endianness,
word byte array length, etc.) and the graphics data (e.g. bytes per channel, bytes
per pixel, etc.) to work properly.

For simple user-specified pixel data, the following settings are recommended in order
to achieve platform-independent data transfer:

- **Data**: Byte Stream of Pixels w/ RGBA Ordering
- `format`: `GL_RGBA`
- `type`: `GL_UNSIGNED_BYTE`
- `internalformat`: `GL_RGBA8`

### Resources ###

The information for this listing was pulled from the following resources:

- [CCSU Learning Assmebly: Big Endian and Little Endian](https://chortle.ccsu.edu/AssemblyTutorial/Chapter-15/ass15_3.html)
- [OpenGL: 'glTexImage2D' Reference](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml)
- [OpenGL Pixel Format Guide](https://afrantzis.com/pixel-format-guide/opengl.html)
- [Stack Overflow: Difference Between Format and InternalFormat](https://stackoverflow.com/a/34497547/837221)

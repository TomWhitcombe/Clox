C VM From https://craftinginterpreters.com

I'm playing around with a couple of ideas. I'm using macro'd dynamic arrays (not stretchy buffers) that are fully backed by a virtual page arena/linear allocator each with a gb of reserved memory.
These allocators can keep commiting more memory as needed without needing to realloc + copy to new location.

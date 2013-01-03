c41: Common C Code Collection
===

This is a freestanding library implementing various utility functions commonly
used in most programs (for example: string manipulation).

The library also defines various interfaces along with some support functions
for basic services such as memory allocation, file I/O, file system access, 
etc. It does not provide real implementations of those interfaces as they would
be OS/environment dependent.

All OS/environment dependent functions are part of a separate library - hbs1
(Host Basic Services v1). 

This model allows writing portable programs quite easily, the core of the 
program being isolated from all OS-dependent functions.

Since c41 is freestanding (no libs imported - except in some cases for what
the compiler enforces) the goal is to have it compiled for all environments
(all Unix-like systems, MS Windows as user-mode and kernel-mode library).

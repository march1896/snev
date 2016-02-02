# Introduction #

Unexpected link error.


# Details #

> I got link error "undefined reference to `glLoadIdentity@0'" for the link command _g++ -lopengl32 view.o_ I searched the whole internet but got no answer, it just tell that _-lopengl32_ is needed, but across the command above, the error still occurs.The reason is that we must add _-l_ in the end of the command, becasue when linking, linker reader the lib and determine which function is needed, if _-l_ is at the beginning of the command, no function is needed. So,
**we must add libs in the end of the linking command**
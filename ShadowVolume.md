# Introduction #

In shadow.cpp, I've casted shadow with _shadow volume_ method, this article record the problems I encounted.

# Details #
The idea of shadow volume is directly, just calculate the _space_ of shadow and check if any plane is _in this space_. If in, there will be a shadow. Many articles have introduced this.

Then very first step is to generate the silhouette, in this ball game, I try to guarantee the order of the vertex, because in the first pass, take **depth pass** for example, when we draw into the stencil buffer, we have to tell front face of shadow volume from back face.
If we can only "see" the front face( the back face can not pass the depth test ), this pixel is in the shadow. So the order of vertex is inportant. But in other words, if the front face and back face of shadow volume can all be seen or all can not be seen, the pixel is not in shadow, if only one of the two faces can be seen, this pixel is in shadow. So, use depth-pass method ,just find the silhouette is OK, but depth-fail?...._depth fail_....

The value in stencil buffer is init as 0, so if you can not tell frontface from backface, you have to init the stencill buffer to a positive number,becasuse the value can not be negative.
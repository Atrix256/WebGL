#define SHADER(...) #__VA_ARGS__

const char *s_vertexShader=
SHADER(
attribute vec2 aVertexPosition;
attribute vec2 aTextureCoord;

varying vec2 vTextureCoord;

void main(void) {
    gl_Position = vec4(aVertexPosition, 1.0, 1.0);
    vTextureCoord = aTextureCoord;
}
);

const char *s_fragmentShader=
SHADER(
/*
   Description:
   1 texture sample taken to make 1 curve for each color channel, where each curve is order 2.  The source texture dimensions are 2x2.

   Texture Layout:
   [R0,G0,B0,A0],[R1,G1,B1,A1],
   [R1,G1,B1,A1],[R2,G2,B2,A2],
   
*/

//#extension GL_OES_standard_derivatives : enable
//#extension OES_texture_float : enable
//#extension OES_texture_float_linear : enable
//precision mediump float;
uniform sampler2D uSampler;
varying vec2 vTextureCoord;

void main(void) {
   vec2 textureCoord = vTextureCoord;

   // Calculate the powers of s and t that we will need.
   float xOffset = 0.0;
   float t1 = textureCoord.x;

   // Calculate our sample value.
   // The center of each pixel is where the color is.
   float sampleX = (0.25 + t1 / 2.0) / 1.0 + xOffset;
   float sampleY = (0.25 + t1 / 2.0);

   // Do the texture sample
   vec4 curveValues = texture2D(uSampler, vec2(sampleX, sampleY));

   // R,G,B curves add to their color channels respectively, alpha adds to all.
   vec3 outColor;
   outColor.x = step(textureCoord.y, curveValues.x) * 0.5;
   outColor.y = step(textureCoord.y, curveValues.y) * 0.5;
   outColor.z = step(textureCoord.y, curveValues.z) * 0.5;
   float v = step(textureCoord.y, curveValues.w) * 0.5;
   outColor.x += v;
   outColor.y += v;
   outColor.z += v;

   // make the output color
   gl_FragColor = vec4(clamp(outColor,0.0,1.0), 1.0);
}
);

#undef SHADER
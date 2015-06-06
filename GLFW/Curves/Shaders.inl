#define SHADER(...) #__VA_ARGS__

const char *s_vertexShader=
SHADER(
    attribute vec3 aVertexPosition;
    attribute vec3 aVertexNormal;
    attribute vec2 aTextureCoord;

    varying vec2 vTextureCoord;
    varying vec3 vNormal;

    uniform mat4 uPMatrix;
    uniform mat3 uNMatrix;
    uniform vec2 uRenderOffset;

    void main(void) {
        vec4 pos = uPMatrix * vec4(aVertexPosition, 1.0);
        pos += vec4(uRenderOffset * pos.w, 0.0, 0.0);
        gl_Position = pos;
        vTextureCoord = aTextureCoord;

        // calculate normal
        vNormal = uNMatrix * aVertexNormal;
    }
);

const char *s_fragmentShader=
SHADER(
    attribute vec3 aVertexPosition;
    attribute vec3 aVertexNormal;
    attribute vec2 aTextureCoord;

    varying vec2 vTextureCoord;
    varying vec3 vNormal;

    uniform mat4 uPMatrix;
    uniform mat3 uNMatrix;
    uniform vec2 uRenderOffset;

    void main(void) {
        vec4 pos = uPMatrix * vec4(aVertexPosition, 1.0);
        pos += vec4(uRenderOffset * pos.w, 0.0, 0.0);
        gl_Position = pos;
        vTextureCoord = aTextureCoord;

        // calculate normal
        vNormal = uNMatrix * aVertexNormal;
    }
);

#undef SHADER
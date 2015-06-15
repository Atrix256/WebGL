#include "Shaders.h"

//=============================================================================================================
//                                            RationalTest
//=============================================================================================================

void CShaderRationalTest::Init()
{
    SetAttributeData_aTextureCoord({
        0.0, 0.0,
        2.0, 0.0,
        2.0, 1.0,
        0.0, 0.0,
        2.0, 1.0,
        0.0, 1.0,
    });
    SetAttributeData_aVertexPosition({
        -1.0, -1.0,
         1.0, -1.0,
         1.0,  1.0,
        -1.0, -1.0,
         1.0,  1.0,
        -1.0,  1.0,
    });

    float A1 = 0.0f;
    float B1 = 1.0f;
    float C1 = 1.0f;

    float A2 = 1.0f;
    float B2 = 1.0f;
    float C2 = 0.0f;

    float W1 = 1.0f;
    float W2 = 1.0f / sqrtf(2.0);
    float W3 = 1.0f;

    A1 *= W1;
    B1 *= W2;
    C1 *= W3;

    A2 *= W1;
    B2 *= W2;
    C2 *= W3;

    // Note - the B channel is empty, we could encode a 3rd rational curve in there, so long
    // as it used the same W weights! (:
    SetTextureData_uSampler(2, 2, {
        A1, A2, 0.0f, W1,     B1, B2, 0.0f, W2,
        B1, B2, 0.0f, W2,     C1, C2, 0.0f, W3
    });
}

const char *CShaderRationalTest::GetVertexShader()
{
    return
    SHADER_SOURCE(
    in vec2 aVertexPosition;
    in vec2 aTextureCoord;

    out vec2 vTextureCoord;

    void main(void) {
        gl_Position = vec4(aVertexPosition, 1.0, 1.0);
        vTextureCoord = aTextureCoord;
    }
    );
}

const char *CShaderRationalTest::GetFragmentShader()
{
    return
    SHADER_SOURCE(
    uniform sampler2D uSampler;
    
    in vec2 vTextureCoord;

    out vec4 outColor;

    vec4 SampleTime(vec2 time, bool linearSampling) {
        // Bilinear sampling:
        // Hardware based bilinear sampling
        if (linearSampling)
            return texture(uSampler, (time + 0.5) / 2.0);

        // Nearest sampling:
        // Software bilinear sampling (higher quality)
        vec2 frac = fract(time);

        vec2 floorTime = floor(time) + 0.5;

        vec4 A = texture(uSampler, (floorTime + vec2(0.0, 0.0)) / 2.0);
        vec4 B = texture(uSampler, (floorTime + vec2(1.0, 0.0)) / 2.0);
        vec4 C = texture(uSampler, (floorTime + vec2(0.0, 1.0)) / 2.0);
        vec4 D = texture(uSampler, (floorTime + vec2(1.0, 1.0)) / 2.0);

        return mix(mix(A, B, frac.x), mix(C, D, frac.x), frac.y);
    }

    void main(void) {
        vec4 colorValue = vec4(0.0);

        bool linearSampling = false;
        float time = 0.0;

        if (vTextureCoord.x < 0.995)
        {
            time = vTextureCoord.x / 0.995;
            linearSampling = true;
        }
        else if (vTextureCoord.x > 1.005)
        {
            time = fract(vTextureCoord.x - 0.005) / 0.995;
            linearSampling = false;
        }
        else
        {
            outColor = vec4(1.0);
            return;
        }

        // we want to process all 4 quadrants between texture coordinates 0 and 1
        float origTime = time;
        time *= 4.0;
        int quadrant = int(floor(time));
        time = fract(time);

        // reverse time in quadrant 1 and 3
        if (quadrant == 1 || quadrant == 3)
            time = 1.0 - time;

        // sample the curves in R,G,B,A
        colorValue = SampleTime(vec2(time), linearSampling);

        // Calculate rational sin curve in R,A (x,w) and rational cosine curve in G,A (y,w)
        vec2 scValue = vec2(colorValue.x / colorValue.w, colorValue.y / colorValue.w);

        // calculate actual sine and cosine values
        vec2 actualscValue = vec2(sin(origTime * 6.28), cos(origTime * 6.28));

        // scale / offset to make the screen's 0-1 map to -1 to 1 for the graphs
        scValue = scValue * 0.5 + 0.5;
        actualscValue = actualscValue * 0.5 + 0.5;

        // sine needs to be flipped over in quadrant 2 and 3 (III and IV)
        if (quadrant == 2 || quadrant == 3)
            scValue.x = 1.0 - scValue.x;

        // cosine needs to be flipped over in quadrant 1 and 2 (II and III)
        if (quadrant == 1 || quadrant == 2)
            scValue.y = 1.0 - scValue.y;

        // scale to 90% size
        scValue = scValue * 0.9 + 0.05;
        actualscValue = actualscValue * 0.9 + 0.05;

        // calculate the slopes
        vec2 slope = dFdx(scValue) / dFdx(vTextureCoord.x);
        vec2 actualSlope = dFdx(actualscValue) / dFdx(vTextureCoord.x);

        // smoothstep to make it a line with width, using the gradient (from slope) to estimate distance from pixel to graph
        scValue.x = smoothstep(0.01, 0.0, abs(vTextureCoord.y - scValue.x) / length(vec2(slope.x, -1.0)));
        scValue.y = smoothstep(0.01, 0.0, abs(vTextureCoord.y - scValue.y) / length(vec2(slope.y, -1.0)));
        actualscValue.x = smoothstep(0.003, 0.0, abs(vTextureCoord.y - actualscValue.x) / length(vec2(actualSlope.x, -1.0)));
        actualscValue.y = smoothstep(0.003, 0.0, abs(vTextureCoord.y - actualscValue.y) / length(vec2(actualSlope.y, -1.0)));

        // color pixel based on values
        outColor = vec4(scValue.x, actualscValue.x, actualscValue.x, 1.0);
        outColor += vec4(actualscValue.y, scValue.y, actualscValue.y, 1.0);

        // clamp the output color
        outColor = vec4(clamp(outColor, 0.0, 1.0));
    }
    );
}

/*

TODO:
 * make this use the same "W only" curve, instead of filling all 4 color channels.
 * make another rational test that has 3 random curves divided by a 4th "W only" curve.

*/
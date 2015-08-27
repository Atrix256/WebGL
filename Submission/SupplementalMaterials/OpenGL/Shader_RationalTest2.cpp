#include "Shaders.h"

//=============================================================================================================
//                                            RationalTest2
//=============================================================================================================

void CShaderRationalTest2::Init()
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

const char *CShaderRationalTest2::GetVertexShader()
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

const char *CShaderRationalTest2::GetFragmentShader()
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

        bool linearSampling = true;
        float time = vTextureCoord.x;

        // we want to process all 4 quadrants between texture coordinates 0 and 1
        float origTime = time;
        time *= 4.0;
        int quadrant = int(floor(time))%4;
        time = fract(time);

        // reverse time in quadrant 1 and 3
        if (quadrant == 1 || quadrant == 3)
            time = 1.0 - time;

        // sample the curves in R,G,B,A
        colorValue = SampleTime(vec2(time), linearSampling);

        // Calculate rational sin curve in R,A (x,w)
        float sinValue = colorValue.x / colorValue.w;

        // calculate actual sine value
        float actualsinValue = sin(origTime * 6.28318530718);

        // scale / offset to make the screen's 0-1 map to -1 to 1 for the graphs
        sinValue = sinValue * 0.5 + 0.5;
        actualsinValue = actualsinValue * 0.5 + 0.5;

        // sine needs to be flipped over in quadrant 2 and 3 (III and IV)
        if (quadrant == 2 || quadrant == 3)
            sinValue = 1.0 - sinValue;

        // scale to 90% size
        sinValue = sinValue * 0.9 + 0.05;
        actualsinValue = actualsinValue * 0.9 + 0.05;

        // calculate the slopes
        float slope = dFdx(sinValue) / dFdx(vTextureCoord.x);
        float actualSlope = dFdx(actualsinValue) / dFdx(vTextureCoord.x);

        // smoothstep to make it a line with width, using the gradient (from slope) to estimate distance from pixel to graph
        sinValue = 1.0 - step(0.005, abs(vTextureCoord.y - sinValue) / length(vec2(slope, -1.0)));
        actualsinValue = 1.0 - step(0.005,abs(vTextureCoord.y - actualsinValue) / length(vec2(actualSlope, -1.0)));

        // color pixel based on values
        if (actualsinValue == 1.0)
        {
            outColor = vec4(vec3(sinValue*0.25), 1.0);
        }
        else
        {
            outColor = vec4(sinValue, 0.0, 0.0, 1.0);
        }

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
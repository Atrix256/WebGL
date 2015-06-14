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

    float A = 0.0f;
    float B = 1.0f;
    float C = 1.0f;

    float A2 = 1.0f;
    float B2 = 1.0f;
    float C2 = 0.0f;

    float W1 = 1.0f;
    float W2 = 1.0f / sqrtf(2.0);
    float W3 = 1.0f;

    SetTextureData_uSampler(2, 2, {
        A * W1, W1, A2 * W1, W1,     B * W2, W2, B2 * W2, W2,
        B * W2, W2, B2 * W2, W2,     C * W3, W3, C2 * W3, W3
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

        float origTime = time;

        time *= 4.0;
        int quadrant = int(floor(time));
        time = fract(time);

        switch (quadrant)
        {
        case 1:
        case 3: time = 1.0 - time;
        }

        // sample the curves in R,G,B,A
        colorValue = SampleTime(vec2(time), linearSampling);

        // Calculate rational sin curve in R,G, and also the actual true sine value
        float sineValue = colorValue.x / colorValue.y;

        switch (quadrant)
        {
        case 0:
        case 1: sineValue = sineValue / 2.0 + 0.5; break;
        case 2:
        case 3: sineValue = (1.0 - (sineValue / 2.0 + 0.5)); break;
        }
        sineValue = sineValue * 0.9 + 0.05;

        float actualSineValue = sin(origTime * 6.28) * 0.5 + 0.5;
        actualSineValue = actualSineValue * 0.9 + 0.05;

        vec2 slope = dFdx(vec2(sineValue, actualSineValue)) / dFdx(vTextureCoord.x);

        sineValue = smoothstep(0.01, 0.0, abs(vTextureCoord.y - sineValue) / length(vec2(slope.x, -1.0)));
        actualSineValue = smoothstep(0.003, 0.0, abs(vTextureCoord.y - actualSineValue) / length(vec2(slope.y, -1.0)));

        outColor = vec4(sineValue, actualSineValue, actualSineValue, 1.0);

        // calculate the rational cos curve in B,A and also the actual true cosine value
        float cosValue = colorValue.z / colorValue.w;
        cosValue = cosValue * 0.9 + 0.05;

        float actualCosineValue = cos(origTime * 6.28) * 0.5 + 0.5;
        actualCosineValue = actualCosineValue * 0.9 + 0.05;
        
        slope = dFdx(vec2(cosValue, actualCosineValue)) / dFdx(vTextureCoord.x);

        cosValue = smoothstep(0.01, 0.0, abs(vTextureCoord.y - cosValue) / length(vec2(slope.x, -1.0)));
        actualCosineValue = smoothstep(0.003, 0.0, abs(vTextureCoord.y - actualCosineValue) / length(vec2(slope.y, -1.0)));

        //outColor += vec4(cosValue, actualCosineValue, actualCosineValue, 1.0);

        // clamp the output color
        outColor = vec4(clamp(outColor, 0.0, 1.0));
    }
    );
}

/*

TODO:
* cosine needs quadrant treatment
 * we change "time" for when we sample sine.  i don't think it's valid to do the same for cosine (diff quadrantnrules)
  * might be able to work it so that the time we sample is the correct one.  i hope so!
 * also need to flip y over and half them etc.

*/
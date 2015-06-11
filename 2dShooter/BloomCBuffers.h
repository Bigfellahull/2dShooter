#pragma once
#include "SimpleMath.h"

namespace SGS2D
{
    struct BloomExtractCBuffer
    {
        BloomExtractCBuffer() :
            BloomThreshold()
        {
        }

        BloomExtractCBuffer(
            float bloomThreshold
            ) :
            BloomThreshold(bloomThreshold)
        {
        }

        BloomExtractCBuffer(const BloomExtractCBuffer& other) :
            BloomThreshold(other.BloomThreshold)
        {
        }

        BloomExtractCBuffer& operator=(const BloomExtractCBuffer& other)
        {
            BloomThreshold = other.BloomThreshold;
            return *this;
        }
        
        // The minimum brightness of a pixel in order for bloom to apply to it on a 0.0f (all pixels) to 1.0f (only 100% white pixels) scale.
        float BloomThreshold;
    };

    // BLUR_SAMPLE_COUNT must match the value in BloomBlurPixelShader.hlsl.
    const int BLUR_SAMPLE_COUNT = 15;

    struct BloomBlurCBuffer
    {
        DirectX::SimpleMath::Vector4 SampleOffsetsAndWeights[BLUR_SAMPLE_COUNT];
    };

    struct BloomCombineCBuffer
    {
        BloomCombineCBuffer() :
            CombineValues()
        {
        }

        BloomCombineCBuffer(
            float bloomIntensity,
            float baseIntensity,
            float bloomSaturation,
            float baseSaturation
            ) :
            CombineValues(bloomIntensity, baseIntensity, bloomSaturation, baseSaturation)
        {
        }

        BloomCombineCBuffer(const BloomCombineCBuffer& other) :
            CombineValues(other.CombineValues)
        {
        }

        BloomCombineCBuffer& operator=(const BloomCombineCBuffer& other)
        {
            CombineValues = other.CombineValues;
            return *this;
        }

        // x - BloomIntensity ; y - BaseIntensity ; z - BloomSaturation ; w - BaseSaturation
        DirectX::SimpleMath::Vector4 CombineValues;
    };
}
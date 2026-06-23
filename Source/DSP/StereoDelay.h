#pragma once
#include <juce_dsp/juce_dsp.h>
#include "Utils.h"

class StereoDelay
{
public:
    void prepare(double newSampleRate, int maxBlockSize)
    {
        sampleRate = newSampleRate;
        const int maxDelaySamples = (int) (sampleRate * 2.0); // up to 2s
        delayLineL.prepare({ sampleRate, (juce::uint32) maxBlockSize, 1 });
        delayLineR.prepare({ sampleRate, (juce::uint32) maxBlockSize, 1 });
        delayLineL.setMaximumDelayInSamples(maxDelaySamples);
        delayLineR.setMaximumDelayInSamples(maxDelaySamples);
        reset();
    }

    void reset()
    {
        delayLineL.reset();
        delayLineR.reset();
    }

    void setParameters(float timeMs, float feedback01, float mix01)
    {
        delayTimeSamples = (float) (timeMs * 0.001 * sampleRate);
        feedback = juce::jlimit(0.0f, 0.95f, feedback01);
        mix = juce::jlimit(0.0f, 1.0f, mix01);
    }

    void processBlock(float* left, float* right, int numSamples)
    {
        delayLineL.setDelay(delayTimeSamples);
        delayLineR.setDelay(delayTimeSamples);

        for (int i = 0; i < numSamples; ++i)
        {
            const float inL = left[i];
            const float inR = right[i];

            const float delayedL = delayLineL.popSample(0);
            const float delayedR = delayLineR.popSample(0);

            // Slight cross-feed between channels widens the tail without
            // turning it into a literal ping-pong delay.
            const float feedL = inL + (delayedR * 0.25f + delayedL * 0.75f) * feedback;
            const float feedR = inR + (delayedL * 0.25f + delayedR * 0.75f) * feedback;

            delayLineL.pushSample(0, DspUtils::softClip(feedL));
            delayLineR.pushSample(0, DspUtils::softClip(feedR));

            left[i]  = inL * (1.0f - mix) + delayedL * mix;
            right[i] = inR * (1.0f - mix) + delayedR * mix;
        }
    }

private:
    double sampleRate = 44100.0;
    juce::dsp::DelayLine<float> delayLineL { 96000 }, delayLineR { 96000 };
    float delayTimeSamples = 0.0f;
    float feedback = 0.3f;
    float mix = 0.0f;
};

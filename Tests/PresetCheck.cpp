// Bass Professor: render every preset, check it sounds (not silent, no NaN, no overload)
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cstdio>
int main (int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI init;
    UndertowAudioProcessor p;
    p.setPlayConfigDetails (0, 2, 48000, 512);
    p.prepareToPlay (48000, 512);
    const int N = p.getNumBassPresets();
    int bad = 0;
    std::vector<std::string> worst;
    for (int i = 0; i < N; ++i)
    {
        p.applyBassPreset (i);
        p.prepareToPlay (48000, 512);
        juce::AudioBuffer<float> b (2, 512); juce::MidiBuffer m;
        double sum = 0; float peak = 0; bool nan = false; int n = 0;
        // a short phrase: E1, G1, A1, E2 (8th notes at 128 BPM) then release
        const int notes[] = { 28, 31, 33, 40 };
        for (int blk = 0; blk < 280; ++blk)
        {
            m.clear();
            const int t = blk * 512, step = (int) (48000 * 60.0 / 128 / 2);
            for (int k = 0; k < 8; ++k)
            {
                const int on = k * step, off = on + step * 3 / 4;
                if (on >= t && on < t + 512) m.addEvent (juce::MidiMessage::noteOn (1, notes[k % 4], (juce::uint8) 100), on - t);
                if (off >= t && off < t + 512) m.addEvent (juce::MidiMessage::noteOff (1, notes[k % 4]), off - t);
            }
            p.processBlock (b, m);
            for (int c = 0; c < 2; ++c) for (int s = 0; s < 512; ++s) { const float v = b.getSample (c, s); if (! std::isfinite (v)) nan = true; peak = std::max (peak, std::abs (v)); sum += v * v; ++n; }
        }
        const double rmsDb = 10 * std::log10 (sum / n + 1e-20);
        const bool ok = ! nan && peak > 0.01f && peak < 4.0f && rmsDb > -48;
        if (! ok || i >= 200 || argc > 1)
            printf ("%3d %-26s %-16s peak %.2f rms %6.1f dB %s\n", i, p.getBassPresetNames()[i].toRawUTF8(), p.getBassPresetCategory (i).toRawUTF8(), peak, rmsDb, ok ? "" : (nan ? "NaN!" : "BAD"));
        if (! ok) ++bad;
    }
    // editor builds
    std::unique_ptr<juce::AudioProcessorEditor> ed (p.createEditor());
    printf ("%d presets, %d bad, editor %dx%d\n", N, bad, ed ? ed->getWidth() : 0, ed ? ed->getHeight() : 0);
    return bad ? 1 : 0;
}

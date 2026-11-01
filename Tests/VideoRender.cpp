// BPVideo: Bass Professor demo, rendered offline with the real synth + editor.
// Plays a bassline through eight of the new 2.1 presets (2 bars each, 128 BPM) and writes
// fNNNNN.png + bass.wav + captions.txt. Drums are added under the bass when encoding.
// Usage: BPVideo outDir [scale]
#include <JuceHeader.h>
#define private public
#define protected public
#include "PluginProcessor.h"
#include "PluginEditor.h"
#undef private
#undef protected
#include <cstdio>

int main (int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI init;
    const double SR = 48000.0; const int FPS = 30, spf = 1600, B = 400;
    const juce::File outDir (argv[1]);
    const float scale = argc > 2 ? (float) atof (argv[2]) : 1.25f;
    outDir.createDirectory();

    UndertowAudioProcessor proc;
    proc.setPlayConfigDetails (0, 2, SR, B);
    proc.prepareToPlay (SR, B);
    std::unique_ptr<juce::AudioProcessorEditor> ed (proc.createEditor());
    auto* e = static_cast<UndertowAudioProcessorEditor*> (ed.get());

    // pattern kinds: R rolling off-16ths, G gallop, O offbeat 8ths, A afro, S 808 slide, W wobble, D dub, T trance
    struct Part { const char* preset; char kind; };
    const Part parts[] = { { "Rolling Psy Classic", 'R' }, { "Full-On Gallop", 'G' }, { "Warehouse Thud", 'O' }, { "Afro Log Drum", 'A' },
                           { "Drill Slide", 'S' }, { "Classic Wobble 1/4", 'W' }, { "Steppers Sine", 'D' }, { "Offbeat Trance Saw", 'T' } };
    const int nParts = 8;
    const double beat = 60.0 / 128.0, bar = 4 * beat, partLen = 2 * bar, tEnd = nParts * partLen + 1.0;
    const auto names = proc.getBassPresetNames();

    // MIDI events (time, note, vel, on)
    struct Ev { double t; int note; int vel; bool on; };
    std::vector<Ev> ev;
    auto note = [&] (double t, double len, int n, int v = 100) { ev.push_back ({ t, n, v, true }); ev.push_back ({ t + len, n, 0, false }); };
    const int roots[4] = { 33, 33, 36, 31 };   // A1 A1 C2 G1 (A minor)
    const double s16 = beat / 4;
    for (int p = 0; p < nParts; ++p)
        for (int b = 0; b < 2; ++b)
        {
            const double t0 = p * partLen + b * bar; const int r = roots[(p * 2 + b) % 4];
            switch (parts[p].kind)
            {
                case 'R': for (int i = 0; i < 16; ++i) if (i % 4) note (t0 + i * s16, s16 * 0.8, r, 105); break;
                case 'G': for (int i = 0; i < 16; ++i) if (i % 4 != 0 && i % 4 != 2) note (t0 + i * s16, s16 * 0.8, r, 105);
                          for (int i = 2; i < 16; i += 4) note (t0 + i * s16, s16 * 0.8, r + 12, 90); break;
                case 'O': for (int k = 0; k < 4; ++k) note (t0 + k * beat + beat / 2, beat * 0.4, r, 110); break;
                case 'A': { const int st[] = { 0, 3, 6, 10, 12, 14 }; for (int i : st) note (t0 + i * s16, s16 * 1.6, (i == 10 ? r + 7 : r), 100); } break;
                case 'S': note (t0, beat * 1.4, r, 115); note (t0 + 1.5 * beat, beat * 0.9, r, 100); note (t0 + 2.5 * beat, beat * 1.4, r + (b ? 5 : 3), 110); break;
                case 'W': note (t0, bar * 0.98, r, 110); break;
                case 'D': note (t0, beat * 0.9, r, 110); note (t0 + 1.5 * beat, beat * 0.45, r, 95); note (t0 + 2 * beat, beat * 0.9, r + 7, 100); note (t0 + 3.5 * beat, beat * 0.45, r + 3, 95); break;
                case 'T': for (int k = 0; k < 4; ++k) { note (t0 + k * beat + beat / 2, s16 * 0.9, r, 110); note (t0 + k * beat + 3 * s16, s16 * 0.9, r, 100); } break;
            }
        }
    std::sort (ev.begin(), ev.end(), [] (const Ev& a, const Ev& b) { return a.t < b.t || (a.t == b.t && ! a.on && b.on); });

    const int nFrames = (int) (tEnd * FPS);
    juce::AudioBuffer<float> out (2, nFrames * spf); out.clear();
    juce::AudioBuffer<float> blk (2, B);
    size_t ei = 0; int cur = -1; juce::String caps;
    for (int fr = 0; fr < nFrames; ++fr)
    {
        const double t = (double) fr / FPS;
        const int part = juce::jmin (nParts - 1, (int) (t / partLen));
        if (part != cur)
        {
            cur = part;
            const int idx = names.indexOf (parts[part].preset);
            if (idx >= 0) e->presetBox.setSelectedId (idx + 1, juce::sendNotificationSync);
            caps << juce::String (part * partLen, 3) << "\t" << juce::String (part + 1 < nParts ? (part + 1) * partLen : tEnd, 3) << "\t"
                 << parts[part].preset << "  -  " << (idx >= 0 ? proc.getBassPresetCategory (idx) : juce::String ("?")) << "\n";
            printf ("%5.2f %s (%d)\n", t, parts[part].preset, idx);
        }
        for (int k = 0; k < spf; k += B)
        {
            const double bt0 = t + k / SR, bt1 = bt0 + B / SR;
            juce::MidiBuffer midi;
            while (ei < ev.size() && ev[ei].t < bt1)
            {
                const int pos = juce::jlimit (0, B - 1, (int) ((ev[ei].t - bt0) * SR));
                midi.addEvent (ev[ei].on ? juce::MidiMessage::noteOn (1, ev[ei].note, (juce::uint8) ev[ei].vel) : juce::MidiMessage::noteOff (1, ev[ei].note), pos);
                ++ei;
            }
            blk.clear();
            proc.processBlock (blk, midi);
            for (int ch = 0; ch < 2; ++ch) out.copyFrom (ch, fr * spf + k, blk, ch, 0, B);
        }
        auto img = ed->createComponentSnapshot (ed->getLocalBounds(), true, scale);
        juce::File f = outDir.getChildFile (juce::String::formatted ("f%05d.png", fr)); f.deleteFile();
        juce::FileOutputStream fos (f); juce::PNGImageFormat().writeImageToStream (img, fos);
    }
    juce::WavAudioFormat wav; auto wf = outDir.getChildFile ("bass.wav"); wf.deleteFile();
    std::unique_ptr<juce::AudioFormatWriter> w (wav.createWriterFor (new juce::FileOutputStream (wf), SR, 2, 24, {}, 0));
    w->writeFromAudioSampleBuffer (out, 0, out.getNumSamples());
    outDir.getChildFile ("captions.txt").replaceWithText (caps);
    printf ("frames %d peak %.3f\n", nFrames, out.getMagnitude (0, out.getNumSamples()));
    ed = nullptr;
}

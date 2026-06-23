# Bass Professor (v2.0)

**Bass Professor** — a 3-band polyphonic bass synthesizer plugin built in JUCE.
(C)(P) 2026 Tully EDM Vibe · info@tullyedmvibe.com

A polyphonic bass synth plugin built in JUCE. This is a real, working synth
core — not a stub — verified before being handed off:

- DSP unit-level smoke test (every oscillator/filter/LFO combination)
- A full **integration test under AddressSanitizer**: real `PluginProcessor`,
  real `prepareToPlay`/`processBlock` — poly mode with two simultaneous
  notes, high filter resonance, LFO-on-cutoff, chorus + delay engaged;
  mono mode with overlapping note stacks under both legato states
  (verifying last-note-priority fallback); a mid-stream poly/mono mode
  switch; wavetable mode with a live position-morph sweep; 3-band mode with
  distinct per-band models/pitch/pan plus a live crossover sweep and band
  muting; three simultaneous per-band filters (LP/BP/HP, high-Q) with a
  live cutoff sweep; all three bands driven to max saturation on a
  max-velocity note (bounded-output check); and a state save/load round
  trip; three bands running three different engines at once (Body analog,
  Character FM, Air wavetable); AM injector at full depth on both targets
  stacked on driven bands; FM mode with a live index sweep at high
  feedback; plus FM corner-case
  coverage in the DSP smoke test (every ratio/index/feedback extreme).
  Plus a dedicated crossover
  reconstruction test (band sum vs unity energy). Zero memory errors, zero
  NaN/Inf across all of it.
- The `.jucer` itself was validated against a real Projucer binary (built
  from source in the process), including confirming the exact module-path
  convention for a sibling JUCE/Undertow folder layout

AU/VST3 themselves couldn't be compiled in the build environment this was
made in (Linux, no Xcode) — that step is yours, on your Mac.

## What's in v2.0

- **Three oscillator engines**, switchable per patch via Osc Model:
  - *Analog* — dual sub-oscillator (Saw/Square/Triangle/Sine, Osc B also
    has Noise), antialiased with PolyBLEP, pulse width, hard sync.
  - *FM* — two-operator phase-modulation FM with ratio, index, and
    modulator self-feedback. The foundational source for DX-style
    metallic/wooden basses and punchy modern FM bass. Output is inherently
    sin-bounded, so it stays stable even at max index + feedback. As of
    v1.9, FM is also selectable **per band** — each of Body/Character/Air
    can independently run Analog, Wavetable, or FM.
  - *Wavetable* — 6 procedurally generated band-limited tables
    (sine → triangle → saw → square → formant → bass-harmonic), mip-mapped
    by octave so high notes don't alias. WT Position scans/crossfades
    continuously across the set; WT Warp skews phase symmetry for moving
    timbres. Nothing to ship — tables are built at load time.
  - Both engines share octave/semitone/fine tune.
- **3-Band architecture** *(new)*: the Body / Character / Air split from the
  original spec. Toggle 3-Band Mode and the voice splits into three
  independent oscillator lanes, each band-limited by a 4th-order
  Linkwitz-Riley crossover (24 dB/oct) at two adjustable crossover points,
  then merged. Each band has its own oscillator model (Analog or Wavetable),
  gain, pan, pitch offset (±24 st), on/off, and *(new in v1.5)* its own
  optional multimode filter (LP/HP/BP/Notch + cutoff + resonance), applied
  to that band's isolated signal after crossover and before merge — so you
  can low-pass the sub, band-pass a resonant mid growl, and high-pass the
  air independently. Each band also has *(new in v1.6)* an independent
  drive/saturation knob — separate from its filter — applied to the
  oscillator before the crossover, so driving the Body band adds harmonic
  weight that the crossover then keeps in the sub region. Verified: the three bands
  reconstruct to within 0.01% of unity energy across a full 20Hz–20kHz
  sweep — that's the LR4 phase-coherence property, the reason this split
  doesn't carve notches at the crossover points. Crossovers auto-enforce a
  minimum one-octave separation. (When 3-Band Mode is off, the voice uses
  the original single-engine path — nothing changes for existing patches.)
- **AM Injector** *(new in v1.7)*: cross-band amplitude modulation — the
  Body band modulates the amplitude of the Character and/or Air bands,
  producing sidebands locked to the low-band content (the metallic,
  vocal-ish growl in aggressive bass). True AM form (carrier × (1 + depth ×
  modulator)) so Amount=0 is the dry carrier and it dials in smoothly.
  Target selects Character, Air, or both. This is the first genuinely
  cross-band feature, so it lives at the voice level rather than inside a
  band — per-band processing stays in the band, cross-band routing in the
  voice.
- **Unison**: 1–8 voices, detune, stereo spread.
- **Multimode filter**: TPT state-variable design (LP/HP/BP/Notch from one
  clean topology), resonance, pre-filter drive/saturation, envelope amount,
  key tracking.
- **Two ADSRs**: amp envelope + a second envelope routable to filter cutoff.
- **One LFO per voice**: Sine/Triangle/Square/Saw/Sample & Hold, routable to
  pitch, filter cutoff, or pulse width.
- **Effects**: chorus (JUCE's own `dsp::Chorus`, rate/depth/feedback/
  mix) and a stereo feedback delay with gentle cross-feed, both on the
  master bus, both bypassable.
- **Voicing** *(new)*: Poly/Mono mode switch, glide (portamento, 0-500ms),
  and Legato. Mono mode is hand-rolled — not `juce::Synthesiser`'s default
  voice allocation, which doesn't do last-note-priority or portamento. It
  tracks a held-note stack directly: new note while one's held glides to
  it (retriggering the envelopes unless Legato is on); releasing back to
  an earlier held note glides back to it. Releasing everything ends the
  note normally.
- **Output stage**: drive/soft-clip + gain.
- Full host automation via `AudioProcessorValueTreeState` — every knob you
  see in the UI is automatable in Logic.
- Save/load plugin state (presets) via Logic's built-in patch save.

## What's NOT in v2.0 (planned next)

The original feature list you shared was a 3-band (Body/Character/Air)
flagship architecture with 9 oscillator engines, 6 filter circuit
emulations, 12 effects, a 33-source mod matrix, multiband compression, and
a 4-band EQ. That's not a single build — it's a roadmap. v1.4 added the
3-band split; v1.5 added per-band filters; v1.6 added per-band drive; v1.7
added the AM injector; v1.8 added the FM oscillator engine; v1.9 adds
per-band FM, a full visual redesign (custom dark/amber theme, arc-style
rotary knobs, branded header), and the Bass Professor product identity.
The remaining big pieces (mod matrix, multiband compression, more filter
models) still slot into this structure cleanly.

## What this is

**Bass Professor is an instrument (synth), not an effect** — it generates sound
from incoming MIDI, like Ancient Synth. The project is configured accordingly:
`IS_SYNTH`, AU main type `aumu` (music device), VST3 category Instrument|Synth.

Builds as **AU, VST3, and Standalone** for macOS, on **JUCE 8.0.7**. Verified:
the full source compiles clean under JUCE 8.0.7 and both validation suites
(DSP smoke test + AddressSanitizer integration test) pass against it.

## Setup — two ways to build, matching your existing workflow

### Option A: Projucer (`BassProfessor.jucer`)
Modules are set to **global path** (`useGlobalPath="1"`), same as your Ancient
Voices / Ancient Synth projects, so it picks up your configured JUCE 8 modules
automatically. Open `BassProfessor.jucer`, **Save and Open in IDE**, build.
The Debug/Release configs have the binary-copy step enabled, so on build the
AU lands in `~/Library/Audio/Plug-Ins/Components` and the VST3 in
`~/Library/Audio/Plug-Ins/VST3` automatically.

### Option B: CMake (`CMakeLists.txt`)
Modeled on your Ancient Synth CMake — same `JUCE_PATH` indirection, instrument
flags, splash disabled, and warning suppressions. Point it at your JUCE 8
checkout:
```
cmake -B build -DJUCE_PATH=/path/to/JUCE
cmake --build build --target BassProfessor_AU      # or _VST3 / _Standalone / _All
```
If you omit `-DJUCE_PATH`, it looks for a `juce/` folder beside the project and
prints a clear clone command if it's missing.

### After building
In Logic: Settings → Plug-In Manager, rescan. It shows up as **Bass Professor**
by **Tully EDM Vibe** under Instruments (AU). First time, run a
**Validate AU** pass (`auval -v aumu Bpro Tull`) before trusting it in a session.

## Project layout

```
Source/
  Params.h               — all parameter IDs + APVTS layout (single source of truth)
  BassProfessorLookAndFeel.h — custom theme: dark/amber palette, arc knobs, pill toggles
  PluginProcessor.h/cpp   — AudioProcessor: synth setup, FX chain, state save/load
  PluginEditor.h/cpp      — UI (functional grid layout, not styled — your call on look)
  SynthSound.h            — accepts all notes
  SynthVoice.h/cpp        — per-voice signal chain: osc → filter → envelopes → LFO
  DSP/
    Utils.h               — PolyBLEP, unit conversions, soft clip
    DualOscillator.h       — analog dual-oscillator engine
    WavetableOscillator.h   — wavetable engine (procedural band-limited tables, mip-mapped)
    FmOscillator.h          — two-operator phase-modulation FM engine
    SynthBand.h             — one band's oscillator pair + per-band gain/pan/pitch/model/filter/drive
    LinkwitzRiley.h         — 4th-order LR crossover for phase-coherent band splitting
    MultiModeFilter.h      — TPT state-variable filter
    ModLFO.h                — per-voice LFO
    StereoDelay.h            — feedback delay with cross-feed (chorus uses JUCE's built-in dsp::Chorus)
```

Everything is one self-contained class per file — when we add oscillator
model #2, it's a new file implementing the same interface, not a rewrite.

`CMakeLists.txt` is included as a bonus — not required for your Projucer
workflow, but it's what this project was validated with (see Validation
above), and it's a fast way to iterate on DSP logic on Linux/CI if you
ever want that.

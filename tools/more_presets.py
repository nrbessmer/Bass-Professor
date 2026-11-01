# Bass Professor 2.x - extra preset bank (100). Hand-designed recipes with seeded variations.
# Output: C++ initializer lines appended to Presets.cpp (same format as gen_presets.py).
import random
random.seed(20261031)

BASE = dict(polyphonyMode=1, glideTimeMs=0, ampAttack=0.003, ampDecay=0.3, ampSustain=0.85, ampRelease=0.15,
            filterType=0, filterCutoff=900, filterResonance=0.9, masterDrive=0.15, oscOctave=-1, oscModel=0)

def P(name, cat, desc, **kv):
    d = dict(BASE); d.update(kv); return (name, cat, desc, d)

def vary(v, pct):
    return v * (1 + random.uniform(-pct, pct))

out = []
# --- each recipe: (category, [(name, desc, params)]) built with small seeded variations
def series(cat, names_descs, make):
    for i, (n, ds) in enumerate(names_descs):
        out.append(P(n, cat, ds, **make(i)))

# 1 Psy Rolling (tight, short decay, filter envelope pluck)
series("Psy Rolling", [
  ("Rolling Psy Classic", "Tight 1/16 rolling psy bass: short saw pluck with a quick filter snap. Sits between kicks."),
  ("Rolling Psy Round", "Rounder rolling bass with less snap; fat low end for progressive psy."),
  ("Rolling Psy Knock", "Square-based roll with an FM knock on every note. Cuts through dense leads."),
  ("Rolling Psy Squelch", "Resonant roll with a wet squelch at the top of each note."),
  ("Rolling Psy Hollow", "Hollow pulse roll; leaves room for a busy kick."),
  ("Rolling Psy Night", "Dark, low-cut-off roll for night-time psy."),
], lambda i: dict(oscWaveA=[0,0,1,0,1,0][i], ampAttack=0.001, ampDecay=vary(0.11,0.15), ampSustain=0.0, ampRelease=0.04,
                  filterCutoff=vary([320,420,380,300,500,240][i],0.1), filterResonance=[1.8,1.2,1.5,4.5,1.0,1.6][i],
                  filterEnvAmount=[0.55,0.35,0.5,0.7,0.4,0.45][i], modDecay=vary(0.08,0.2), modSustain=0.0,
                  filterDrive=0.35, masterDrive=0.25, pulseWidth=[0.5,0.5,0.5,0.5,0.32,0.5][i],
                  oscModel=[0,0,2,0,0,0][i], fmRatio=2.0, fmIndex=[1,1,2.2,1,1,1][i]))

# 2 Full-On / Gallop
series("Full-On", [
  ("Full-On Gallop", "Bright triplet-gallop bass with a pronounced bite."),
  ("Full-On Boing", "Octave 'boing' bass: fast envelope with pitch-y FM."),
  ("Full-On Twang", "Twangy FM gallop bass for morning full-on."),
  ("Full-On Drive", "Driven saw gallop; loud and forward."),
], lambda i: dict(oscWaveA=0, ampAttack=0.001, ampDecay=vary(0.14,0.15), ampSustain=0.0, ampRelease=0.05,
                  filterCutoff=vary([600,520,700,550][i],0.1), filterResonance=[2.0,2.8,1.6,1.4][i], filterEnvAmount=[0.6,0.7,0.5,0.55][i],
                  modDecay=vary(0.1,0.2), oscModel=[0,2,2,0][i], fmRatio=[2,1,3,2][i], fmIndex=[1,3.5,2.8,1][i], fmFeedback=[0,0.2,0.1,0][i],
                  masterDrive=[0.25,0.3,0.25,0.55][i], filterDrive=0.4))

# 3 Dark / Forest psy
series("Dark Psy", [
  ("Forest Floor", "Low, gritty forest bass with a short metallic tick."),
  ("Night Crawler", "Detuned saw roll with a creepy resonant edge."),
  ("Hi-Tech Tick", "Very short 180+ BPM hi-tech bass: almost all transient."),
  ("Swamp Gas", "Wet, resonant swamp roll with a slow LFO on the filter."),
], lambda i: dict(oscWaveA=0, oscBGain=[0.4,0.6,0,0.3][i], oscWaveB=0, oscBPitch=[0.1,0.18,0,-0.12][i], ampAttack=0.001,
                  ampDecay=[0.1,0.12,0.06,0.13][i], ampSustain=0.0, ampRelease=0.03, filterCutoff=[260,340,700,300][i],
                  filterResonance=[2.5,3.5,1.2,5.0][i], filterEnvAmount=[0.5,0.6,0.8,0.5][i], modDecay=[0.07,0.09,0.04,0.1][i],
                  masterDrive=0.45, filterDrive=0.5, lfoDestination=[0,0,0,2][i], lfoRate=0.3, lfoDepth=[0,0,0,0.25][i]))

# 4 Peak-time / Hard techno
series("Hard Techno", [
  ("Warehouse Thud", "Distorted mid-bass thud for 150 BPM hard techno."),
  ("Rave Stab Bass", "Square bass with a rave-style octave layer."),
  ("Industrial Grind", "Grinding FM bass with heavy drive."),
  ("Rumble Tail", "Long, low rumble that fills the space after the kick."),
  ("Pounder", "Short saturated sine pounder; pairs with a big kick."),
], lambda i: dict(oscWaveA=[0,1,0,3,3][i], oscModel=[0,0,2,0,0][i], fmRatio=1.5, fmIndex=[1,1,4,1,1][i], fmFeedback=[0,0,0.35,0,0][i],
                  oscBGain=[0,0.5,0,0,0][i], oscWaveB=1, oscBPitch=[0,12,0,0,0][i],
                  ampDecay=[0.22,0.18,0.3,0.9,0.12][i], ampSustain=[0.2,0.1,0.3,0.4,0][i], ampRelease=[0.1,0.08,0.12,0.6,0.05][i],
                  filterCutoff=[700,900,1100,260,1500][i], filterResonance=[1.2,1.0,1.4,0.8,0.7][i],
                  masterDrive=[0.7,0.5,0.85,0.4,0.75][i], filterDrive=0.6))

# 5 Afro / Organic house
series("Afro House", [
  ("Afro Log Drum", "Woody log-drum style bass: sine with a pitched click and short decay."),
  ("Organic Pluck", "Soft rounded pluck that breathes with percussion."),
  ("Kalimba Low", "FM bell-bass with a gentle attack, organic house style."),
  ("Desert Sub", "Warm sub with slow glide for organic grooves."),
], lambda i: dict(oscModel=[2,0,2,0][i], oscWaveA=[3,2,3,3][i], fmRatio=[3.5,1,4.0,1][i], fmIndex=[2.5,1,1.6,0][i],
                  ampAttack=0.002, ampDecay=[0.25,0.35,0.5,0.6][i], ampSustain=[0.0,0.2,0.0,0.85][i], ampRelease=[0.2,0.25,0.35,0.3][i],
                  modDecay=[0.05,0.2,0.12,0.2][i], filterCutoff=[1500,700,2000,500][i], filterResonance=0.7,
                  glideTimeMs=[0,0,0,90][i], legato=[0,0,0,1][i], masterDrive=0.1))

# 6 Halftime / Neuro additions
series("Halftime", [
  ("Halftime Growl", "Slow wobbling growl for 85 BPM halftime."),
  ("Neuro Snarl", "Comb-like FM snarl with an LFO-driven filter."),
  ("Foghorn", "Huge detuned unison foghorn for drops."),
  ("Robot Talk", "Vowel-ish band-pass talk bass with S&H movement."),
  ("Pressure Reese", "Wide, phasey reese with slow movement."),
], lambda i: dict(oscModel=[2,2,0,0,0][i], fmRatio=[1.0,2.01,1,1,1][i], fmIndex=[5,7,1,1,1][i], fmFeedback=[0.5,0.6,0,0,0][i],
                  oscWaveA=0, oscBGain=[0,0,0.8,0.5,0.9][i], oscWaveB=0, oscBPitch=[0,0,0.2,7,0.12][i],
                  unisonVoices=[1,1,6,1,4][i], unisonDetune=[10,10,28,10,18][i], unisonSpread=0.8,
                  ampSustain=0.9, ampDecay=0.4, ampRelease=0.2, filterType=[0,0,0,2,0][i],
                  filterCutoff=[900,1400,1200,800,700][i], filterResonance=[2.5,3.5,1.0,4.0,1.4][i],
                  lfoDestination=2, lfoShape=[0,1,0,4,0][i], lfoRate=[2.0,4.0,0.5,6.0,0.25][i], lfoDepth=[0.6,0.5,0.15,0.55,0.3][i],
                  masterDrive=[0.6,0.7,0.5,0.45,0.4][i], filterDrive=0.6))

# 7 808 / Phonk / Drill
series("808 & Phonk", [
  ("Phonk Cowbell 808", "Distorted phonk 808 with a long tail and glide."),
  ("Drill Slide", "UK drill 808 with a fast pitch slide between notes."),
  ("Clean Long 808", "Clean, long 808 with a soft click."),
  ("Memphis Boom", "Saturated Memphis-style boom with short tail."),
  ("Plugg Bounce", "Bouncy, rounded 808 for plugg / rage beats."),
], lambda i: dict(oscWaveA=3, oscOctave=-1, ampAttack=0.001, ampDecay=[1.4,1.0,1.8,0.6,0.9][i], ampSustain=[0.3,0.4,0.2,0.0,0.3][i],
                  ampRelease=[0.5,0.4,0.7,0.2,0.3][i], glideTimeMs=[60,35,40,0,50][i], legato=[1,1,1,0,1][i],
                  filterCutoff=[1800,1500,800,2400,1100][i], filterResonance=0.7,
                  masterDrive=[0.85,0.55,0.15,0.9,0.4][i], filterDrive=[0.6,0.4,0.1,0.7,0.3][i]))

# 8 Funk / slap / Moog-ish
series("Funk & Soul", [
  ("Slap Synth", "Plucky synth slap with a quick envelope pop."),
  ("Talkbox Bass", "Band-pass vowel bass with envelope movement."),
  ("Minimoog-ish Fat", "Fat ladder-style low pass on two detuned saws."),
  ("Wah Pluck", "Envelope-filter 'wah' bass for funk and disco."),
  ("P-Funk Square", "Square bass with resonant envelope and a little glide."),
], lambda i: dict(oscWaveA=[0,0,0,0,1][i], oscBGain=[0.3,0.4,0.7,0.2,0.3][i], oscWaveB=0, oscBPitch=[0,7,-0.08,0,12][i],
                  ampAttack=0.001, ampDecay=[0.2,0.35,0.5,0.3,0.4][i], ampSustain=[0.2,0.5,0.7,0.3,0.6][i], ampRelease=0.12,
                  filterType=[0,2,0,0,0][i], filterCutoff=[500,700,600,350,450][i], filterResonance=[2.0,4.5,1.5,5.0,3.0][i],
                  filterEnvAmount=[0.7,0.5,0.4,0.85,0.6][i], modDecay=[0.08,0.25,0.3,0.18,0.15][i], modSustain=[0,0.2,0.1,0,0.1][i],
                  glideTimeMs=[0,0,25,0,40][i], masterDrive=0.25))

# 9 Synth-pop / 80s / Italo
series("Synth Pop", [
  ("Italo Octaves", "Bouncy octave bass for italo and synth-pop (play staccato 8ths)."),
  ("Eighties Pulse", "Thin, bright pulse bass with chorus."),
  ("New Wave Saw", "Punchy new-wave saw bass with a short decay."),
  ("Chorus Glass", "Glassy FM bass through chorus."),
], lambda i: dict(oscWaveA=[0,1,0,3][i], oscModel=[0,0,0,2][i], fmRatio=2.0, fmIndex=[1,1,1,2.0][i], oscBGain=[0.6,0,0,0][i], oscWaveB=0, oscBPitch=[12,0,0,0][i],
                  pulseWidth=[0.5,0.25,0.5,0.5][i], oscOctave=0, ampDecay=[0.25,0.35,0.22,0.5][i], ampSustain=[0.3,0.5,0.2,0.3][i],
                  filterCutoff=[1400,1800,1200,2500][i], filterResonance=1.2, filterEnvAmount=0.35, modDecay=0.2,
                  chorusEnable=[0,1,0,1][i], chorusMix=0.4, chorusDepth=0.45, chorusRate=0.6, masterDrive=0.15))

# 10 Dub / Roots / Steppers
series("Dub & Steppers", [
  ("Steppers Sine", "Round steppers sub with weight and almost no top."),
  ("Roots Warm", "Warm, slightly overdriven roots bass."),
  ("Dub Siren Low", "Low bass with a slow pitch LFO for dubby effects."),
  ("Echo Chamber Bass", "Bass with a dotted delay for dub breakdowns."),
], lambda i: dict(oscWaveA=[3,2,3,2][i], ampDecay=0.6, ampSustain=0.8, ampRelease=0.3, filterCutoff=[300,500,350,450][i],
                  filterResonance=0.8, masterDrive=[0.1,0.35,0.15,0.2][i], lfoDestination=[0,0,1,0][i], lfoRate=0.2, lfoDepth=[0,0,0.2,0][i],
                  delayEnable=[0,0,0,1][i], delayTimeMs=375, delayFeedback=0.45, delayMix=0.25))

# 11 Garage / Bassline / Speed garage
series("Bassline House", [
  ("Niche Organ Bass", "Organ-like bassline house wobble with an octave layer."),
  ("Speed Garage Reese", "Detuned speed-garage reese with filter movement."),
  ("Wobbly Bassline", "Bassline house bass with a 1/8 LFO wobble."),
  ("Two-Step Sub", "Bouncy UKG sub with a short pluck."),
], lambda i: dict(oscWaveA=[1,0,0,3][i], oscBGain=[0.5,0.8,0.4,0][i], oscWaveB=[1,0,0,1][i], oscBPitch=[12,0.15,0.1,0][i],
                  unisonVoices=[1,3,1,1][i], unisonDetune=20, ampDecay=0.35, ampSustain=[0.7,0.8,0.7,0.2][i],
                  filterCutoff=[1200,900,800,600][i], filterResonance=[1.5,2.0,2.5,0.8][i],
                  lfoDestination=[0,2,2,0][i], lfoRate=[2,0.5,4.0,2][i], lfoDepth=[0,0.3,0.45,0][i], masterDrive=0.35))

# 12 Cinematic / ambient additions
series("Cinematic", [
  ("Drone Floor", "Evolving low drone with slow filter drift (hold long notes)."),
  ("Trailer Hit Low", "Big low boom with a long reverb tail."),
  ("Pulse Tension", "Pulsing tension bass with an S&H shimmer."),
  ("Abyss Pad Bass", "Slow-attack wide bass pad for underscore."),
], lambda i: dict(oscWaveA=[0,3,1,0][i], oscBGain=[0.6,0,0.3,0.7][i], oscWaveB=0, oscBPitch=[0.12,0,7,-0.1][i],
                  ampAttack=[0.6,0.002,0.01,1.2][i], ampDecay=[1,2.5,0.3,1][i], ampSustain=[0.9,0,0.6,0.9][i], ampRelease=[1.5,2.5,0.3,2.0][i],
                  polyphonyMode=[1,1,1,0][i], filterCutoff=[500,900,700,600][i], filterResonance=1.0,
                  lfoDestination=[2,0,2,2][i], lfoShape=[0,0,4,0][i], lfoRate=[0.08,1,5,0.1][i], lfoDepth=[0.3,0,0.25,0.25][i],
                  reverbEnable=[1,1,0,1][i], reverbRoom=0.8, reverbWet=0.3, unisonVoices=[1,1,1,4][i], unisonDetune=15, masterDrive=0.1))

# 13 Three-band showcase (uses the 3-band engine)
series("3-Band Designer", [
  ("Split Sub + Grit", "3-band: clean sine body, driven FM character, airy wavetable top."),
  ("Mono Low, Wide Top", "3-band: mono sub, wide detuned air band for big rooms."),
  ("Talking Mids", "3-band: steady body, band-pass talking character, quiet air."),
  ("Glass & Gravel", "3-band: deep body, gravelly mids, glassy FM air."),
  ("Neuro Stack", "3-band neuro stack: heavy drive on character, filtered air."),
], lambda i: dict(bandsEnable=1, crossoverLow=[120,110,140,100,130][i], crossoverHigh=[1800,2200,1500,2500,1600][i],
                  bodyModel=0, bodyGain=0.9, charModel=[2,0,0,0,2][i], charGain=[0.7,0.6,0.8,0.7,0.8][i], charDrive=[0.6,0.2,0.3,0.7,0.9][i],
                  airModel=[1,1,1,2,1][i], airGain=[0.4,0.6,0.25,0.45,0.35][i], airDrive=[0.3,0.1,0,0.2,0.4][i],
                  fmRatio=2, fmIndex=[3,1,1,4,5][i], wtPosition=[0.4,0.7,0.3,0.6,0.5][i], wtWarp=0.6,
                  ampSustain=0.85, filterCutoff=8000, filterResonance=0.7, masterDrive=0.3))

# 14 Pluck / staccato
series("Plucks", [
  ("Rubber Pluck", "Rubbery short pluck with a bouncy filter."),
  ("Wood Pluck", "Muted, woody pluck for melodic house."),
  ("Glass Pluck", "Bright FM glass pluck an octave up."),
  ("Pizz Bass", "Pizzicato-style short bass for cinematic ostinatos."),
], lambda i: dict(oscWaveA=[0,2,3,0][i], oscModel=[0,0,2,0][i], fmRatio=3.0, fmIndex=[1,1,2.5,1][i], oscOctave=[-1,-1,0,-1][i],
                  ampAttack=0.001, ampDecay=[0.18,0.22,0.3,0.15][i], ampSustain=0, ampRelease=[0.1,0.12,0.2,0.08][i],
                  filterCutoff=[400,600,2500,700][i], filterResonance=[3.0,1.0,0.8,1.2][i], filterEnvAmount=[0.75,0.4,0.2,0.5][i],
                  modDecay=[0.12,0.1,0.15,0.08][i], masterDrive=0.15))

# 15 Wobble / Riddim additions
series("Wobble", [
  ("Classic Wobble 1/4", "Classic dubstep wobble (LFO on the filter, sync it by ear)."),
  ("Fast Yoi", "Fast vowel-like 'yoi' wobble with resonance."),
  ("Riddim Square Chop", "Square riddim bass with a stepped S&H filter."),
  ("Lazy Wub", "Slow lazy wub for halftime breaks."),
], lambda i: dict(oscWaveA=[0,0,1,0][i], oscBGain=0.5, oscWaveB=1, oscBPitch=[-12,0,0,-12][i], ampSustain=0.95, ampDecay=0.5,
                  filterCutoff=[700,900,800,600][i], filterResonance=[2.5,6.0,2.0,2.0][i],
                  lfoDestination=2, lfoShape=[0,1,4,0][i], lfoRate=[2.0,6.0,8.0,0.8][i], lfoDepth=[0.7,0.6,0.55,0.7][i],
                  masterDrive=[0.6,0.6,0.7,0.5][i], filterDrive=0.6))

# 16 Jazz / acoustic-ish
series("Upright & Jazz", [
  ("Upright-ish", "Synth approximation of an upright: soft attack thump and woody decay."),
  ("Fretless Glide", "Fretless-style singing bass with glide and slow vibrato."),
  ("Walking Warm", "Warm, even walking bass for jazz and lo-fi."),
], lambda i: dict(oscWaveA=[2,3,2][i], oscModel=[2,0,0][i], fmRatio=1.0, fmIndex=[0.8,1,1][i], ampAttack=[0.004,0.02,0.005][i],
                  ampDecay=[0.6,1.2,0.8][i], ampSustain=[0.2,0.7,0.4][i], ampRelease=0.25, filterCutoff=[700,900,800][i],
                  filterResonance=0.8, glideTimeMs=[0,110,0][i], legato=[0,1,0][i],
                  lfoDestination=[0,1,0][i], lfoRate=5.0, lfoDepth=[0,0.05,0][i], masterDrive=0.1))

# 17 Trance additions
series("Trance Rolling", [
  ("Offbeat Trance Saw", "Classic offbeat trance bass: saw, short, punchy."),
  ("Uplifting Roll", "Rolling 1/16 trance bass with a soft pluck."),
  ("Tech Trance Drive", "Driven, darker tech-trance roll."),
], lambda i: dict(oscWaveA=0, oscBGain=0.4, oscWaveB=0, oscBPitch=0.08, ampDecay=[0.16,0.12,0.14][i], ampSustain=[0.1,0,0.05][i], ampRelease=0.06,
                  filterCutoff=[900,700,500][i], filterResonance=[1.2,1.0,1.8][i], filterEnvAmount=[0.4,0.45,0.5][i], modDecay=0.1,
                  masterDrive=[0.2,0.2,0.5][i], filterDrive=0.4))

# 18 Experimental
series("Experimental", [
  ("Broken Radio", "Noisy, band-passed FM chatter - use sparingly."),
  ("Metal Spring", "High FM feedback metallic spring bass."),
  ("Bitter Sweet", "Detuned square duo with a sour fifth."),
  ("Shapeshifter", "Wavetable sweep driven by the LFO; never sounds the same twice."),
], lambda i: dict(oscModel=[2,2,0,1][i], fmRatio=[7.0,3.5,1,1][i], fmIndex=[6,4,1,1][i], fmFeedback=[0.8,0.9,0,0][i],
                  oscWaveA=1, oscBGain=[0,0,0.7,0][i], oscWaveB=1, oscBPitch=[0,0,7.02,0][i],
                  wtPosition=0.3, wtWarp=0.7, filterType=[2,0,0,0][i], filterCutoff=[1200,1500,900,1200][i], filterResonance=[3.0,1.5,1.2,1.5][i],
                  lfoDestination=[2,1,0,2][i], lfoShape=[4,0,0,1][i], lfoRate=[9,7,1,0.4][i], lfoDepth=[0.4,0.02,0,0.5][i], masterDrive=0.45))

# pad up to 100 with tasteful variations of the strongest recipes
pool = [o for o in out if o[1] in ("Psy Rolling","Hard Techno","808 & Phonk","Halftime","3-Band Designer","Funk & Soul")]
k = 0
while len(out) < 100:
    n, c, d, p = pool[k % len(pool)]; k += 1
    q = dict(p)
    q["filterCutoff"] = max(80, q["filterCutoff"] * random.uniform(0.8, 1.25))
    q["masterDrive"] = min(0.95, max(0, q["masterDrive"] + random.uniform(-0.1, 0.12)))
    q["ampDecay"] = max(0.03, q["ampDecay"] * random.uniform(0.85, 1.2))
    out.append((n + " II" if not n.endswith(" II") else n + "I", c, d.split(".")[0] + " - alternate voicing.", q))

order = []
for o in out:
    order.append(o)
lines = []
for n, c, d, p in order:
    kv = ", ".join('{"%s", %.4ff}' % (k2, float(v)) for k2, v in p.items())
    lines.append('        { "%s", "%s", "%s", {%s} },' % (n, c, d.replace('"', "'"), kv))
open("/home/claude/bp/tools/extra_presets.inc", "w").write("\n".join(lines) + "\n")
print(len(lines), "presets;", len(set(c for _, c, _, _ in order)), "categories")
from collections import Counter
print(Counter(c for _, c, _, _ in order))

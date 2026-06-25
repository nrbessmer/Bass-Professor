#!/usr/bin/env python3
"""Generate 200 genre presets for Bass Professor as a C++ data file.

Each genre family has a hand-designed base template capturing the params that
define that sound; variants tweak the sound-shaping params within musically
safe ranges. Values are real (un-normalized); the C++ side converts via the
parameter range. Out-of-range values are clamped here as a safety net.
"""
import random

random.seed(20260623)  # reproducible

# Parameter ranges for clamping (id -> (min, max)). Choice/bool use index range.
RANGES = {
    "oscModel": (0, 2), "oscWaveA": (0, 3), "oscWaveB": (0, 4), "oscBGain": (0, 1),
    "oscBPitch": (-12, 12), "hardSync": (0, 1), "pulseWidth": (0.05, 0.95),
    "oscOctave": (-3, 3), "oscSemitone": (-12, 12), "oscFineCents": (-100, 100),
    "unisonVoices": (1, 8), "unisonDetune": (0, 50), "unisonSpread": (0, 1),
    "amAmount": (0, 1), "amTarget": (0, 3), "wtPosition": (0, 1), "wtWarp": (0, 1),
    "fmRatio": (0.5, 12), "fmIndex": (0, 12), "fmFeedback": (0, 1),
    "bandsEnable": (0, 1), "crossoverLow": (20, 5000), "crossoverHigh": (100, 20000),
    "bodyModel": (0, 2), "bodyGain": (0, 1), "bodyPan": (-1, 1), "bodyPitch": (-24, 24),
    "bodyActive": (0, 1), "bodyFilterOn": (0, 1), "bodyFilterType": (0, 3),
    "bodyFilterCut": (20, 20000), "bodyFilterRes": (0.1, 10), "bodyDrive": (0, 1),
    "charModel": (0, 2), "charGain": (0, 1), "charPan": (-1, 1), "charPitch": (-24, 24),
    "charActive": (0, 1), "charFilterOn": (0, 1), "charFilterType": (0, 3),
    "charFilterCut": (20, 20000), "charFilterRes": (0.1, 10), "charDrive": (0, 1),
    "airModel": (0, 2), "airGain": (0, 1), "airPan": (-1, 1), "airPitch": (-24, 24),
    "airActive": (0, 1), "airFilterOn": (0, 1), "airFilterType": (0, 3),
    "airFilterCut": (20, 20000), "airFilterRes": (0.1, 10), "airDrive": (0, 1),
    "filterType": (0, 3), "filterCutoff": (20, 20000), "filterResonance": (0.1, 10),
    "filterDrive": (0, 1), "filterEnvAmount": (-1, 1), "filterKeyTrack": (0, 1),
    "ampAttack": (0.001, 5), "ampDecay": (0.001, 5), "ampSustain": (0, 1), "ampRelease": (0.001, 5),
    "modAttack": (0.001, 5), "modDecay": (0.001, 5), "modSustain": (0, 1), "modRelease": (0.001, 5),
    "lfoShape": (0, 4), "lfoRate": (0.01, 20), "lfoDepth": (0, 1), "lfoDestination": (0, 3),
    "masterDrive": (0, 1), "masterGain": (-24, 6),
    "chorusEnable": (0, 1), "chorusRate": (0.05, 5), "chorusDepth": (0, 1),
    "chorusFeedback": (-0.9, 0.9), "chorusMix": (0, 1),
    "delayEnable": (0, 1), "delayTimeMs": (1, 1000), "delayFeedback": (0, 0.95), "delayMix": (0, 1),
    "polyphonyMode": (0, 1), "glideTimeMs": (0, 500), "legato": (0, 1),
}

def clamp(pid, v):
    lo, hi = RANGES.get(pid, (-1e9, 1e9))
    return max(lo, min(hi, v))

def jitter(v, amt):
    return v + random.uniform(-amt, amt)

presets = []  # (name, category, {param: value})

def add(name, category, params):
    cleaned = {k: clamp(k, float(v)) for k, v in params.items()}
    presets.append((name, category, cleaned))

# Convenience waveform/model indices
SAW, SQR, TRI, SINE = 0, 1, 2, 3
NOISE = 4
ANALOG, WT, FM = 0, 1, 2
LP, HP, BP, NOTCH = 0, 1, 2, 3
MONO, POLY = 1, 0

# ---------------------------------------------------------------------------
# Genre family generators. Each returns a list of (name, params) variants.
# ---------------------------------------------------------------------------

def family(cat, count, base, namer, vary):
    for i in range(count):
        p = dict(base)
        vary(p, i)
        add(namer(i), cat, p)

# --- Sub Bass (foundation) ---
family("Sub Bass", 6,
    dict(oscModel=ANALOG, oscWaveA=SINE, oscBGain=0.0, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=18, ampAttack=0.004, ampDecay=0.3,
         ampSustain=0.95, ampRelease=0.16, filterType=LP, filterCutoff=600,
         filterResonance=0.5, masterDrive=0.08),
    lambda i: f"Pure Sub {i+1}",
    lambda p, i: (p.update(oscWaveA=SINE if i % 2 else TRI,
                           oscOctave=-1 if i < 4 else -2,
                           filterCutoff=jitter(550, 200),
                           masterDrive=0.05 + 0.04 * (i % 3))))

# --- Deep House ---
family("Deep House", 5,
    dict(oscModel=ANALOG, oscWaveA=TRI, oscWaveB=SINE, oscBGain=0.35, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=28, ampAttack=0.006, ampDecay=0.35,
         ampSustain=0.85, ampRelease=0.22, filterType=LP, filterCutoff=760,
         filterResonance=0.8, masterDrive=0.12),
    lambda i: f"Deep House {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(720, 180),
                           filterResonance=jitter(0.9, 0.3),
                           oscBGain=jitter(0.35, 0.15),
                           glideTimeMs=jitter(28, 12))))

# --- Tech House ---
family("Tech House", 5,
    dict(oscModel=ANALOG, oscWaveA=SAW, oscOctave=-1, polyphonyMode=MONO, glideTimeMs=10,
         ampAttack=0.003, ampDecay=0.18, ampSustain=0.5, ampRelease=0.12,
         filterType=LP, filterCutoff=620, filterResonance=1.1, filterEnvAmount=0.45,
         modAttack=0.002, modDecay=0.2, modSustain=0.0, masterDrive=0.2),
    lambda i: f"Tech House {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(600, 150),
                           filterEnvAmount=jitter(0.45, 0.2),
                           modDecay=jitter(0.2, 0.08),
                           masterDrive=jitter(0.2, 0.1))))

# --- Bass House ---
family("Bass House", 6,
    dict(oscModel=WT, wtPosition=0.4, wtWarp=0.5, oscOctave=-1, polyphonyMode=MONO,
         glideTimeMs=8, ampAttack=0.002, ampDecay=0.2, ampSustain=0.6, ampRelease=0.12,
         filterType=LP, filterCutoff=900, filterResonance=2.5, filterDrive=0.6,
         masterDrive=0.5),
    lambda i: f"Bass House {i+1}",
    lambda p, i: (p.update(wtPosition=jitter(0.4, 0.3),
                           filterResonance=jitter(2.5, 1.0),
                           masterDrive=jitter(0.5, 0.2),
                           filterDrive=jitter(0.6, 0.2))))

# --- Progressive House ---
family("Prog House", 5,
    dict(oscModel=ANALOG, oscWaveA=SAW, oscOctave=-1, unisonVoices=3, unisonDetune=12,
         unisonSpread=0.5, polyphonyMode=MONO, glideTimeMs=20, ampAttack=0.008,
         ampDecay=0.3, ampSustain=0.8, ampRelease=0.25, filterType=LP, filterCutoff=1000,
         filterResonance=0.8, lfoShape=0, lfoRate=0.3, lfoDepth=0.15, lfoDestination=2,
         masterDrive=0.12),
    lambda i: f"Prog House {i+1}",
    lambda p, i: (p.update(unisonDetune=jitter(12, 6), filterCutoff=jitter(1000, 250),
                           lfoRate=jitter(0.3, 0.15))))

# --- Future House ---
family("Future House", 5,
    dict(oscModel=FM, fmRatio=2.0, fmIndex=2.5, fmFeedback=0.2, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=10, ampAttack=0.002, ampDecay=0.18,
         ampSustain=0.4, ampRelease=0.12, filterType=LP, filterCutoff=1100,
         filterResonance=1.2, filterEnvAmount=0.4, modDecay=0.18, masterDrive=0.25),
    lambda i: f"Future House {i+1}",
    lambda p, i: (p.update(fmRatio=[1.0,2.0,3.0,2.0,4.0,1.5][i],
                           fmIndex=jitter(2.5, 1.0),
                           filterEnvAmount=jitter(0.4, 0.2))))

# --- Detroit / Melodic Techno ---
family("Melodic Techno", 5,
    dict(oscModel=ANALOG, oscWaveA=SAW, oscBGain=0.25, oscBPitch=-12, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=24, ampAttack=0.006, ampDecay=0.4,
         ampSustain=0.75, ampRelease=0.3, filterType=LP, filterCutoff=720,
         filterResonance=0.9, lfoShape=0, lfoRate=0.2, lfoDepth=0.2, lfoDestination=2,
         masterDrive=0.15),
    lambda i: f"Melodic Techno {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(700, 180), lfoRate=jitter(0.2, 0.1),
                           lfoDepth=jitter(0.2, 0.1), glideTimeMs=jitter(24, 10))))

# --- Dub Techno ---
family("Dub Techno", 5,
    dict(oscModel=ANALOG, oscWaveA=TRI, oscBGain=0.3, oscOctave=-1, polyphonyMode=MONO,
         glideTimeMs=30, ampAttack=0.02, ampDecay=0.5, ampSustain=0.7, ampRelease=0.5,
         filterType=LP, filterCutoff=340, filterResonance=0.6, masterDrive=0.1,
         chorusEnable=1, chorusRate=0.4, chorusDepth=0.4, chorusMix=0.35,
         delayEnable=1, delayTimeMs=375, delayFeedback=0.45, delayMix=0.3),
    lambda i: f"Dub Techno {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(330, 80), delayTimeMs=[375,500,250,333,420,300][i],
                           delayFeedback=jitter(0.45, 0.12))))

# --- Acid (303) ---
family("Acid", 8,
    dict(oscModel=ANALOG, oscWaveA=SAW, oscOctave=-1, polyphonyMode=MONO, glideTimeMs=40,
         legato=1, ampAttack=0.002, ampDecay=0.3, ampSustain=0.0, ampRelease=0.1,
         filterType=LP, filterCutoff=500, filterResonance=7.0, filterEnvAmount=0.6,
         filterDrive=0.3, modAttack=0.002, modDecay=0.28, modSustain=0.0, masterDrive=0.25),
    lambda i: f"Acid 303 {i+1}",
    lambda p, i: (p.update(oscWaveA=SAW if i % 2 == 0 else SQR,
                           filterResonance=jitter(7.0, 2.0),
                           filterEnvAmount=jitter(0.6, 0.2),
                           modDecay=jitter(0.28, 0.12),
                           filterCutoff=jitter(480, 180),
                           masterDrive=jitter(0.25, 0.15))))

# --- Industrial Techno ---
family("Industrial Techno", 4,
    dict(oscModel=ANALOG, oscWaveA=SAW, oscWaveB=NOISE, oscBGain=0.15, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=6, ampAttack=0.002, ampDecay=0.25, ampSustain=0.6,
         ampRelease=0.12, filterType=LP, filterCutoff=700, filterResonance=1.5,
         filterDrive=0.7, masterDrive=0.7),
    lambda i: f"Industrial {i+1}",
    lambda p, i: (p.update(masterDrive=jitter(0.7, 0.15), filterDrive=jitter(0.7, 0.15),
                           oscBGain=jitter(0.15, 0.1), filterResonance=jitter(1.5, 0.6))))

# --- Uplifting Trance ---
family("Trance", 5,
    dict(oscModel=ANALOG, oscWaveA=SAW, oscBGain=0.3, oscBPitch=-12, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=0, ampAttack=0.003, ampDecay=0.12, ampSustain=0.0,
         ampRelease=0.08, filterType=LP, filterCutoff=800, filterResonance=0.8, masterDrive=0.18),
    lambda i: f"Trance Roller {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(800, 200), ampRelease=jitter(0.08, 0.03),
                           oscBGain=jitter(0.3, 0.1))))

# --- Psytrance ---
family("Psytrance", 8,
    dict(oscModel=ANALOG, oscWaveA=TRI, oscBGain=0.2, oscOctave=-1, polyphonyMode=MONO,
         glideTimeMs=0, ampAttack=0.002, ampDecay=0.075, ampSustain=0.0, ampRelease=0.05,
         filterType=LP, filterCutoff=620, filterResonance=1.2, filterEnvAmount=0.3,
         modDecay=0.07, modSustain=0.0, masterDrive=0.22),
    lambda i: f"Psy Bass {i+1}",
    lambda p, i: (p.update(ampDecay=jitter(0.075, 0.02), ampRelease=jitter(0.05, 0.015),
                           filterCutoff=jitter(620, 150), oscWaveA=TRI if i % 2 else SAW,
                           filterResonance=jitter(1.2, 0.5), masterDrive=jitter(0.22, 0.08))))

# --- Goa ---
family("Goa", 3,
    dict(oscModel=ANALOG, oscWaveA=SAW, oscOctave=-1, polyphonyMode=MONO, glideTimeMs=10,
         ampAttack=0.002, ampDecay=0.1, ampSustain=0.1, ampRelease=0.08, filterType=LP,
         filterCutoff=700, filterResonance=3.0, filterEnvAmount=0.4, modDecay=0.1, masterDrive=0.2),
    lambda i: f"Goa Bass {i+1}",
    lambda p, i: (p.update(filterResonance=jitter(3.0, 1.0), filterCutoff=jitter(700, 150))))

# --- Dubstep / Brostep growl ---
family("Dubstep", 8,
    dict(oscModel=FM, fmRatio=1.0, fmIndex=4.0, fmFeedback=0.4, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=5, ampAttack=0.002, ampDecay=0.3, ampSustain=0.9,
         ampRelease=0.1, filterType=LP, filterCutoff=900, filterResonance=2.0,
         filterDrive=0.5, lfoShape=2, lfoRate=4.0, lfoDepth=0.7, lfoDestination=2,
         amAmount=0.3, amTarget=1, masterDrive=0.5),
    lambda i: f"Dubstep Growl {i+1}",
    lambda p, i: (p.update(lfoShape=[2,1,3,0,2,1,2,3,0,2][i],
                           lfoRate=[2.0,4.0,6.0,8.0,3.0,5.0,1.5,7.0,4.0,2.5][i],
                           fmIndex=jitter(4.0, 1.5), lfoDepth=jitter(0.7, 0.15),
                           masterDrive=jitter(0.5, 0.15))))

# --- Riddim ---
family("Riddim", 5,
    dict(oscModel=WT, wtPosition=0.6, wtWarp=0.5, oscOctave=-1, polyphonyMode=MONO,
         glideTimeMs=4, ampAttack=0.002, ampDecay=0.3, ampSustain=0.9, ampRelease=0.08,
         filterType=LP, filterCutoff=800, filterResonance=2.2, filterDrive=0.6,
         lfoShape=2, lfoRate=2.0, lfoDepth=0.8, lfoDestination=2, masterDrive=0.55),
    lambda i: f"Riddim {i+1}",
    lambda p, i: (p.update(lfoRate=[1.0,2.0,1.5,3.0,2.5,4.0][i], wtPosition=jitter(0.6, 0.25),
                           masterDrive=jitter(0.55, 0.15))))

# --- Neuro DnB ---
family("Neuro DnB", 8,
    dict(oscModel=FM, fmRatio=2.0, fmIndex=5.0, fmFeedback=0.5, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=4, ampAttack=0.002, ampDecay=0.35, ampSustain=0.85,
         ampRelease=0.1, bandsEnable=1, crossoverLow=180, crossoverHigh=1800,
         bodyModel=ANALOG, bodyGain=1.0, charModel=FM, charGain=0.9, charDrive=0.5,
         airModel=WT, airGain=0.7, airDrive=0.3, filterType=LP, filterCutoff=2000,
         filterResonance=1.5, lfoShape=4, lfoRate=6.0, lfoDepth=0.5, lfoDestination=2,
         amAmount=0.4, amTarget=3, masterDrive=0.45),
    lambda i: f"Neuro {i+1}",
    lambda p, i: (p.update(fmIndex=jitter(5.0, 1.5), lfoRate=[4.0,6.0,8.0,3.0,5.0,7.0,2.0,6.0,4.0,5.0][i],
                           amAmount=jitter(0.4, 0.2), charDrive=jitter(0.5, 0.2),
                           masterDrive=jitter(0.45, 0.12))))

# --- Liquid DnB ---
family("Liquid DnB", 5,
    dict(oscModel=ANALOG, oscWaveA=SINE, oscWaveB=TRI, oscBGain=0.3, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=20, ampAttack=0.006, ampDecay=0.3, ampSustain=0.85,
         ampRelease=0.2, filterType=LP, filterCutoff=520, filterResonance=0.6, masterDrive=0.1),
    lambda i: f"Liquid Sub {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(520, 140), oscBGain=jitter(0.3, 0.12))))

# --- Jump Up DnB ---
family("Jump Up DnB", 5,
    dict(oscModel=WT, wtPosition=0.5, oscOctave=-1, polyphonyMode=MONO, glideTimeMs=6,
         ampAttack=0.002, ampDecay=0.25, ampSustain=0.8, ampRelease=0.1, filterType=LP,
         filterCutoff=1000, filterResonance=2.0, filterDrive=0.5, lfoShape=1, lfoRate=3.0,
         lfoDepth=0.5, lfoDestination=2, masterDrive=0.5),
    lambda i: f"Jump Up {i+1}",
    lambda p, i: (p.update(lfoRate=jitter(3.0, 1.0), wtPosition=jitter(0.5, 0.25),
                           masterDrive=jitter(0.5, 0.15))))

# --- Reese ---
family("Reese", 5,
    dict(oscModel=ANALOG, oscWaveA=SAW, oscOctave=-1, unisonVoices=5, unisonDetune=28,
         unisonSpread=0.7, polyphonyMode=MONO, glideTimeMs=10, ampAttack=0.004, ampDecay=0.3,
         ampSustain=0.9, ampRelease=0.15, filterType=LP, filterCutoff=1200, filterResonance=1.0,
         lfoShape=0, lfoRate=0.5, lfoDepth=0.25, lfoDestination=2, masterDrive=0.3,
         chorusEnable=1, chorusRate=0.5, chorusDepth=0.4, chorusMix=0.3),
    lambda i: f"Reese {i+1}",
    lambda p, i: (p.update(unisonDetune=[18,24,28,34,40,22,30,36][i],
                           unisonVoices=[4,5,6,7,5,6,4,7][i],
                           filterCutoff=jitter(1200, 300), masterDrive=jitter(0.3, 0.12))))

# --- Trap 808 ---
family("Trap 808", 8,
    dict(oscModel=ANALOG, oscWaveA=SINE, oscBGain=0.0, oscOctave=-2, polyphonyMode=MONO,
         glideTimeMs=70, ampAttack=0.003, ampDecay=0.6, ampSustain=0.6, ampRelease=1.2,
         filterType=LP, filterCutoff=500, filterResonance=0.4, masterDrive=0.1),
    lambda i: f"808 {['Clean','Boom','Distorted','Long','Punch','Sub','Glide','Hard','Round','Deep'][i]}",
    lambda p, i: (p.update(glideTimeMs=[40,70,60,90,30,80,120,50,70,100][i],
                           ampRelease=[0.8,1.2,1.0,2.0,0.6,1.5,1.2,0.9,1.4,2.5][i],
                           masterDrive=[0.05,0.1,0.45,0.1,0.2,0.08,0.1,0.6,0.12,0.08][i],
                           oscOctave=-2 if i != 4 else -1)))

# --- Future Bass ---
family("Future Bass", 5,
    dict(oscModel=WT, wtPosition=0.3, wtWarp=0.5, oscOctave=-1, unisonVoices=4, unisonDetune=20,
         unisonSpread=0.6, polyphonyMode=MONO, glideTimeMs=8, ampAttack=0.004, ampDecay=0.3,
         ampSustain=0.8, ampRelease=0.18, filterType=LP, filterCutoff=1400, filterResonance=1.0,
         lfoShape=0, lfoRate=0.5, lfoDepth=0.3, lfoDestination=2, masterDrive=0.2,
         chorusEnable=1, chorusRate=0.6, chorusDepth=0.4, chorusMix=0.3),
    lambda i: f"Future Bass {i+1}",
    lambda p, i: (p.update(wtPosition=jitter(0.3, 0.25), unisonDetune=jitter(20, 8),
                           filterCutoff=jitter(1400, 350))))

# --- Synthwave / Outrun ---
family("Synthwave", 5,
    dict(oscModel=ANALOG, oscWaveA=SQR, pulseWidth=0.5, oscBGain=0.3, oscBPitch=-12,
         oscOctave=-1, polyphonyMode=MONO, glideTimeMs=15, ampAttack=0.005, ampDecay=0.3,
         ampSustain=0.8, ampRelease=0.25, filterType=LP, filterCutoff=950, filterResonance=0.7,
         lfoShape=0, lfoRate=1.2, lfoDepth=0.2, lfoDestination=3, masterDrive=0.12,
         chorusEnable=1, chorusRate=0.5, chorusDepth=0.45, chorusMix=0.4),
    lambda i: f"Synthwave {i+1}",
    lambda p, i: (p.update(oscWaveA=SQR if i % 2 else SAW, pulseWidth=jitter(0.5, 0.2),
                           filterCutoff=jitter(950, 220), lfoRate=jitter(1.2, 0.5))))

# --- Electro ---
family("Electro", 5,
    dict(oscModel=ANALOG, oscWaveA=SQR, pulseWidth=0.4, oscOctave=-1, polyphonyMode=MONO,
         glideTimeMs=6, ampAttack=0.002, ampDecay=0.2, ampSustain=0.6, ampRelease=0.12,
         filterType=LP, filterCutoff=900, filterResonance=2.0, filterEnvAmount=0.4,
         modDecay=0.18, masterDrive=0.35),
    lambda i: f"Electro {i+1}",
    lambda p, i: (p.update(pulseWidth=jitter(0.4, 0.15), filterResonance=jitter(2.0, 0.8),
                           masterDrive=jitter(0.35, 0.12))))

# --- UK Garage ---
family("UK Garage", 5,
    dict(oscModel=ANALOG, oscWaveA=TRI, oscWaveB=SINE, oscBGain=0.4, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=20, ampAttack=0.005, ampDecay=0.25, ampSustain=0.7,
         ampRelease=0.18, filterType=LP, filterCutoff=620, filterResonance=0.9, masterDrive=0.12),
    lambda i: f"UK Garage {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(620, 150), oscBGain=jitter(0.4, 0.12),
                           glideTimeMs=jitter(20, 10))))

# --- Grime ---
family("Grime", 5,
    dict(oscModel=ANALOG, oscWaveA=SQR, pulseWidth=0.5, oscBGain=0.3, oscBPitch=-12,
         oscOctave=-1, polyphonyMode=MONO, glideTimeMs=8, ampAttack=0.002, ampDecay=0.2,
         ampSustain=0.7, ampRelease=0.1, filterType=LP, filterCutoff=850, filterResonance=1.5,
         masterDrive=0.4),
    lambda i: f"Grime {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(850, 200), masterDrive=jitter(0.4, 0.15),
                           oscBGain=jitter(0.3, 0.12))))

# --- Hardstyle ---
family("Hardstyle", 5,
    dict(oscModel=FM, fmRatio=1.0, fmIndex=3.0, fmFeedback=0.6, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=4, ampAttack=0.002, ampDecay=0.15, ampSustain=0.9,
         ampRelease=0.08, filterType=LP, filterCutoff=1200, filterResonance=1.0,
         filterDrive=0.7, masterDrive=0.8),
    lambda i: f"Hardstyle {i+1}",
    lambda p, i: (p.update(fmIndex=jitter(3.0, 1.0), masterDrive=jitter(0.8, 0.1),
                           filterDrive=jitter(0.7, 0.15))))

# --- Gabber / Hardcore ---
family("Gabber", 3,
    dict(oscModel=ANALOG, oscWaveA=SAW, oscOctave=-1, polyphonyMode=MONO, glideTimeMs=2,
         ampAttack=0.001, ampDecay=0.15, ampSustain=0.9, ampRelease=0.06, filterType=LP,
         filterCutoff=1500, filterResonance=1.0, filterDrive=0.8, masterDrive=0.9),
    lambda i: f"Gabber {i+1}",
    lambda p, i: (p.update(masterDrive=jitter(0.9, 0.08), filterDrive=jitter(0.8, 0.12))))

# --- Big Room / Festival ---
family("Big Room", 5,
    dict(oscModel=ANALOG, oscWaveA=SINE, oscBGain=0.2, oscBPitch=-12, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=0, ampAttack=0.003, ampDecay=0.2, ampSustain=0.0,
         ampRelease=0.1, filterType=LP, filterCutoff=700, filterResonance=0.5, masterDrive=0.1),
    lambda i: f"Big Room {i+1}",
    lambda p, i: (p.update(ampRelease=jitter(0.1, 0.04), filterCutoff=jitter(700, 180))))

# --- Minimal ---
family("Minimal", 3,
    dict(oscModel=ANALOG, oscWaveA=TRI, oscOctave=-1, polyphonyMode=MONO, glideTimeMs=5,
         ampAttack=0.002, ampDecay=0.1, ampSustain=0.0, ampRelease=0.06, filterType=LP,
         filterCutoff=500, filterResonance=0.8, masterDrive=0.1),
    lambda i: f"Minimal {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(500, 120), ampDecay=jitter(0.1, 0.03))))

# --- IDM / Glitch ---
family("IDM", 5,
    dict(oscModel=FM, fmRatio=3.5, fmIndex=4.0, fmFeedback=0.4, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=6, ampAttack=0.002, ampDecay=0.2, ampSustain=0.5,
         ampRelease=0.12, filterType=LP, filterCutoff=1100, filterResonance=2.5,
         lfoShape=4, lfoRate=8.0, lfoDepth=0.4, lfoDestination=2, masterDrive=0.3),
    lambda i: f"IDM {i+1}",
    lambda p, i: (p.update(fmRatio=[1.5,3.5,5.0,2.5,7.0,4.0][i], lfoRate=jitter(8.0, 4.0),
                           fmIndex=jitter(4.0, 1.5))))

# --- Ambient / Downtempo ---
family("Ambient", 5,
    dict(oscModel=ANALOG, oscWaveA=SINE, oscWaveB=TRI, oscBGain=0.4, oscOctave=-1,
         polyphonyMode=POLY, glideTimeMs=0, ampAttack=0.3, ampDecay=0.5, ampSustain=0.8,
         ampRelease=0.9, filterType=LP, filterCutoff=600, filterResonance=0.5, masterDrive=0.05,
         chorusEnable=1, chorusRate=0.3, chorusDepth=0.5, chorusMix=0.4,
         delayEnable=1, delayTimeMs=500, delayFeedback=0.4, delayMix=0.3),
    lambda i: f"Ambient {i+1}",
    lambda p, i: (p.update(ampAttack=jitter(0.3, 0.15), ampRelease=jitter(0.9, 0.3),
                           filterCutoff=jitter(600, 200))))

# --- Nu-Disco / Funk ---
family("Nu-Disco", 5,
    dict(oscModel=ANALOG, oscWaveA=SAW, oscOctave=-1, polyphonyMode=MONO, glideTimeMs=8,
         ampAttack=0.003, ampDecay=0.2, ampSustain=0.4, ampRelease=0.14, filterType=LP,
         filterCutoff=750, filterResonance=1.2, filterEnvAmount=0.45, modDecay=0.15,
         masterDrive=0.18),
    lambda i: f"Nu-Disco {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(750, 180), filterEnvAmount=jitter(0.45, 0.18),
                           masterDrive=jitter(0.18, 0.08))))

# --- Hyperpop / Wonky ---
family("Hyperpop", 3,
    dict(oscModel=WT, wtPosition=0.7, wtWarp=0.6, oscOctave=-1, unisonVoices=3, unisonDetune=25,
         polyphonyMode=MONO, glideTimeMs=30, ampAttack=0.003, ampDecay=0.2, ampSustain=0.7,
         ampRelease=0.15, filterType=LP, filterCutoff=1500, filterResonance=2.0, masterDrive=0.4),
    lambda i: f"Hyperpop {i+1}",
    lambda p, i: (p.update(wtPosition=jitter(0.7, 0.25), glideTimeMs=jitter(30, 15),
                           masterDrive=jitter(0.4, 0.15))))

# --- Rock ---
family("Rock", 4,
    dict(oscModel=ANALOG, oscWaveA=SQR, pulseWidth=0.45, oscBGain=0.2, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=4, ampAttack=0.004, ampDecay=0.25, ampSustain=0.75,
         ampRelease=0.2, filterType=LP, filterCutoff=1000, filterResonance=0.7, masterDrive=0.25),
    lambda i: f"Rock Bass {i+1}",
    lambda p, i: (p.update(pulseWidth=jitter(0.45, 0.1), filterCutoff=jitter(1000, 250),
                           masterDrive=jitter(0.25, 0.1))))

# --- Metal ---
family("Metal", 4,
    dict(oscModel=ANALOG, oscWaveA=SAW, oscBGain=0.2, oscOctave=-1, polyphonyMode=MONO,
         glideTimeMs=2, ampAttack=0.002, ampDecay=0.2, ampSustain=0.8, ampRelease=0.12,
         filterType=LP, filterCutoff=1400, filterResonance=1.0, filterDrive=0.6, masterDrive=0.5),
    lambda i: f"Metal Bass {i+1}",
    lambda p, i: (p.update(masterDrive=jitter(0.5, 0.15), filterCutoff=jitter(1400, 300),
                           filterDrive=jitter(0.6, 0.15))))

# --- Funk / Soul ---
family("Funk", 4,
    dict(oscModel=ANALOG, oscWaveA=TRI, oscWaveB=SINE, oscBGain=0.35, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=10, ampAttack=0.004, ampDecay=0.2, ampSustain=0.5,
         ampRelease=0.16, filterType=LP, filterCutoff=820, filterResonance=1.4,
         filterEnvAmount=0.4, modDecay=0.14, masterDrive=0.15),
    lambda i: f"Funk Bass {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(820, 180), filterEnvAmount=jitter(0.4, 0.18))))

# --- Jazz / Upright ---
family("Jazz", 3,
    dict(oscModel=ANALOG, oscWaveA=SINE, oscWaveB=TRI, oscBGain=0.3, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=25, ampAttack=0.008, ampDecay=0.3, ampSustain=0.4,
         ampRelease=0.25, filterType=LP, filterCutoff=520, filterResonance=0.5, masterDrive=0.08),
    lambda i: f"Upright {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(520, 120), glideTimeMs=jitter(25, 10))))

# --- Reggae / Dub ---
family("Reggae Dub", 4,
    dict(oscModel=ANALOG, oscWaveA=SINE, oscBGain=0.0, oscOctave=-1, polyphonyMode=MONO,
         glideTimeMs=20, ampAttack=0.006, ampDecay=0.4, ampSustain=0.8, ampRelease=0.3,
         filterType=LP, filterCutoff=380, filterResonance=0.5, masterDrive=0.1),
    lambda i: f"Dub Bass {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(380, 90), ampRelease=jitter(0.3, 0.1))))

# --- Pop / RnB ---
family("Pop RnB", 4,
    dict(oscModel=ANALOG, oscWaveA=SINE, oscWaveB=TRI, oscBGain=0.3, oscOctave=-1,
         polyphonyMode=MONO, glideTimeMs=15, ampAttack=0.004, ampDecay=0.25, ampSustain=0.75,
         ampRelease=0.2, filterType=LP, filterCutoff=700, filterResonance=0.6, masterDrive=0.1),
    lambda i: f"Pop Bass {i+1}",
    lambda p, i: (p.update(filterCutoff=jitter(700, 160), oscBGain=jitter(0.3, 0.1))))

# --- Cinematic ---
family("Cinematic", 4,
    dict(oscModel=ANALOG, oscWaveA=SINE, oscOctave=-2, polyphonyMode=POLY, glideTimeMs=0,
         ampAttack=0.4, ampDecay=0.6, ampSustain=0.9, ampRelease=1.5, bandsEnable=1,
         crossoverLow=120, crossoverHigh=2000, bodyModel=ANALOG, bodyGain=1.0,
         charModel=ANALOG, charGain=0.6, airModel=WT, airGain=0.5,
         filterType=LP, filterCutoff=500, filterResonance=0.4, masterDrive=0.15,
         chorusEnable=1, chorusRate=0.2, chorusDepth=0.5, chorusMix=0.4),
    lambda i: f"Cinematic Sub {i+1}",
    lambda p, i: (p.update(ampRelease=jitter(1.5, 0.5), filterCutoff=jitter(500, 150),
                           oscOctave=-2 if i % 2 == 0 else -1)))

# ---------------------------------------------------------------------------
print(f"Generated {len(presets)} presets")
assert len(presets) == 200, f"expected exactly 200, have {len(presets)} — adjust family counts"

# Emit C++
lines = []
lines.append('#include "Presets.h"')
lines.append('')
lines.append('// AUTO-GENERATED by tools/gen_presets.py — do not edit by hand.')
lines.append('// 200 genre presets. Each entry: name, category, {paramID, value} overrides.')
lines.append('// Params not listed fall back to their default when the preset is applied.')
lines.append('')
lines.append('namespace BPPresets {')
lines.append('')
lines.append('const std::vector<Preset>& getAll()')
lines.append('{')
lines.append('    static const std::vector<Preset> presets = {')
for name, cat, params in presets:
    parts = ", ".join(f'{{"{k}", {v:.4f}f}}' for k, v in params.items())
    lines.append(f'        {{ "{name}", "{cat}", {{ {parts} }} }},')
lines.append('    };')
lines.append('    return presets;')
lines.append('}')
lines.append('')
lines.append('} // namespace BPPresets')
lines.append('')

with open("/home/claude/BassProfessor/Source/Presets.cpp", "w") as f:
    f.write("\n".join(lines))
print("Wrote Source/Presets.cpp")

# Print category summary
from collections import Counter
cats = Counter(c for _, c, _ in presets)
print("Categories:", dict(cats))

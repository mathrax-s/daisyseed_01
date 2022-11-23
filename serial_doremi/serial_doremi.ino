#include "DaisyDuino.h"

#define MAXCH 10

DaisyHardware hw;

Oscillator osc[MAXCH];
AdEnv env[MAXCH];
ReverbSc verb;

uint8_t scale[7] = {0, 2, 4, 5, 7, 9, 11};
uint8_t keylist[] = {'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/'};
float rawsig, comp_sig, wetvl, wetvr;
uint8_t ch_cnt = 0;

static void audio(float **in, float **out, size_t size) {
  // Audio Loop
  for (size_t i = 0; i < size; i++) {
    // Process
    for (int c = 0; c < MAXCH; c++) {
      osc[c].SetAmp(env[c].Process() / (MAXCH / 4));
      rawsig += osc[c].Process() / (size * MAXCH);
    }
    verb.Process(rawsig, rawsig, &wetvl, &wetvr);

    out[0][i] = wetvl;
    out[1][i] = wetvr;
  }
}

void InitSynth(float samplerate) {
  for (int c = 0; c < MAXCH; c++) {
    osc[c].Init(samplerate);
    osc[c].SetWaveform(Oscillator::WAVE_SIN);
    osc[c].SetFreq(100.0f);
    osc[c].SetAmp(.5f);
    env[c].Init(samplerate);
    env[c].SetCurve(-15.0f);
    env[c].SetTime(ADENV_SEG_ATTACK, 0.01f);
    env[c].SetTime(ADENV_SEG_DECAY, 0.1f);
    env[c].SetTime(ADSR_SEG_RELEASE, .5);
    env[c].SetMax(1.f);
    env[c].SetMin(0.f);
  }
  verb.Init(samplerate);
  verb.SetFeedback(0.91f);
  verb.SetLpFreq(12000.0f);
}

void setup() {
  float samplerate, callback_rate;
  hw = DAISY.init(DAISY_POD, AUDIO_SR_48K);
  samplerate = DAISY.get_samplerate();
  InitSynth(samplerate);

  Serial.begin(115200);
  while (!Serial) {
  };
  DAISY.begin(audio);
}

void loop() {
  if (Serial.available()) {
    uint8_t rcvdata = Serial.read();
    for (int c = 0; c < MAXCH; c++) {
      if (rcvdata == keylist[c]) {
        float freq = mtof(72.0f + scale[c % 7] + (int)(c / 7) * 12);
        osc[ch_cnt % MAXCH].SetFreq(freq);
        env[ch_cnt % MAXCH].SetTime(ADENV_SEG_DECAY, 1.0);
        env[ch_cnt % MAXCH].Trigger();
        ch_cnt++;
      }
    }
  }
}

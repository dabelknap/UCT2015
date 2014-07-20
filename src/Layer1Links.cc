#include "../interface/Layer1Links.h"


/** ----------------------------
 *
 * TriggerTower Implementation
 *
 * -----------------------------
 */


TriggerTower::TriggerTower(short iEta, short iPhi) {
  ieta = iEta;
  iphi = iPhi;
}


void
TriggerTower::set_ecal_fg(bool fg) {
  ecal_fg = fg;
}


void
TriggerTower::set_hcal_fg(bool fg) {
  hcal_fg = fg;
}


void
TriggerTower::set_ecal_energy(int E) {
  ecal_energy = E;
}


void
TriggerTower::set_hcal_energy(int E) {
  hcal_energy = E;
}


uint16_t
TriggerTower::total_energy() {
   uint16_t E = (uint16_t)(ecal_energy + hcal_energy);
   
   return (E & 0x1ff); // return only 9 bits
}


uint16_t
TriggerTower::e_ratio() {
  // set to 0 for July 2014 test
  return 0;
}


bool
TriggerTower::denom() {
  // set to 0 for July 2014 test
  return 0;
}


bool
TriggerTower::e_over_h() {
  // set to 0 for July 2014 test
  return 0;
}


uint16_t
TriggerTower::output_word() {
  uint16_t out = 0;

  // bit 15
  if (ecal_fg) {
    out |= 0x8000;
  }

  // bit 14
  if (hcal_fg) {
    out |= 0x4000;
  }

  // bit 13
  if (e_over_h()) {
    out |= 0x2000;
  }

  // bit 12
  if (denom()) {
    out |= 0x1000;
  }

  // bits 9-11
  uint16_t ratio = e_ratio();
  out |= (ratio << 9);

  // bits 0-8
  uint16_t E = total_energy();
  out |= E;

  return out;
}


/** ----------------------------
 *
 * Layer1Links Implementation
 *
 * -----------------------------
 */


Layer1Links::Layer1Links(unsigned int Event, unsigned int Lumi,
    unsigned int Run) {
  event = Event;
  lumi = Lumi;
  run = Run;

  // link number
  for (int i = 0; i < 72; i++) {
    // abs|ieta|
    for (int j = 0; j < 40; j++) {

      short iphi = i/2;
      short ieta = i * (i % 2 == 0 ? 1 : -1);

      trigger_towers[i][j][0] = TriggerTower(ieta, iphi);
      trigger_towers[i][j][1] = TriggerTower(ieta, iphi+1);
    }
  }
}


void
Layer1Links::add_tower() {
}

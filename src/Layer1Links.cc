/**
 * Filename: Layer1Links.cc
 *
 * Description: This file contains the implementations for the TriggerTower and
 *              Layer1Link classes.
 *
 * Author: D. Austin Belknap, UW-Madison
 */

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


/**
 * Layer1Link constructor. Each object is identified by the even/lumi/run
 * number. Initialize the collection of trigger towers.
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

      short iphi = i/2+1;
      short ieta = (j + 1) * (i % 2 == 0 ? 1 : -1);

      // ieta/iphi bounds checking
      if (1 > iphi || iphi > 72 || 1 > (j+1) || (j+1) > 41) {
        std::stringstream ss;
        ss << "Layer1Links Constructor  iphi: " << iphi << ", |ieta|: " << (j+1);
        throw std::invalid_argument(ss.str());
      }

      // initialize towers
      trigger_towers[i][j][0] = TriggerTower(ieta, iphi);
      trigger_towers[i][j][1] = TriggerTower(ieta, iphi+1);
    }
  }
}


/**
 * Add an ECAL trigger tower to the collection: iEta, iPhi, energy, and
 * fine-grain.
 */
void
Layer1Links::add_ecal_tower(short iEta, short iPhi, int E, bool fg) {
  // convert iPhi/iEta values into its corresponding link number
  short link = (iEta < 0 ? 2*((iPhi-1)/2)+1 : 2*((iPhi-1)/2));

  short tower = (iPhi % 2 == 0 ? 0 : 1);

  short ieta = iEta * (iEta > 0 ? 1 : -1);

  // bounds checking on link, ieta, and tower values
  if (0 > link || link > 71 || 0 > (ieta-1) || (ieta-1) > 39 || 0 > tower || tower > 1) {
      std::stringstream ss;
      ss << "link: " << link << ", ieta-1: " << (ieta-1) << ", tower: " << tower;
      throw std::invalid_argument(ss.str());
  }

  trigger_towers[link][ieta-1][tower].set_ecal_energy(E);
  trigger_towers[link][ieta-1][tower].set_ecal_fg(E);
}


/**
 * Add an HCAL trigger tower to the collection: iEta, iPhi, energy, and
 * fine-grain.
 */
void
Layer1Links::add_hcal_tower(short iEta, short iPhi, int E, bool fg) {
  // convert iPhi/iEta values into its corresponding link number
  short link = (iEta < 0 ? 2*((iPhi-1)/2)+1 : 2*((iPhi-1)/2));

  short tower = (iPhi % 2 == 0 ? 0 : 1);

  short ieta = iEta * (iEta > 0 ? 1 : -1);

  if (0 > link || link > 71 || 0 > (ieta-1) || (ieta-1) > 39 || 0 > tower || tower > 1) {
      std::stringstream ss;
      ss << "link: " << link << ", ieta-1: " << (ieta-1) << ", tower: " << tower;
      throw std::invalid_argument(ss.str());
  }

  // bounds checking on link, ieta, and tower values
  trigger_towers[link][ieta-1][tower].set_hcal_energy(E);
  trigger_towers[link][ieta-1][tower].set_hcal_fg(E);
}


/**
 * Convert the trigger tower data into the format used by the output optical
 * links.
 */
void
Layer1Links::populate_links() {
  for (int i = 0; i < 72; ++i) {
    for (int j = 0; j < 40; ++j) {
      uint8_t byte0 = 0;
      uint8_t byte1 = 0;
      uint8_t byte2 = 0;
      uint8_t byte3 = 0;

      byte0 = (uint8_t)( trigger_towers[i][j][0].output_word() & 0x00ff);
      byte1 = (uint8_t)((trigger_towers[i][j][0].output_word() & 0xff00) >> 8);
      byte2 = (uint8_t)( trigger_towers[i][j][1].output_word() & 0x00ff);
      byte3 = (uint8_t)((trigger_towers[i][j][1].output_word() & 0xff00) >> 8);

      links[i][j][3] = byte0;
      links[i][j][2] = byte1;
      links[i][j][1] = byte2;
      links[i][j][0] = byte3;
    }
  }
}


/**
 * Write the CTP7/Layer1 output patterns to a text file.
 */
void
Layer1Links::write_to_file(std::ofstream& outfile) {
  if (!outfile.is_open()) {
    throw std::runtime_error("File is not open");
  }

  // convert TriggerTower collection to link format
  populate_links();

  // identify event with run/lumi/event numbers
  outfile << "run: " << run << " lumi: " << lumi << " event: " << event << std::endl;

  // for all links
  for (int i = 0; i < 72; i++) {
    outfile << "Link " << std::uppercase << std::setfill('0') << std::setw(2) << i << ": ";

    // for all ieta values
    for (int j = 0; j < 40; ++j) {
      uint32_t out = 0;

      // for all byte numbers
      for (int k = 0; k < 4; ++k) {
        out <<= 8;
        out |= links[i][j][k];
      }
      outfile << std::setw(8) << std::hex << (long int)(out) <<  " ";
    }
    outfile << std::dec << std::endl;
  }
  outfile << std::endl;
}

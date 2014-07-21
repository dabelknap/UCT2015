/**
 * Filename: Layer1Links.h
 *
 * Description: Two classes are defined here. The TriggerTower class contains
 *              eta/phi and energy information of a single trigger tower, and
 *              can compute the values produced by Layer1 (CTP7). Layer1Links
 *              contains the collection of all trigger towers, and has methods
 *              for inputing ECAL/HCAL trigger primitive data, and outputing the
 *              CTP7 link data to a text file.
 *
 * Author: D. Austin Belknap, UW-Madison
 */

#ifndef _LAYER1LINKS_h
#define _LAYER1LINKS_h

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <stdint.h>
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <sstream>


/**
 * This class contains the trigger tower data relevant to the output of Layer1.
 * (CTP7 board).
 */
class TriggerTower {

  private:

    short ieta;
    short iphi;
    bool ecal_fg;
    bool hcal_fg;
    int ecal_energy;
    int hcal_energy;

    uint16_t total_energy();
    uint16_t e_ratio();
    bool denom();
    bool e_over_h();
    

  public:

    TriggerTower(short iEta, short iPhi);
    TriggerTower() { };
    ~TriggerTower() { };

    void set_ecal_fg(bool fg);
    void set_hcal_fg(bool fg);

    void set_ecal_energy(int E);
    void set_hcal_energy(int E);

    uint16_t output_word();
};


/**
 * This class contains a collection of all trigger towers, and all of the Layer1
 * output links. It can accept ECAL/HCAL trigger primitives, and can convert
 * them into the form output by the CTP7. It contains a method for writing the
 * CTP7 output patterns to a text file.
 */
class Layer1Links {

  private:

    uint8_t links[72][40][4];

    TriggerTower trigger_towers[72][40][2];

    unsigned int event;
    unsigned int lumi;
    unsigned int run;


  public:

    Layer1Links(unsigned int event, unsigned int lumi, unsigned int run);
    ~Layer1Links() { };

    void add_ecal_tower(short ieta, short iphi, int E, bool fg);
    void add_hcal_tower(short ieta, short iphi, int E, bool fg);

    void populate_links();

    void write_to_file(std::ofstream& outfile);
};


#endif

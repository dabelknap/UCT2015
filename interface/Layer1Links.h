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

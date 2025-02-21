#pragma once

#ifndef CUSTOMINFOS_H
#define CUSTOMINFOS_H

#include "datainfos.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///// IF USING CUSTOM, FILL IN YOUR DATA PACKERS HERE /////////////////////////////////////////////
const MapGridInfo CustomMapGridInfo = {
   .mapgrid_masks = {
      {"metatileId", 0x03FF},
      {"collision",  0x0C00},
      {"elevation",  0xF000}
   }
};
const MetatilesInfo CustomMetatilesInfo = {
   .num_metatiles_in_primary = 512,
   .num_metatiles_in_secondary = 512,
   .num_tiles_in_primary = 512,
   .num_tiles_in_secondary = 512,
   .num_pals_in_primary = 6,
   .num_pals_in_secondary = 7,
   .num_tiles_in_metatile = 8,
   .tiles_masks{
      {"tileId",  0x03FF},
      {"xflip",   0x0400},
      {"yflip",   0x0800},
      {"palette", 0xF000}
   }
};
const MetatileAttributesInfo<uint16_t> CustomMetatileAttributesInfo = {
   .num_metatiles_in_primary = 512,
   .num_metatiles_in_secondary = 512,
   .attribute_masks = {
      {"behavior", 0x00FF},
      {"layer",    0xF000},
      {"unused",   0x0F00}
   }
};
///////////////////////////////////////////////////////////////////////////////////////////////////


#endif // CUSTOMINFOS_H
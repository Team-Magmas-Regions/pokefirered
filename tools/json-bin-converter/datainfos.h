#pragma once

#ifndef DATAINFOS_H
#define DATAINFOS_H

#include <string>
#include <map>

struct MapGridInfo {
   std::map<std::string, uint16_t> mapgrid_masks{};
};

struct MetatilesInfo {
   unsigned num_metatiles_in_primary{0};
   unsigned num_metatiles_in_secondary{0};
   unsigned num_tiles_in_primary{0};
   unsigned num_tiles_in_secondary{0};
   unsigned num_pals_in_primary{0};
   unsigned num_pals_in_secondary{0};
   unsigned num_tiles_in_metatile{0};
   std::map<std::string, uint16_t> tiles_masks{};
};

/// The type should be uint16_t or uint32_t depending on RSE or FRLG respectively for size 
///    of the attribute. Though this should also support uint8_t and uint64_t
template<class T>
struct MetatileAttributesInfo {
   unsigned num_metatiles_in_primary{0};
   unsigned num_metatiles_in_secondary{0};

   std::map<std::string, T> attribute_masks{};

   // We're just doing a check to make sure we were created with an int pre c++20
   static_assert(std::is_integral_v<T>);
};


const MapGridInfo MapGridInfoAll{
   .mapgrid_masks = {
      {"metatileId", 0x03FF},
      {"collision",  0x0C00},
      {"elevation",  0xF000}
   }
};

const MetatilesInfo MetatilesInfoRSE{
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
const MetatilesInfo MetatilesInfoFRLG{
   .num_metatiles_in_primary = 640,
   .num_metatiles_in_secondary = 512,
   .num_tiles_in_primary = 640,
   .num_tiles_in_secondary = 384,
   .num_pals_in_primary = 7,
   .num_pals_in_secondary = 6,
   .num_tiles_in_metatile = 8,
   .tiles_masks{
      {"tileId",  0x03FF},
      {"xflip",   0x0400},
      {"yflip",   0x0800},
      {"palette", 0xF000}
   }
};

const MetatileAttributesInfo<uint16_t> MetatileAttributesInfoRSE{
   .num_metatiles_in_primary = 512,
   .num_metatiles_in_secondary = 512,
   .attribute_masks = {
      {"behavior", 0x00FF},
      {"layer",    0xF000},
      {"unused",   0x0F00}
   }
};
const MetatileAttributesInfo<uint32_t> MetatileAttributesInfoFRLG{
   .num_metatiles_in_primary = 512,
   .num_metatiles_in_secondary = 512,
   .attribute_masks = {
      {"behavior",  0x000001FF},
      {"terrain",   0x00003E00},
      {"encounter", 0x07000000},
      {"layer",     0x60000000},
      {"unused",    0x98FFC000},
   }
};

#endif // DATAINFOS_H
# Map JSON data

# Inputs
MAPS_DIR = $(DATA_ASM_SUBDIR)/maps
LAYOUTS_DIR = $(DATA_ASM_SUBDIR)/layouts
TILESETS_DIR = $(DATA_ASM_SUBDIR)/tilesets

# Outputs
MAPS_OUTDIR := $(MAPS_DIR)
LAYOUTS_OUTDIR := $(LAYOUTS_DIR)
INCLUDECONSTS_OUTDIR := include/constants

AUTO_GEN_TARGETS += $(INCLUDECONSTS_OUTDIR)/map_groups.h
AUTO_GEN_TARGETS += $(INCLUDECONSTS_OUTDIR)/layouts.h
AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/heal_locations.h

MAP_DIRS := $(dir $(wildcard $(MAPS_DIR)/*/map.json))
MAP_CONNECTIONS := $(patsubst $(MAPS_DIR)/%/,$(MAPS_DIR)/%/connections.inc,$(MAP_DIRS))
MAP_EVENTS := $(patsubst $(MAPS_DIR)/%/,$(MAPS_DIR)/%/events.inc,$(MAP_DIRS))
MAP_HEADERS := $(patsubst $(MAPS_DIR)/%/,$(MAPS_DIR)/%/header.inc,$(MAP_DIRS))
MAP_JSONS := $(patsubst $(MAPS_DIR)/%/,$(MAPS_DIR)/%/map.json,$(MAP_DIRS))

LAYOUTS_DIRS := $(wildcard $(LAYOUTS_DIR)/*/)
LAYOUTS_MAP_BINS := $(patsubst $(LAYOUTS_DIR)/%/,$(LAYOUTS_DIR)/%/map.bin,$(LAYOUTS_DIRS))
LAYOUTS_BORDER_BINS := $(patsubst $(LAYOUTS_DIR)/%/,$(LAYOUTS_DIR)/%/border.bin,$(LAYOUTS_DIRS))

LAYOUTS_MAP_JSONS := $(patsubst $(LAYOUTS_DIR)/%/,$(LAYOUTS_DIR)/%/map.json,$(LAYOUTS_DIR))
LAYOUTS_BORDER_JSONS := $(patsubst $(LAYOUTS_DIR)/%/,$(LAYOUTS_DIR)/%/border.json,$(LAYOUTS_DIR))

$(LAYOUTS_DIR)/%/border.bin: $(LAYOUTS_DIR)/%/border.json
		@echo "$(JSON2BIN) mapgrid $<"
			$(JSON2BIN) mapgrid $<
$(LAYOUTS_DIR)/%/map.bin: $(LAYOUTS_DIR)/%/map.json
		@echo "$(JSON2BIN) mapgrid $<"
			$(JSON2BIN) mapgrid $<

mapgrid-bins-generated: $(LAYOUTS_MAP_BINS) $(LAYOUTS_BORDER_BINS)
		@echo "mapgrid-bins-generated $(LAYOUTS_BORDER_BINS) $(LAYOUTS_MAP_BINS)"

$(LAYOUTS_DIR)/layouts.json: $(mapgrid-bins-generated)

PRIMARY_TILESETS_DIRS := $(wildcard $(TILESETS_DIR)/primary/*/)
SECONDARY_TILESETS_DIRS := $(wildcard $(TILESETS_DIR)/secondary/*/)
TILESETS_METATILES_BINS := $(patsubst $(TILESETS_DIR)/primary/%/,$(TILESETS_DIR)/primary/%/metatiles.bin,$(PRIMARY_TILESETS_DIRS)) $(patsubst $(TILESETS_DIR)/secondary/%/,$(TILESETS_DIR)/secondary/%/metatiles.bin,$(SECONDARY_TILESETS_DIRS))
TILESETS_METATILE_ATTRIBUTES_BINS := $(patsubst $(TILESETS_DIR)/primary/%/,$(TILESETS_DIR)/primary/%/metatile_attributes.bin,$(PRIMARY_TILESETS_DIRS)) $(patsubst $(TILESETS_DIR)/secondary/%/,$(TILESETS_DIR)/secondary/%/metatile_attributes.bin,$(SECONDARY_TILESETS_DIRS))

TILESETS_METATILES_JSONS := $(patsubst $(TILESETS_DIR)/primary/%/,$(TILESETS_DIR)/primary/%/metatiles.json,$(PRIMARY_TILESETS_DIRS)) $(patsubst $(TILESETS_DIR)/secondary/%/,$(TILESETS_DIR)/secondary/%/metatiles.json,$(SECONDARY_TILESETS_DIRS))
TILESETS_METATILE_ATTRIBUTES_JSONS := $(patsubst $(TILESETS_DIR)/primary/%/,$(TILESETS_DIR)/primary/%/metatile_attributes.json,$(PRIMARY_TILESETS_DIRS)) $(patsubst $(TILESETS_DIR)/secondary/%/,$(TILESETS_DIR)/secondary/%/metatile_attributes.json,$(SECONDARY_TILESETS_DIRS))

%/metatiles.bin: %/metatiles.json
		@echo "$(JSON2BIN) metatiles $<"
			$(JSON2BIN) metatiles $<
%/metatile_attributes.bin: %/metatile_attributes.json
		@echo "$(JSON2BIN) metatile_attributes $<"
			$(JSON2BIN) metatile_attributes $<

metatile-bins-generated: $(TILESETS_METATILES_BINS) $(TILESETS_METATILE_ATTRIBUTES_BINS)
		@echo "metatile-bins-generated $(TILESETS_METATILES_BINS) $(TILESETS_METATILE_ATTRIBUTES_BINS)"

$(DATA_ASM_BUILDDIR)/tilesets.o: $(metatile-bins-generated)

$(DATA_ASM_BUILDDIR)/maps.o: $(DATA_ASM_SUBDIR)/maps.s $(LAYOUTS_DIR)/layouts.inc $(LAYOUTS_DIR)/layouts_table.inc $(MAPS_DIR)/headers.inc $(MAPS_DIR)/groups.inc $(MAPS_DIR)/connections.inc $(MAP_CONNECTIONS) $(MAP_HEADERS)
	$(PREPROC) $< charmap.txt | $(CPP) -I include -nostdinc -undef -Wno-unicode - | $(PREPROC) -ie $< charmap.txt | $(AS) $(ASFLAGS) -o $@
$(DATA_ASM_BUILDDIR)/map_events.o: $(DATA_ASM_SUBDIR)/map_events.s $(MAPS_DIR)/events.inc $(MAP_EVENTS)
	$(PREPROC) $< charmap.txt | $(CPP) -I include -nostdinc -undef -Wno-unicode - | $(PREPROC) -ie $< charmap.txt | $(AS) $(ASFLAGS) -o $@

$(MAPS_OUTDIR)/%/header.inc $(MAPS_OUTDIR)/%/events.inc $(MAPS_OUTDIR)/%/connections.inc: $(MAPS_DIR)/%/map.json
	$(MAPJSON) map firered $< $(LAYOUTS_DIR)/layouts.json $(@D)

$(MAPS_OUTDIR)/connections.inc $(MAPS_OUTDIR)/groups.inc $(MAPS_OUTDIR)/events.inc $(MAPS_OUTDIR)/headers.inc $(INCLUDECONSTS_OUTDIR)/map_groups.h: $(MAPS_DIR)/map_groups.json
	$(MAPJSON) groups firered $< $(MAPS_OUTDIR) $(INCLUDECONSTS_OUTDIR)

$(LAYOUTS_OUTDIR)/layouts.inc $(LAYOUTS_OUTDIR)/layouts_table.inc $(INCLUDECONSTS_OUTDIR)/layouts.h: $(LAYOUTS_DIR)/layouts.json $(LAYOUTS_MAP_BINS) $(LAYOUTS_BORDER_BINS)
	$(MAPJSON) layouts firered $< $(LAYOUTS_OUTDIR) $(INCLUDECONSTS_OUTDIR)

$(DATA_SRC_SUBDIR)/heal_locations.h: $(MAP_JSONS)
	@$(MAPJSON) heal_locations firered $^ $(DATA_SRC_SUBDIR)/heal_locations.h

# This is a migration script you can run to convert data/layouts/*/border.bin and data/layouts/*/map.bin bin files to json
.PHONY: run-layout-mapgrids-bin-to-json
run-layout-mapgrids-bin-to-json:
	$(BIN2JSON) frlg mapgrid $(LAYOUTS_BORDER_BINS) $(LAYOUTS_MAP_BINS)

# This is a migration script you can run to convert data/tilesets/*/*/metatiles.bin and data/tilesets/*/*/metatile_attributes.bin bin files to json
.PHONY: run-tileset-metatiles-bin-to-json
run-tileset-metatiles-bin-to-json:
		$(BIN2JSON) frlg metatiles $(TILESETS_METATILES_BINS)
		$(BIN2JSON) frlg metatile_attributes $(TILESETS_METATILE_ATTRIBUTES_BINS)

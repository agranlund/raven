
BOARD		= a1
DATE		= $(shell date +%Y%m%d)

FW_NESSI	= nessi_240901.jed
FW_NARNIA	= narnia_240422.jed
FW_IKSI		= iksi_240418.jed
FW_EIFFEL	= eiffel_1.10.1\(Mq\)_fixedFire_noLcd.hex
FW_SIMMPROG	= ravenprog.uf2

RELEASE_TAG	= Raven.A1.latest
RELEASE_DESC	= Raven.A1.$(DATE)
RELEASE_ZIP	= raven_a1_$(DATE).zip

ATARIDIR	= /Volumes/SHARE/asus/raven
ROOTDIR		= ..
DISTDIR		= $(ROOTDIR)/dist
SUPPDIR		= $(DISTDIR)/3rdparty
OUTDIR		= $(DISTDIR)/raven
HWDIR		= $(OUTDIR)/hardware
FWDIR		= $(OUTDIR)/firmware
ROMDIR		= $(OUTDIR)/roms
SWDIR		= $(OUTDIR)/software
COPY		= cp -p


SWDIR_AUTO	= $(SWDIR)/AUTO
SWDIR_NOVA	= $(SWDIR)/NOVA
SWDIR_TOOLS	= $(SWDIR)/TOOLS
SWDIR_ISA	= $(SWDIR)/RAVEN/DRIVERS




.phony: release release-test
.phony: setup clean build copy upload
.phony: copydocs copyroms copyfw
.phony: buildroms

release: setup build copy zipit upload

release-test: setup copy zipit

setup: clean
	mkdir -p $(OUTDIR)

clean:
	rm -r -f $(OUTDIR)
	rm -f *.zip	

copy: copydocs copyhw copyfw copyroms 

build: buildroms


buildroms:
	make -C $(ROOTDIR)/sw/rom dist

copydocs:
	$(COPY) $(ROOTDIR)/doc/raven_a1.pdf $(OUTDIR)/manual.pdf

copyroms:
	mkdir -p $(ROMDIR)
	$(COPY) $(DISTDIR)/readme_rom.txt $(ROMDIR)/readme.txt
	$(COPY) $(ROOTDIR)/sw/rom/rom_mon.bin $(ROMDIR)/
	$(COPY) $(ROOTDIR)/sw/rom/rom_tos.bin $(ROMDIR)/
	$(COPY) $(ROOTDIR)/sw/rom/rom_dbg.bin $(ROMDIR)/


copyfw:
	mkdir -p $(FWDIR)
	$(COPY) $(DISTDIR)/readme_fw.txt $(FWDIR)/readme.txt
	$(COPY) $(ROOTDIR)/fw/$(BOARD)/Nessi/$(FW_NESSI) $(FWDIR)/
	$(COPY) $(ROOTDIR)/fw/$(BOARD)/Narnia/$(FW_NARNIA) $(FWDIR)/
	$(COPY) $(ROOTDIR)/fw/$(BOARD)/Iksi/$(FW_IKSI) $(FWDIR)/
	$(COPY) $(ROOTDIR)/fw/eiffel/$(FW_EIFFEL) $(FWDIR)/eiffel.hex
	$(COPY) $(ROOTDIR)/hw/simm/programmer/firmware/build/$(FW_SIMMPROG) $(FWDIR)/


copyhw:
	mkdir -p $(HWDIR)
	$(COPY) $(ROOTDIR)/hw/raven/$(BOARD)/production/raven-a1_gerber.zip $(HWDIR)/
	$(COPY) $(ROOTDIR)/hw/raven/$(BOARD)/production/raven-a1_bom.md $(HWDIR)/
	$(COPY) $(ROOTDIR)/hw/raven/$(BOARD)/production/raven-a1_ibom.html $(HWDIR)/
	$(COPY) $(ROOTDIR)/hw/raven/$(BOARD)/production/raven-a1_gerber.zip $(HWDIR)/
	$(COPY) $(ROOTDIR)/hw/raven/$(BOARD)/production/raven-a1_sch.pdf $(HWDIR)/
	$(COPY) $(ROOTDIR)/hw/raven/$(BOARD)/production/raven-a1_brd.pdf $(HWDIR)/

zipit:
	zip -r -o $(RELEASE_ZIP) raven/*

upload: github_delete_release github_create_release



.phony: github_delete_release
github_delete_release:
	gh release delete $(RELEASE_TAG) --cleanup-tag -y

.phony: github_create_release
github_create_release:
	gh release create $(RELEASE_TAG) --notes-file "release_notes.txt"
	gh release upload $(RELEASE_TAG) $(RELEASE_ZIP) --clobber



copysw:
	mkdir -p $(SWDIR)
	mkdir -p $(SWDIR_AUTO)
	mkdir -p $(SWDIR_TOOLS)
	mkdir -p $(SWDIR_ISA)

	$(COPY) $(DISTDIR)/readme_sw.txt $(SWDIR)/raven.txt

	# --------------------------------------------------
	# RAVEN base tools
	# --------------------------------------------------
		mkdir -p $(SWDIR_TOOLS)/castaway
		$(COPY) $(ROOTDIR)/../castaway/bin/* $(SWDIR_TOOLS)/castaway/
		$(COPY) $(ROOTDIR)/../castaway/readme.txt $(SWDIR_TOOLS)/castaway/
		$(COPY) $(ROOTDIR)/../castaway/license.txt $(SWDIR_TOOLS)/castaway/


	# --------------------------------------------------
	# NOVA
	# --------------------------------------------------
		mkdir -p $(SWDIR_NOVA)
		unzip $(SUPPDIR)/nova/TT_ET4000T0_V300.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/T0 $(SWDIR_NOVA)/ET4K-T0
		unzip $(SUPPDIR)/nova/TT_ET4000T3_V300.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/T3 $(SWDIR_NOVA)/ET4K-T3
		unzip $(SUPPDIR)/nova/TT_ET4000T4_V300.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/T4 $(SWDIR_NOVA)/ET4K-T4
		unzip $(SUPPDIR)/nova/TT_ET4000T8_V300.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/T8 $(SWDIR_NOVA)/ET4K-T8
		unzip $(SUPPDIR)/nova/TT_ET4000W32_V301.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/TT $(SWDIR_NOVA)/ET4KW32I
		unzip $(SUPPDIR)/nova/TT_Mach32_V301.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/TT $(SWDIR_NOVA)/MACH32
		unzip $(SUPPDIR)/nova/TT_Mach64_V303.zip -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/M64 $(SWDIR_NOVA)/MACH64

		unzip -j $(SUPPDIR)/nova/NOVA_COL_171.zip "NOVA_COE.PRG" -d $(SWDIR_NOVA)
		mv $(SWDIR_NOVA)/NOVA_COE.PRG $(SWDIR)/NOVA_COL.ACC
		unzip -j $(SUPPDIR)/nova/vme8_0.zip "VME.8/CPX/*" -d $(SWDIR_NOVA)
		unzip $(SUPPDIR)/nova/XMENU_ENG.zip -d $(SWDIR_NOVA)
		unzip -j $(SUPPDIR)/nova/SAVEOFF.zip -d $(SWDIR_NOVA)

		$(COPY) $(SUPPDIR)/nova/NOVA_COL.INF $(SWDIR)/
		$(COPY) $(SUPPDIR)/nova/STA_VDI.INF $(SWDIR_AUTO)/


	# --------------------------------------------------
	# Eiffel
	# --------------------------------------------------
		unzip $(SUPPDIR)/eiffel1110.zip "eiffel/eiffelcf.app" -d $(SWDIR_TOOLS)
		unzip $(SUPPDIR)/eiffel1110.zip "eiffel/copying" -d $(SWDIR_TOOLS)
		unzip $(SUPPDIR)/eiffel1110.zip "eiffel/eiffel_g.rsc" -d $(SWDIR_TOOLS)
		unzip $(SUPPDIR)/eiffel1110.zip "eiffel/doc/eiffel.hyp" -d $(SWDIR_TOOLS)
		unzip $(SUPPDIR)/eiffel1110.zip "eiffel/doc/eiffel.ref" -d $(SWDIR_TOOLS)
		unzip $(SUPPDIR)/eiffel1110.zip "eiffel/doc/authors.txt" -d $(SWDIR_TOOLS)
		unzip $(SUPPDIR)/eiffel1110.zip "eiffel/doc/Technik.txt" -d $(SWDIR_TOOLS)
		unzip $(SUPPDIR)/eiffel1110.zip "eiffel/french/*" -d $(SWDIR_TOOLS)
		unzip $(SUPPDIR)/eiffel1110.zip "eiffel/german/*" -d $(SWDIR_TOOLS)
		unzip $(SUPPDIR)/eiffel1110.zip "eiffel/tab_keyb/*" -d $(SWDIR_TOOLS)

	# --------------------------------------------------
	# Additional support tools
	# --------------------------------------------------
		unzip -j $(SUPPDIR)/Autosort.zip "AUTOSORT/AUTOSORT.PRG" -d $(SWDIR_TOOLS)


	# --------------------------------------------------
	# ISA drivers
	# --------------------------------------------------

	# NE2000
		mkdir -p $(SWDIR_ISA)/NE2000
		$(COPY) $(ROOTDIR)/sw/isa/drivers/ne2000/ethernec/ENEH.MIF $(SWDIR_ISA)/NE2000/ENEH.XIF
		$(COPY) $(ROOTDIR)/sw/isa/drivers/ne2000/ethernec/ENEH.STX $(SWDIR_ISA)/NE2000/ENEH.STX
		$(COPY) $(ROOTDIR)/sw/isa/drivers/ne2000/ethernec/ETHERNE.TXT $(SWDIR_ISA)/NE2000/
		$(COPY) $(ROOTDIR)/sw/isa/drivers/ne2000/ethernec/COPYING.TXT $(SWDIR_ISA)/NE2000/

	# PicoGUS driver
		mkdir -p $(SWDIR_ISA)/PICOGUS
		$(COPY) $(ROOTDIR)/sw/isa/drivers/picogus/pgusinit/pgusinit.prg $(SWDIR_ISA)/PICOGUS/PGUSINIT.PRG
		$(COPY) $(ROOTDIR)/sw/isa/drivers/picogus/pgusinit/README.md $(SWDIR_ISA)/PICOGUS/README.TXT

	# GUS driver
		mkdir -p $(SWDIR_ISA)/GUS
		$(COPY) $(ROOTDIR)/sw/isa/drivers/gus/ultrinit.prg $(SWDIR_ISA)/GUS/ULTRINIT.PRG







# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995, 1996, 1997 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
#
# This file file be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.
#
# - Modified by Owen Taylor <otaylor@redhat.com> to use GETTEXT_PACKAGE
#   instead of PACKAGE and to look for po2tbl in ./ not in intl/
#
# - Modified by jacob berkman <jacob@ximian.com> to install
#   Makefile.in.in and po2tbl.sed.in for use with glib-gettextize

GETTEXT_PACKAGE = vips7
PACKAGE = vips
VERSION = 7.22.1

SHELL = /bin/sh


srcdir = .
top_srcdir = ..


prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = ${prefix}/share
datadir = ${datarootdir}
libdir = ${exec_prefix}/lib
localedir = $(libdir)/locale
gnulocaledir = $(datadir)/locale
gettextsrcdir = $(datadir)/glib-2.0/gettext/po
subdir = po

INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
MKINSTALLDIRS = $(top_srcdir)/.//mkinstalldirs

CC = gcc
GENCAT = @GENCAT@
GMSGFMT = /usr/bin/msgfmt
MSGFMT = /usr/bin/msgfmt
MSGFMT_OPTS = -c
XGETTEXT = /usr/bin/xgettext
MSGMERGE = msgmerge

DEFS = -DHAVE_CONFIG_H
CFLAGS = -g -O2
CPPFLAGS = 

INCLUDES = -I.. -I$(top_srcdir)/intl

COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)

SOURCES = 
POFILES =  en_GB.po de.po
GMOFILES =  en_GB.gmo de.gmo
DISTFILES = LINGUAS ChangeLog Makefile.in.in POTFILES.in $(GETTEXT_PACKAGE).pot \
$(POFILES) $(GMOFILES) $(SOURCES)

POTFILES = \
	../libvips/acquire/im_clamp.c \
	../libvips/arithmetic/im_deviate.c \
	../libvips/arithmetic/im_point_bilinear.c \
	../libvips/arithmetic/im_linreg.c \
	../libvips/arithmetic/arith_dispatch.c \
	../libvips/arithmetic/im_subtract.c \
	../libvips/arithmetic/im_stats.c \
	../libvips/arithmetic/im_maxpos.c \
	../libvips/arithmetic/im_maxpos_avg.c \
	../libvips/arithmetic/im_maxpos_vec.c \
	../libvips/arithmetic/im_invert.c \
	../libvips/arithmetic/im_sign.c \
	../libvips/arithmetic/round.c \
	../libvips/arithmetic/im_measure.c \
	../libvips/arithmetic/math.c \
	../libvips/arithmetic/im_minpos.c \
	../libvips/arithmetic/im_abs.c \
	../libvips/arithmetic/im_recomb.c \
	../libvips/arithmetic/im_cross_phase.c \
	../libvips/arithmetic/im_divide.c \
	../libvips/arithmetic/im_bandmean.c \
	../libvips/arithmetic/im_lintra.c \
	../libvips/arithmetic/im_remainder.c \
	../libvips/arithmetic/power.c \
	../libvips/arithmetic/im_add.c \
	../libvips/arithmetic/im_avg.c \
	../libvips/arithmetic/im_multiply.c \
	../libvips/boolean/boolean.c \
	../libvips/boolean/bool_dispatch.c \
	../libvips/cimg/cimg_dispatch.c \
	../libvips/colour/im_rad2float.c \
	../libvips/colour/im_LCh2UCS.c \
	../libvips/colour/im_icc_transform.c \
	../libvips/colour/disp.c \
	../libvips/colour/im_dE_fromLab.c \
	../libvips/colour/im_Yxy2XYZ.c \
	../libvips/colour/im_LabQ2disp.c \
	../libvips/colour/im_Lab2LabS.c \
	../libvips/colour/im_LabQ2Lab.c \
	../libvips/colour/im_UCS2LCh.c \
	../libvips/colour/im_LabS2Lab.c \
	../libvips/colour/im_dECMC_fromLab.c \
	../libvips/colour/im_LabQ2LabS.c \
	../libvips/colour/im_Lab2XYZ.c \
	../libvips/colour/im_disp2XYZ.c \
	../libvips/colour/im_XYZ2disp.c \
	../libvips/colour/colour.c \
	../libvips/colour/im_float2rad.c \
	../libvips/colour/im_Lab2LCh.c \
	../libvips/colour/colour_dispatch.c \
	../libvips/colour/derived.c \
	../libvips/colour/im_LabS2LabQ.c \
	../libvips/colour/im_dE00_fromLab.c \
	../libvips/colour/im_lab_morph.c \
	../libvips/colour/im_XYZ2Yxy.c \
	../libvips/colour/im_Lab2LabQ.c \
	../libvips/colour/im_LCh2Lab.c \
	../libvips/colour/im_XYZ2Lab.c \
	../libvips/conversion/im_insert.c \
	../libvips/conversion/im_mask2vips.c \
	../libvips/conversion/im_gaussnoise.c \
	../libvips/conversion/im_flipver.c \
	../libvips/conversion/im_falsecolour.c \
	../libvips/conversion/im_vips2mask.c \
	../libvips/conversion/im_clip2fmt.c \
	../libvips/conversion/im_ri2c.c \
	../libvips/conversion/im_system.c \
	../libvips/conversion/im_text.c \
	../libvips/conversion/im_zoom.c \
	../libvips/conversion/conver_dispatch.c \
	../libvips/conversion/im_copy_file.c \
	../libvips/conversion/im_c2real.c \
	../libvips/conversion/im_grid.c \
	../libvips/conversion/im_c2imag.c \
	../libvips/conversion/im_scaleps.c \
	../libvips/conversion/im_fliphor.c \
	../libvips/conversion/im_scale.c \
	../libvips/conversion/im_black.c \
	../libvips/conversion/im_tbjoin.c \
	../libvips/conversion/im_replicate.c \
	../libvips/conversion/im_embed.c \
	../libvips/conversion/im_lrjoin.c \
	../libvips/conversion/im_rot180.c \
	../libvips/conversion/im_extract.c \
	../libvips/conversion/im_wrap.c \
	../libvips/conversion/im_c2rect.c \
	../libvips/conversion/im_copy.c \
	../libvips/conversion/im_rot90.c \
	../libvips/conversion/im_c2amph.c \
	../libvips/conversion/im_msb.c \
	../libvips/conversion/im_system_image.c \
	../libvips/conversion/im_subsample.c \
	../libvips/conversion/im_rot270.c \
	../libvips/conversion/im_gbandjoin.c \
	../libvips/convolution/im_contrast_surface.c \
	../libvips/convolution/im_gradcor.c \
	../libvips/convolution/im_compass.c \
	../libvips/convolution/im_convsep.c \
	../libvips/convolution/im_conv.c \
	../libvips/convolution/im_fastcor.c \
	../libvips/convolution/convol_dispatch.c \
	../libvips/convolution/im_conv_f.c \
	../libvips/convolution/im_addgnoise.c \
	../libvips/convolution/im_spcor.c \
	../libvips/convolution/im_convsep_f.c \
	../libvips/convolution/im_sharpen.c \
	../libvips/format/im_tile_cache.c \
	../libvips/format/im_ppm2vips.c \
	../libvips/format/format.c \
	../libvips/format/im_vips2tiff.c \
	../libvips/format/im_vips2csv.c \
	../libvips/format/im_vips2ppm.c \
	../libvips/format/im_png2vips.c \
	../libvips/format/format_dispatch.c \
	../libvips/format/im_analyze2vips.c \
	../libvips/format/im_magick2vips.c \
	../libvips/format/im_vips2raw.c \
	../libvips/format/im_tiff2vips.c \
	../libvips/format/im_exr2vips.c \
	../libvips/format/matlab.c \
	../libvips/format/im_jpeg2vips.c \
	../libvips/format/im_csv2vips.c \
	../libvips/format/im_vips2png.c \
	../libvips/format/radiance.c \
	../libvips/format/im_raw2vips.c \
	../libvips/format/im_vips2jpeg.c \
	../libvips/freq_filt/im_freq_mask.c \
	../libvips/freq_filt/im_phasecor_fft.c \
	../libvips/freq_filt/fft_sp.c \
	../libvips/freq_filt/fmaskcir.c \
	../libvips/freq_filt/im_fractsurf.c \
	../libvips/freq_filt/im_freqflt.c \
	../libvips/freq_filt/fmask4th.c \
	../libvips/freq_filt/im_invfft.c \
	../libvips/freq_filt/freq_dispatch.c \
	../libvips/freq_filt/im_rotquad.c \
	../libvips/freq_filt/im_invfftr.c \
	../libvips/freq_filt/im_fwfft.c \
	../libvips/freq_filt/im_disp_ps.c \
	../libvips/histograms_lut/im_gammacorrect.c \
	../libvips/histograms_lut/im_hist.c \
	../libvips/histograms_lut/im_project.c \
	../libvips/histograms_lut/im_stdif.c \
	../libvips/histograms_lut/im_mpercent.c \
	../libvips/histograms_lut/im_histnD.c \
	../libvips/histograms_lut/im_histspec.c \
	../libvips/histograms_lut/im_identity.c \
	../libvips/histograms_lut/im_histindexed.c \
	../libvips/histograms_lut/hist_dispatch.c \
	../libvips/histograms_lut/im_hsp.c \
	../libvips/histograms_lut/im_histgr.c \
	../libvips/histograms_lut/im_buildlut.c \
	../libvips/histograms_lut/tone.c \
	../libvips/histograms_lut/im_histeq.c \
	../libvips/histograms_lut/im_histplot.c \
	../libvips/histograms_lut/im_lhisteq.c \
	../libvips/histograms_lut/im_invertlut.c \
	../libvips/histograms_lut/im_heq.c \
	../libvips/histograms_lut/im_maplut.c \
	../libvips/inplace/smudge_area.c \
	../libvips/inplace/flood.c \
	../libvips/inplace/im_insertplace.c \
	../libvips/inplace/plot_point.c \
	../libvips/inplace/line_draw.c \
	../libvips/inplace/im_plotmask.c \
	../libvips/inplace/im_paintrect.c \
	../libvips/inplace/inplace_dispatch.c \
	../libvips/inplace/im_circle.c \
	../libvips/iofuncs/check.c \
	../libvips/iofuncs/sink.c \
	../libvips/iofuncs/rect.c \
	../libvips/iofuncs/semaphore.c \
	../libvips/iofuncs/memory.c \
	../libvips/iofuncs/region.c \
	../libvips/iofuncs/im_setbuf.c \
	../libvips/iofuncs/callback.c \
	../libvips/iofuncs/im_wrapmany.c \
	../libvips/iofuncs/im_guess_prefix.c \
	../libvips/iofuncs/im_generate.c \
	../libvips/iofuncs/init.c \
	../libvips/iofuncs/sinkscreen.c \
	../libvips/iofuncs/im_mapfile.c \
	../libvips/iofuncs/package.c \
	../libvips/iofuncs/debug.c \
	../libvips/iofuncs/im_init_world.c \
	../libvips/iofuncs/im_partial.c \
	../libvips/iofuncs/meta.c \
	../libvips/iofuncs/im_setupout.c \
	../libvips/iofuncs/base64.c \
	../libvips/iofuncs/time.c \
	../libvips/iofuncs/im_histlin.c \
	../libvips/iofuncs/object.c \
	../libvips/iofuncs/im_image.c \
	../libvips/iofuncs/im_cp_desc.c \
	../libvips/iofuncs/buf.c \
	../libvips/iofuncs/util.c \
	../libvips/iofuncs/im_prepare.c \
	../libvips/iofuncs/im_binfile.c \
	../libvips/iofuncs/header.c \
	../libvips/iofuncs/sinkmemory.c \
	../libvips/iofuncs/window.c \
	../libvips/iofuncs/sinkdisc.c \
	../libvips/iofuncs/im_demand_hint.c \
	../libvips/iofuncs/buffer.c \
	../libvips/iofuncs/im_open.c \
	../libvips/iofuncs/error.c \
	../libvips/iofuncs/dispatch_types.c \
	../libvips/iofuncs/threadpool.c \
	../libvips/iofuncs/im_writeline.c \
	../libvips/iofuncs/im_open_vips.c \
	../libvips/iofuncs/im_unmapfile.c \
	../libvips/iofuncs/im_close.c \
	../libvips/mask/im_matcat.c \
	../libvips/mask/rw_mask.c \
	../libvips/mask/im_matinv.c \
	../libvips/mask/im_mattrn.c \
	../libvips/mask/im_matmul.c \
	../libvips/mask/im_logmasks.c \
	../libvips/mask/im_gaussmasks.c \
	../libvips/mask/rotmask.c \
	../libvips/mask/matalloc.c \
	../libvips/mask/mask_dispatch.c \
	../libvips/morphology/im_profile.c \
	../libvips/morphology/im_label_regions.c \
	../libvips/morphology/morph_dispatch.c \
	../libvips/morphology/im_zerox.c \
	../libvips/morphology/im_erode.c \
	../libvips/morphology/im_cntlines.c \
	../libvips/morphology/im_rank.c \
	../libvips/morphology/im_dilate.c \
	../libvips/morphology/im_rank_image.c \
	../libvips/mosaicing/mosaicing_dispatch.c \
	../libvips/mosaicing/im_lrmerge.c \
	../libvips/mosaicing/im_remosaic.c \
	../libvips/mosaicing/im_tbmosaic.c \
	../libvips/mosaicing/im_chkpair.c \
	../libvips/mosaicing/im_tbcalcon.c \
	../libvips/mosaicing/global_balance.c \
	../libvips/mosaicing/im_tbmerge.c \
	../libvips/mosaicing/im_maxpos_subpel.c \
	../libvips/mosaicing/im_improve.c \
	../libvips/mosaicing/im_avgdxdy.c \
	../libvips/mosaicing/im_align_bands.c \
	../libvips/mosaicing/im_lrcalcon.c \
	../libvips/mosaicing/im_initialize.c \
	../libvips/mosaicing/match.c \
	../libvips/mosaicing/mosaic1.c \
	../libvips/mosaicing/im_clinear.c \
	../libvips/mosaicing/im_lrmosaic.c \
	../libvips/other/im_simcontr.c \
	../libvips/other/glds_funcs.c \
	../libvips/other/cooc_funcs.c \
	../libvips/other/im_grey.c \
	../libvips/other/other_dispatch.c \
	../libvips/other/im_benchmark.c \
	../libvips/other/im_meanstd.c \
	../libvips/other/im_zone.c \
	../libvips/other/im_sines.c \
	../libvips/other/im_eye.c \
	../libvips/other/im_spatres.c \
	../libvips/other/im_make_xy.c \
	../libvips/other/im_dif_std.c \
	../libvips/relational/im_ifthenelse.c \
	../libvips/relational/relational_dispatch.c \
	../libvips/relational/relational.c \
	../libvips/relational/im_blend.c \
	../libvips/resample/im_affine.c \
	../libvips/resample/im_stretch3.c \
	../libvips/resample/transform.c \
	../libvips/resample/im_shrink.c \
	../libvips/resample/resample_dispatch.c \
	../libvips/resample/interpolate.c \
	../libvips/resample/im_rightshift_size.c \
	../libvips/video/video_dispatch.c \
	../libvips/video/im_video_test.c \
	../libvips/video/im_video_v4l1.c \
	../tools/iofuncs/vips.c \
	../tools/iofuncs/edvips.c \
	../tools/iofuncs/vipsthumbnail.c \
	../tools/iofuncs/header.c \
	../tools/mosaicing/find_mosaic.c \
	../tools/mosaicing/mergeup.c \
	../tools/other/simcontr.c \
	../tools/other/squares.c \
	../tools/other/sines.c \
	../tools/other/glds_features.c \
	../tools/other/glds.c \
	../tools/other/spatres.c \
	../tools/other/cooc.c \
	../tools/other/cooc_features.c \
	../libvipsCC/VDisplay.cc \
	../libvipsCC/VError.cc \
	../libvipsCC/VImage.cc \
	../libvipsCC/vipsc++.cc \
	../libvipsCC/VMask.cc \
	../libvipsCC/include/vips/VDisplay.h \
	../libvipsCC/include/vips/VImage.h \
	../libvipsCC/include/vips/vipscpp.h \
	../libvipsCC/include/vips/VError.h \
	../libvipsCC/include/vips/VMask.h \
	../libvipsCC/include/vips/vipsc++.h \
	../libvips/cimg/CImg.h \
	../libvips/format/dbh.h \
	../libvips/include/vips/type.h \
	../libvips/include/vips/struct.h \
	../libvips/include/vips/deprecated.h \
	../libvips/include/vips/image.h \
	../libvips/include/vips/mosaicing.h \
	../libvips/include/vips/freq_filt.h \
	../libvips/include/vips/buf.h \
	../libvips/include/vips/threadpool.h \
	../libvips/include/vips/header.h \
	../libvips/include/vips/inlines.h \
	../libvips/include/vips/generate.h \
	../libvips/include/vips/cimg_funcs.h \
	../libvips/include/vips/object.h \
	../libvips/include/vips/other.h \
	../libvips/include/vips/util.h \
	../libvips/include/vips/inplace.h \
	../libvips/include/vips/version.h \
	../libvips/include/vips/private.h \
	../libvips/include/vips/boolean.h \
	../libvips/include/vips/conversion.h \
	../libvips/include/vips/almostdeprecated.h \
	../libvips/include/vips/resample.h \
	../libvips/include/vips/dispatch.h \
	../libvips/include/vips/rect.h \
	../libvips/include/vips/semaphore.h \
	../libvips/include/vips/disp.h \
	../libvips/include/vips/colour.h \
	../libvips/include/vips/intl.h \
	../libvips/include/vips/region.h \
	../libvips/include/vips/morphology.h \
	../libvips/include/vips/format.h \
	../libvips/include/vips/internal.h \
	../libvips/include/vips/interpolate.h \
	../libvips/include/vips/check.h \
	../libvips/include/vips/memory.h \
	../libvips/include/vips/meta.h \
	../libvips/include/vips/transform.h \
	../libvips/include/vips/error.h \
	../libvips/include/vips/convolution.h \
	../libvips/include/vips/callback.h \
	../libvips/include/vips/mask.h \
	../libvips/include/vips/vips.h \
	../libvips/include/vips/debug.h \
	../libvips/include/vips/histograms_lut.h \
	../libvips/include/vips/thread.h \
	../libvips/include/vips/relational.h \
	../libvips/include/vips/video.h \
	../libvips/include/vips/arithmetic.h \
	../libvips/iofuncs/base64.h \
	../libvips/mosaicing/mosaic.h \
	../libvips/mosaicing/merge.h \
	../libvips/mosaicing/global_balance.h \
	../libvips/resample/templates.h \
	../libvips/video/im_video_v4l1.h \
	../libvips/cimg/cimg.cpp \
	../libvips/resample/bicubic.cpp \
	../libvips/resample/nohalo.cpp \
	../libvips/resample/vsqbs.cpp \
	../libvips/resample/lbb.cpp

CATALOGS =  en_GB.gmo de.gmo
CATOBJEXT = .gmo
INSTOBJEXT = .mo

.SUFFIXES:
.SUFFIXES: .c .o .po .pox .gmo .mo .msg .cat

.c.o:
	$(COMPILE) $<

.po.pox:
	$(MAKE) $(GETTEXT_PACKAGE).pot
	$(MSGMERGE) $< $(srcdir)/$(GETTEXT_PACKAGE).pot -o $*.pox

.po.mo:
	$(MSGFMT) -o $@ $<

.po.gmo:
	file=$(srcdir)/`echo $* | sed 's,.*/,,'`.gmo \
	  && rm -f $$file && $(GMSGFMT) $(MSGFMT_OPTS) -o $$file $<

.po.cat:
	sed -f ../intl/po2msg.sed < $< > $*.msg \
	  && rm -f $@ && $(GENCAT) $@ $*.msg


all: all-yes

all-yes: $(CATALOGS)
all-no:

$(srcdir)/$(GETTEXT_PACKAGE).pot: $(POTFILES)
	$(XGETTEXT) --default-domain=$(GETTEXT_PACKAGE) --directory=$(top_srcdir) \
	  --add-comments --keyword=_ --keyword=N_ \
	  --keyword=C_:1c,2 \
	  --keyword=NC_:1c,2 \
	  --keyword=g_dngettext:2,3 \
	  --flag=N_:1:pass-c-format \
	  --flag=C_:2:pass-c-format \
	  --flag=NC_:2:pass-c-format \
	  --flag=g_dngettext:2:pass-c-format \
          --flag=g_strdup_printf:1:c-format \
          --flag=g_string_printf:2:c-format \
          --flag=g_string_append_printf:2:c-format \
          --flag=g_error_new:3:c-format \
          --flag=g_set_error:4:c-format \
          --flag=g_markup_printf_escaped:1:c-format \
          --flag=g_log:3:c-format \
          --flag=g_print:1:c-format \
          --flag=g_printerr:1:c-format \
          --flag=g_printf:1:c-format \
          --flag=g_fprintf:2:c-format \
          --flag=g_sprintf:2:c-format \
          --flag=g_snprintf:3:c-format \
          --flag=g_scanner_error:2:c-format \
          --flag=g_scanner_warn:2:c-format \
	  --files-from=$(srcdir)/POTFILES.in \
	&& test ! -f $(GETTEXT_PACKAGE).po \
	   || ( rm -f $(srcdir)/$(GETTEXT_PACKAGE).pot \
		&& mv $(GETTEXT_PACKAGE).po $(srcdir)/$(GETTEXT_PACKAGE).pot )

install: install-exec install-data
install-exec:
install-data: install-data-yes
install-data-no: all
install-data-yes: all
	if test -r "$(MKINSTALLDIRS)"; then \
	  $(MKINSTALLDIRS) $(DESTDIR)$(datadir); \
	else \
	  $(SHELL) $(top_srcdir)/mkinstalldirs $(DESTDIR)$(datadir); \
	fi
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  case "$$cat" in \
	    *.gmo) destdir=$(gnulocaledir);; \
	    *)     destdir=$(localedir);; \
	  esac; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  dir=$(DESTDIR)$$destdir/$$lang/LC_MESSAGES; \
	  if test -r "$(MKINSTALLDIRS)"; then \
	    $(MKINSTALLDIRS) $$dir; \
	  else \
	    $(SHELL) $(top_srcdir)/mkinstalldirs $$dir; \
	  fi; \
	  if test -r $$cat; then \
	    $(INSTALL_DATA) $$cat $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	    echo "installing $$cat as $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT)"; \
	  else \
	    $(INSTALL_DATA) $(srcdir)/$$cat $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	    echo "installing $(srcdir)/$$cat as" \
		 "$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT)"; \
	  fi; \
	  if test -r $$cat.m; then \
	    $(INSTALL_DATA) $$cat.m $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	    echo "installing $$cat.m as $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m"; \
	  else \
	    if test -r $(srcdir)/$$cat.m ; then \
	      $(INSTALL_DATA) $(srcdir)/$$cat.m \
		$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	      echo "installing $(srcdir)/$$cat as" \
		   "$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m"; \
	    else \
	      true; \
	    fi; \
	  fi; \
	done
	if test "$(PACKAGE)" = "glib"; then \
	  if test -r "$(MKINSTALLDIRS)"; then \
	    $(MKINSTALLDIRS) $(DESTDIR)$(gettextsrcdir); \
	  else \
	    $(SHELL) $(top_srcdir)/mkinstalldirs $(DESTDIR)$(gettextsrcdir); \
	  fi; \
	  $(INSTALL_DATA) $(srcdir)/Makefile.in.in \
			  $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	else \
	  : ; \
	fi

# Define this as empty until I found a useful application.
installcheck:

uninstall:
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	done
	if test "$(PACKAGE)" = "glib"; then \
	  rm -f $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	fi

check: all

dvi info tags TAGS ID:

mostlyclean:
	rm -f core core.* *.pox $(GETTEXT_PACKAGE).po *.old.po cat-id-tbl.tmp
	rm -fr *.o

clean: mostlyclean

distclean: clean
	rm -f Makefile Makefile.in POTFILES *.mo *.msg *.cat *.cat.m

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f $(GMOFILES)

distdir = ../$(GETTEXT_PACKAGE)-$(VERSION)/$(subdir)
dist distdir: update-po $(DISTFILES)
	dists="$(DISTFILES)"; \
	for file in $$dists; do \
	  ln $(srcdir)/$$file $(distdir) 2> /dev/null \
	    || cp -p $(srcdir)/$$file $(distdir); \
	done

update-po: Makefile
	$(MAKE) $(GETTEXT_PACKAGE).pot
	tmpdir=`pwd`; \
	cd $(srcdir); \
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  echo "$$lang:"; \
	  if $(MSGMERGE) $$lang.po $(GETTEXT_PACKAGE).pot -o $$tmpdir/$$lang.new.po; then \
	    if cmp $$lang.po $$tmpdir/$$lang.new.po >/dev/null 2>&1; then \
	      rm -f $$tmpdir/$$lang.new.po; \
            else \
	      if mv -f $$tmpdir/$$lang.new.po $$lang.po; then \
	        :; \
	      else \
	        echo "msgmerge for $$lang.po failed: cannot move $$tmpdir/$$lang.new.po to $$lang.po" 1>&2; \
	        rm -f $$tmpdir/$$lang.new.po; \
	        exit 1; \
	      fi; \
	    fi; \
	  else \
	    echo "msgmerge for $$cat failed!"; \
	    rm -f $$tmpdir/$$lang.new.po; \
	  fi; \
	done

# POTFILES is created from POTFILES.in by stripping comments, empty lines
# and Intltool tags (enclosed in square brackets), and appending a full
# relative path to them
POTFILES: POTFILES.in
	( if test 'x$(srcdir)' != 'x.'; then \
	    posrcprefix='$(top_srcdir)/'; \
	  else \
	    posrcprefix="../"; \
	  fi; \
	  rm -f $@-t $@ \
	    && (sed -e '/^#/d' 						\
		    -e "s/^\[.*\] +//" 					\
		    -e '/^[ 	]*$$/d' 				\
		    -e "s@.*@	$$posrcprefix& \\\\@" < $(srcdir)/$@.in	\
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

Makefile: Makefile.in.in ../config.status POTFILES
	cd .. \
	  && $(SHELL) ./config.status $(subdir)/$@.in

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:

include $(top_srcdir)/example/Makefile.inc

LDADD += -lm

bin_PROGRAMS = model_explorer simple_linear mnist

simple_linear_SOURCES = simple_linear.c model_read.c model_read.h
model_explorer_SOURCES = model_explorer.c model_read.c model_read.h
mnist_SOURCES = mnist.c model_read.c model_read.h

EXTRA_DIST = odp_ml_run.sh simple_linear.onnx example_digit.csv

if have_png
bin_PROGRAMS += style_transfer
style_transfer_SOURCES = style_transfer.c model_read.c model_read.h
LDADD += -lpng
EXTRA_DIST += ODP-Logo-HQ.png odp_ml_run_style_transfer.sh
endif

if test_example
TESTS = odp_ml_run.sh
if have_png
TESTS += odp_ml_run_style_transfer.sh
endif
endif

# If building out-of-tree, make check will not copy the scripts and data to the
# $(builddir) assuming that all commands are run locally. However this prevents
# running tests on a remote target using LOG_COMPILER.
# So copy all script and data files explicitly here.
all-local:
	if [ "x$(srcdir)" != "x$(builddir)" ]; then \
		for f in $(EXTRA_DIST); do \
			if [ -e $(srcdir)/$$f ]; then \
				mkdir -p $(builddir)/$$(dirname $$f); \
				cp -f $(srcdir)/$$f $(builddir)/$$f; \
			fi \
		done \
	fi

clean-local:
	if [ "x$(srcdir)" != "x$(builddir)" ]; then \
		for f in $(EXTRA_DIST); do \
			rm -f $(builddir)/$$f; \
		done \
	fi

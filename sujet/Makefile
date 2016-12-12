.PHONY  : all
CP = cp
FIG2DEV = fig2dev
INKSCAPE= inkscape
PDFLATEX = TEXINPUTS=..:: pdflatex

SCHEMA := $(wildcard *.fig) 
DIAGRAM := $(wildcard *.svg) 
PDF = $(DIAGRAM:%.svg=%.pdf)
PDFTEX = $(SCHEMA:%.fig=%.pdf)
PDFTEX_T = $(SCHEMA:%.fig=%.pdftex_t)

%.pdf : %.svg
	$(INKSCAPE) -T --file=$< --export-pdf=$@

%.pdf: %.fig
	$(FIG2DEV) -L pdftex $< > $@

%.pdftex_t: %.fig %.pdf
	$(FIG2DEV) -L pdftex_t -p $*.pdf $< > $@

tp-branch.pdf : tp-branch.tex $(PDFTEX) $(PDFTEX_T) $(PDF)
	$(PDFLATEX) $<
	$(PDFLATEX) $<


clean:
	$(RM) *aux *log *nav *out *snv *toc *snm *dvi *bak *vrb

realclean: clean
	$(RM) tp-branch.pdf $(PDFTEX) $(PDFTEX_T) *.pdf

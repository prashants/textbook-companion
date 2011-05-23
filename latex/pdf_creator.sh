$ cat changedir.sh
#!/bin/bash

./latex_test.sh $1 $2 $3 $4
pdflatex TEX_final.tex
pdflatex TEX_final.tex
pdflatex TEX_final.tex
pdflatex TEX_final.tex
#rm log.txt

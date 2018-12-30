#!/bin/bash
git pull
cd ./client/subcriber
pwd
make TARGET=z1 clean &> /dev/null
make TARGET=sky clean &> /dev/null
cd ../publisher/
pwd
make TARGET=z1 clean
make TARGET=sky clean
cd ../..
cd rpl-border-router
make TARGET=z1 clean
make TARGET=sky clean
cd ..
cd presentation
mkdir out
pdflatex -aux-directory out/ -output-directory out/ -quiet presentation.tex
pdflatex -aux-directory out/ -output-directory out/ -quiet presentation.tex
cp out/presentation.pdf ./
rm -rf out/
cd ..
cd docs
mkdir out
pdflatex -aux-directory out/ -output-directory out/ -quiet main.tex
pdflatex -aux-directory out/ -output-directory out/ -quiet main.tex
cp out/main.pdf ./
rm -rf out/
cd ..
pwd
tar -cvzf coap-pub-sub.tar.gz *

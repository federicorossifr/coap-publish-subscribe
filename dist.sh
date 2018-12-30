#!/bin/bash
git pull
cd ./client/subscriber
echo Cleaning...
pwd
make TARGET=z1 clean &> /dev/null
make TARGET=sky clean &> /dev/null
cd ../publisher/
echo Cleaning...
pwd
make TARGET=z1 clean &> /dev/null
make TARGET=sky clean &> /dev/null
cd ../..
cd rpl-border-router
echo Cleaning...
pwd
make TARGET=z1 clean &> /dev/null
make TARGET=sky clean &> /dev/null
cd ..
cd presentation
echo Building...
pwd
mkdir out
pdflatex -aux-directory out/ -output-directory out/ -quiet presentation.tex
pdflatex -aux-directory out/ -output-directory out/ -quiet presentation.tex
cp out/presentation.pdf ./
rm -rf out/
cd ..
cd docs
echo Building...
pwd
mkdir out
pdflatex -aux-directory out/ -output-directory out/ -quiet main.tex
pdflatex -aux-directory out/ -output-directory out/ -quiet main.tex
cp out/main.pdf ./
rm -rf out/
cd ..
echo Generating archive...
pwd
tar -czf coap-pub-sub.tar.gz *
echo Done
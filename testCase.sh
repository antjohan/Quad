cd ~/git
rm -r -f Quad
git clone https://github.com/antjohan/Quad.git
cd Quad
gcc testCases.c -o ./testQuad -std=c99 -lwiringPi -lm
./testQuad

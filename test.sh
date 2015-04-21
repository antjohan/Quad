cd ~/git
rm -r -f Quad
git clone https://github.com/antjohan/Quad.git
cd Quad
gcc AutoQuad.c -o ./AutoQuad -std=c99 -lwiringPi -lm
./AutoQuad
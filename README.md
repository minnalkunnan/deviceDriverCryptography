Lab 4: deviceDriverCryptography

Authors

Minnal Kunnan
Cody Thompson

Task 1. We modeled our disk system off of simple text files that we open and close with fopen and fwrite and fclose. Everything necessary is implemented and will throw proper errors without exitting when attempting to do unnatural behaviors.

Task 2. We implemented OCB authenticated encryption for our disk system. We kept the tag and nonce in a file that has the same name as the file we create except with the .data extension. This file is acting similarly to the other, in the sense that it represents another, smaller disk of memory. Currently it is paired with the input file and is a combination of n 12 byte blocks (8 for the tag, and 4 for the nonce), where n is the number of 4096 byte blocks in the input file. This also implies that input files must have an extension (any will do, however). Please see the noted future work / problems with our implementation below.

Task 3. Refer to our implementation of OCB in 2

How to use the authenticated encryption:

Currently everything must be done manually, such as specifying the input file name and blocks to write within the main function. We've provided three sample blocks, A, B, and C, that hold 4096 bytes of 'a', 'b', and 'c' characters, respectively. This also includes if you would like to change the key (please see below).

Please put any input files and data files within the files folder provided. We've included a sample file (disk) "easy.in" with one encrypted block written with a 8 byte tag and a nonce value of zero stored within "easy.data". It was encrypted with the key 'deal with it bro'.

Future work / Problems:

1) Currently we are not using a random key. We are forcing it to be 'deal with it bro' in the code. However, randomizing it would not be much extra work, because we already have a representation of paired data in the .data files we create. For example, we could store the 16 byte keys in the data files and have each block be 28 bytes long instead of 12. Then that key would be stored to a direct mapping of the block that was encrypted.

2) Our nonce value is for the most part not a big issue, and we simply increment it based on how many writes are done. However, when the program is restarted, the nonce counter is reset to zero. Therefore, if a program is run and one block written in run A, and then it is reset a new block is written in run B, then both the block written in run A and the one in run B will have been encrypted with the same nonce value. This could be fixed with using a global nonce value that tracks previous runs.

Keep in mind, this program assumes that each input file will be paired with its own data file (see easy.in and easy.data as an example). Because of this the program will fail out if nBytes is zero when mounting a disk if the disk is mounted with an input file that is not paired with its data file. However, if nBytes is specified then the data file will be made automatically.

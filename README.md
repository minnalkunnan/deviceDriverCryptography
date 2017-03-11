# deviceDriverCryptography

1. We modeled our disk system off of simple text files that we open and close with fopen and fwrite and fclose.

2. We implemented Galois Counter Mode (GCM) to ensure authenticated encryption for our disk system. We kept the tag and nonce in a file that has the same name as the file we create except with the .data extension.

3. Refer to our implementation of GCM in 2
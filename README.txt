Tyler Van Ollefen and Paul Turchinetz

The environment that this project was built in tested in is:
Ubuntu 14.04
gcc 4.8

If you encounter any issues while running it, please try running it in the specified environment!
Otherwise rmdir will not work at all.
If you encounter any other issues, please let us know.

To run our shell:
Extract this tar.gz archive into a folder of your choice.
Type "make"
Type "./shell" to run the shell.

(we've included a few copies of Floppy2 for testing purposes)

known bugs:

"cd .." doesn't look nice
	cd /SUBDIR -> cd .. ==> pwd ==> /SUBDIR/..
	(ls will still work properly with it)

pbs prints an extra FAT12 pls help

files without extensions don't work when using ls or cat

rm doesn't work with paths

when using touch, the sector that the new file is in breaks
ditto on mkdir
(ls and cd don't work on directories)

mkdir is not fully implemented

shell seg faults when you press enter with no commands

the tabbing could look nicer - it was done through the emacs editor



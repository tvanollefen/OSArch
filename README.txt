Tyler Van Ollefen and Paul Turchinetz

To run our shell:
Extract this tar.gz archive into a folder of your choice.
Type "make"
Type "./shell" to run the shell.

(type in help for a list of commands that we have so far - it's not done yet)

known bugs:
ls does not do pathing right
	can't ls /SUBDIR/EXAMPLE.C
	can cd /SUBDIR (or cd SUBDIR)
	can ls EXAMPLE.C

"cd .." doesn't look nice
	cd /SUBDIR -> cd .. ==> pwd ==> /SUBDIR/..
	(ls will still work properly with it)

pbs prints an extra FAT12 pls help




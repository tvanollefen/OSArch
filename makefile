shell: cat.c cd.c ls.c pbs.c pfe.c pwd.c rm.c rmdir.c shell.c utilities.c
	gcc cat.c utilities.c -o cat
	gcc cd.c utilities.c -o cd
	gcc ls.c utilities.c -o ls
	gcc pbs.c utilities.c -o pbs
	gcc pfe.c utilities.c -o pfe
	gcc pwd.c -o pwd
#	gcc rm.c utilities.c -o rm
	gcc rmdir.c utilities.c -o rmdir
	gcc shell.c -o shell

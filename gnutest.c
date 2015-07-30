/** 

	This program is an example of how you can use gnuplot to automatically
	generate graphs from a textfile.

	For more information on gnuplot, visit: http://www.gnuplot.info

	By: Karl Ahrendsen (karl.ahrendsen@gmail.com)

**/
#include <stdio.h>


int main(){

	// Open a pointer to a file so that we can assign it something later
	FILE* gnuplot;
	
	// Open the program "gnuplot" and assign it to the pointer
	// so that we can 'talk' to the program. The option "w" 
	// stands for 'write' and allows us to do the talking. 
	gnuplot = popen("gnuplot","w");

	// First check to see if the file was successfully opened. 
	// If the file was NOT successfully opened, gnuplot would
	// have a value of NULL. The expression "!=" reads 'is not
	// equal to', so we check to see if gnuplot is not null. 
	// If gnuplot is not NULL, it was successfully opened, and 
	// we can continue to do what we set out to do.
	if (gnuplot != NULL){
		fprintf(gnuplot, "set terminal dumb\n");	// gnuplot displays information in 'terminals'
													// which are basically ways of writing
													// information. The "dumb" terminal writes
													// information in plain text.

		fprintf(gnuplot, "set output\n");			// The output command tells gnuplot where
													// to write its information. By not putting
													// anything after "output", we are telling
													// the program to write the information 
													// to STDOUT, in this case, the screen.
		fprintf(gnuplot, "plot '/home/pi/RbData/FDayRot2015-07-06_134351.dat'\n");
		fprintf(gnuplot, "unset output\n"); 
		fprintf(gnuplot, "set terminal png\n");
		fprintf(gnuplot, "set output '/home/pi/RbData/gnutest.png'\n");
		fprintf(gnuplot, "plot '/home/pi/RbData/FDayRot2015-07-06_134351.dat'\n");
	}

	pclose(gnuplot);
	return 0;
}

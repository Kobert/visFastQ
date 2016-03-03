# visFastQ
Simple tool for visualizing short reads.

----------------------------------------

This tool utilizes gnuplot to visualize the contents of a *.fastq file containing short reads.

Output are:
      - The quality profile displayed as a boxplot
      - The per site coverage
      - The relative base frequencies per position
      
----------------------------------------

Installation:

Simply run the provided makefile...

(On a 32 bit system use the dedicated makefile_32 file.)
      
----------------------------------------


version 0.1.0

Important: In order to use the program, you must have gnuplot
           (preferebly gnuplot-x11) installed!

Required arguments are:

One of the following input files:
	-s for specifying a file with shortreads (fastq format only).
	-d for specifying a previously obtained data file.
	
Optional arguments:
The output:
	-o specify a prefix for the output names (can include a path).
	   If no Prefix is provided, no *.png files will be written.
	   If the arguement is "-o SOMEPATH/SOMENAME", the following
	   files will be written to the SOMEPATH/ folder:
		   SOMENAME.dataPoints.dat
		   SOMENAME.qualityProfile.png
		   SOMENAME.coverageProfile.png
		   SOMENAME.baseFrequencyProfile.png

-m Specify how many boxes to utilise for the quality profile boxplot.
   (The actual number may be +-1) (default 40) 
-c Conceals output. Does not open a gnuplot plot window. 
   An alternative output must be set with -o instead. 
-h print this help message.


-------------------------------------------
# OMA
OMA photometric image processing software for Xcode version


## Release Notes:

December 2011 -- update oma for newer versions of Xcode

Implement source control with subversion
	Notes:
	svn does not handle resource forks
	the old main.rsrc had no information in it
	to move things out of the resource fork:
	cp main.rsrc/..namedfork/rsrc main.rs
	then it's OK to rename min.'s to main.rsrc

	need to change ./subversion/config so that it does not ignore ".a" files


### v 2.2.0 (Not yet released)

__Bugs fixed:__

Dragging a macro file onto a window now works even if there is a prefix specified.
Don't get as many "Possible Parameter Mismatch" errors when loading up hi-res images.
Now reads color tiff images with 16 bits/color properly.
ROTRGB -- bug fixed that could crash the program.
Changed title of images displayed by dragging to be the last part of the name, rather than starting with the whole file path

`LOOKUP` command now does more error checking.

`GETFUNCTION` prints out more function info.

Fixed bug that would sometimes cause GTEMP to crash.

`BLOCKRGB` now finds the new min/max

Fixed a problem in arithmetic assignment commands where an integer variable would get strange values when set to a floating point expression

Bugs noted but not fixed:

`GWIND` doesn't work, but is still described in the help - search for getwindowdata(

Column plot doesn't work right for window that is scaled down to fractional size (fixed I think)

New Window and Color Image Tools:

 

New Commands:

`ABELPREP clip_value [fill_value]`
 	Finds the maximum pixel value along horizontal lines in the image. If that 
 	maximum is < clip_value, the entire horizontal line is set to fill_value.
 	Default for fill_value is 0.

`ABELRECT rec_width [rec_y0 rec_y1]`
 	Sets the rectangle (of width rec_width) according to the centroid of the image.
 	If rec_y0 and rec_y1 are omitted, they are taken to be the top and bottom of the image

`CLIPBOTTOM clip_value`
	For the current image, set pixels whose value is < clip_value to be equal to clip_value.

`CLIPFBOTTOM clip_fraction`
	For the current image, set pixels whose value is < clip_fraction*image_max to be equal to clip_fraction*image_max.

`FINDBAD Counts`
	Searches the current image buffer for pixels whose value is more than "Counts" above that of its
	nearest eight neighbors. Those pixels are tagged as hot pixels.

`CLEARBAD`
 	Sets pixels tagged as bad to the value of their 8 nearest neighbors. This will not work well if
	there are contiguous bad pixels. For that, consider using the FILBOX or FILMSK commands.

`READBAD filename`
 	Read in bad pixel data from a text file.
	Format is:
	# of bad pixels
	detector_width	detector_height
	bad_pix1_x		bad_pix1_y
	bad_pix2_x		bad_pix2_y
	.
	.
	.

`WRITEBAD filename`
 	save bad pixel data to a text file.
	Format is:
	# of bad pixels
	detector_width	detector_height
	bad_pix1_x		bad_pix1_y
	bad_pix2_x		bad_pix2_y
	.
	.
	.

`DZRGB  npix`
	Using the size specified by the current rectangle, zoom in on an area of an RGB image
	 centered on the cursor. Zoom factor is npix (integer). This is good for getting a
	 close-up of a sub-region of an image. This is the color image version the DZOOM command.

`SBIG command_string [parameters]`
	 command_string specifies the SBIG command (listed below).
 	Only the first 3 characters of SBIG commands matter
 	The values for parameters depend on the particular command.

 	SBIG command_strings:
 
 	`EXP`osure exposure_time
		Specify exposure_time in seconds. Does not take an image.

	 `FIL`ter
		 Specify which fillter to use.

	 `TEM`perature
		 Specify set temperature of the CCD in degrees C.

	 `DIS`connect
		 Close the device and driver.

 	`STA`tus 
		 Print out CCD info, temperature, exposure, etc.

	 ACQuire 
		Get an image using the current settings.

`AINBURST num_channels, num_points, rate`
	Read Analog input on a LabJack U12 in burst mode.
	num_channels: must be 1, 2 or 4
	num_points: the number of points acquired for each channel
	rate: the scan rate (Hz)
	restrictions: 
		num_channels*rate must be 400-8192.
		num_channels*num_points must be 1-4096
		Analog channels 0, 0 and 1, or 0, 1, 2, and 3 are sampled for num_channels equal to 1, 2, or 4 respectively.

`SHELL shell_command`
	Opens a pipe to a shell and sends along the command. Output from the command is echoed to the command window.

`CAPTURE file_name`
	Uses libgphoto2 calls to capture and download an image from a digital camera that is recognized by libgphoto2 (and supports capture).

`CAMLISTSETTINGS`
	Uses gphoto2 calls to list the settings of the camera that can be set, along with their current values.

`CAMGETSETTING  name`
	Uses gphoto2 calls to list the current value of the specified setting, along with the allowed values for resetting it.

`CAMSETSETTING  name  new_setting_value`
	Uses gphoto2 calls to reset the value of the specified setting to the new value. For a listing of valid setting values, use the `GETCAMSETTING` command.

`CAMCLOSE`
	Disconnects from the digital camera accessed through gphoto2.

`DCRAWARGS  [arguments]`
	Allows the user to specify the arguments passed to the dcraw routine, which decodes camera raw files to OMA format. Omitting the argument lists the allowed arguments and gives the current argument settings. The filename field is ignored.

`DOC2RGB c1 c2 c3 c4`
 Treat the image in the current image buffer as a raw document (output from the dcraw routine with options -d or -D selected) 
 and convert it to an RGB image. This is assumed to have a 2 x 2 color matrix of R G B values in a Bayer pattern.
 c1 - c4 have values 0, 1 or 2, corresponding to red, green, and blue. For example if Bayer Matrix is
 G B
 R G
 c1 - c4 should be 1 2 0 1
 Appropriate values depend on the specific camera. (See the output from the `GETRGB` command.)

New features for command interpretation:

`%p`	Filled in with the "save data" prefix

`%q`	Filled in with the "get data" prefix

__Modified Commands__

`RECTANGLE [ulx uly lrx lry]`
	Specify a rectangle that calculations are to be done on. The arguments are upper
	 left x coordinate; upper left y coordinate; lower right x; lower right y. If no arguments are given, the current rectangle is echoed to the terminal. The coordinates of the current rectangle are returned in the command arguments.

`RECTCENTER n m`
	Select rectangle of size n x m about center of image. The coordinates of the rectangle are returned in the command arguments.

`INTEGRATE direction_flag selection_flag do_average`
Sum up the data in the horizontal ( direction_flag=0 ) or vertical (direction_flag=1)
	 direction. The two-dimensional image becomes a single line. If selection_flag=1,
	 the subset of the image specified by the selection rectangle is used. Possible cases
	 are:
	direction_flag = 1 The result is an array in x; sum in y direction
	direction_flag = 0 The result is an array in y; sum in x direction
	selection_box = 1 A selection box specifies what region to sum
	selection_box = 0 Sum all channels or tracks (columns or rows)
	do_average = 1 Average along the integrate direction (divide by box size in integration direction).
	do_average = 0 Just sum, don't average.

`INTFILL direction_flag selection_flag do_average`
	As above, but a two-dimensional image is formed by duplicating the summed (or averaged) values.

__New Menu Command:__
	
Plot Both (Cmnd 3)


### v 2.1.4

#### What's New:

__CoverFlow__

 

For Leopard (Mac OS 10.5.0 or greater) there is now a QuickLook Plugin that allows the user to browse OMA files in CoverFlow from the Finder. Supported file types include OMA data files as well as PIV files. Hit the space bar for a more detailed Preview.

To install the QuickLook Plugin (MacOS 10.5 only), double click "Install QuickLook for OMA files."  The installer will put the plugin (a file named "omalook3.qlgenerator") in the folder /Library/Quicklook.

__Drag and Drop__

Now you can drag and drop files onto any OMA window to open them. Note that dropping tiff, jpeg, or macro files onto the command window will display them there.

__Automatic Display__

When a data file is opened by double clicking, dragging and dropping to an oma window, or dragging onto the oma icon, the files will be opened AND displayed.

__New Commands__

`SATIFFSCALED min max <filename>`
	Convert image to 8 bit and save as a Greyscale TIFF file. Uses the specified min and max for scaling rather than the min and max of the image in the buffer. Results will be between 0 - 255. Depending on min and max, the image may be clipped or the full 8-bit dynamic range may not be used.

`PIVUODETECT`
	A PIV command that implements the Universal Outlier detection algorithm of J. Westerweel and F. Scarano "Universal Outlier Detection for PIV data" Experiments in Fluids [39]:1096-1100 (2005).

`RNDUP`
	Round the DATAWORD values UP to the nearest integer value.
 
`RNDOFF`
	Round the DATAWORD values Down to the nearest integer value.


`TSMOOTH `
	Smooth radius based on x_dim = Temp[0], y_dim = Temp[1]
	This is a function that enables (Rectangular) smoothing of an image, with variable filter size. It works basically like SMOOTH 13 13, say, but it takes the x_dim and y_dim locally based on the values in the  Temporary buffers T[0] and T[1]. It has been used to variably smooth an image based on the local length scale which was modelled.   

`PIXVALUE x y`
	Prints the value of the pixel at location x,y (i.e, at column x and row y). The upper left of an image is at 0,0.
	command_return_1 is the value.
	

__Changes/Corrections to Current Functions__

Changed the way menu commands work for saving a rectangle -- allow the menu command to use the currently defined 
rectangle rather than having to specifically draw one on the screen.  Added error checking to be sure current rectangle is valid.

Corrected problem when reading in TIFF images with 16 bits per sample into intel machines. Also removed the
scale factor of 2 for 16 bits per sample data (not needed with new floating point default data type).

`GTIFF` command now returns the number of samples per pixel as variable command_return_1.

`AOUTPUT v1 v2 [v3 v4]`
	Sends voltages to D/A converters 0 and 1 on a Labjack USB analog/digital I/O device.
 	If a Labjack U3 is present, v1 and v2 are sent to it.
	If there is no U3 but there is a Labjack U12, v1 and v2 are sent to the U12.
	If both U3 and U12 Labjacks are present and all 4 voltages are given, v1 and v2 are sent to the U3 and v3 and v4 are sent to the U12.

`BIT8 [min] [max]`
	Converts the file in the OMA image buffer to have a dynamic range of 0 to 255. That is, 8 unsigned bits per pixel. This conversion is automatically done before saving the image buffer as a TIFF image. If the optional min and max are specified, they are used for the scaling rather than the min and max of the current image buffer.
	 
Fixed bug in ROTCRP command that had it going completely off the rails.

OMA is now tolerant of renaming the application. It used to be that if the application was renamed, various resources (palettes, help files, etc. were not found. That should be fixed now.



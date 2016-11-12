GMUX for FreeBSD
The gmux program can be used on FreeBSD to switch to the iGPU and back,
as well as configure screen brightness.

The program requires root privileges to work since it writes to some
specific "gmux" ports directly. When installing using 'make install',
gmux will be installed as suid root.

COMPILING
Clone or download the source and run

	make install

as root. The program requires root privileges to work.

USAGE
if you run the program without any arguments, gmux will output
the current values. The output can be written to a file and is directly
executable by a shell. E.g.:

	$ /usr/local/bin/gmux
	/usr/local/bin/gmux gpu iGPU
	# max brightness = 110400
	/usr/local/bin/gmux brightness 9000

Saving the current configuration to a file

	/usr/local/bin/gmux > /etc/gmux.savedstate

yields a file which can restore the state at a later time (e.g. at bootup) :

	/bin/sh /etc/gmux.savedstate

- GPU

To configure which GPU to use:

	/usr/local/bin/gmux gpu iGPU
or
	/usr/local/bin/gmux gpu dGPU

To configure the screen brightness, one can try any positive number up
to the maximum brightness as output by gmux without any arguments:

	/usr/local/bin/gmux brightness 8000

Atlernatively, one can prefix the number with a '+' or a '-' to
alter the current brightness. gmux will keep the amount between zero and
the maximum. For example:

	/usr/local/bin/gmux brightness -1000
or
	/usr/local/bin/gmux brightness +1000

THANKS
gmux is simply a combination of snippets of code found on the internet,
rewritten just a bit to make the functionality work on FreeBSD. The reverse
engineering work was done by others. Thanks!

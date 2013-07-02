libWipe for FirefoxOS
=====================

This software uses LD_PRELOAD to hook on 'unlink' and
wipe the contents of the file before removing the file.

WARNING: This library comes with absolutely no warranty.
Depending on storage and filesystem used this kind of
wipe will not be much effective. It's your own responsability
to do the necessary tests to ensure this can protect your
privacy.

Plug your fxos device accessible via usb

	make install
	make release
	make restart

If you want to debug which files are being removed

	make debug
	make restart
	make log

To go back to release mode (no logs)

	make release
	make restart

Enjoy!

  -- pancake'2013

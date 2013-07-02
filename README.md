libWipe for FirefoxOS
=====================

This software uses LD_PRELOAD to hook on 'unlink'
Run file wiper alorithm before removing

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

#!/system/bin/sh
export OLD_LD_PRELOAD="${LD_PRELOAD}"
export LD_PRELOAD="/system/lib/libwipe.so ${LD_PRELOAD}"
if [ -w /data/b2g.log ]; then
	exec /system/b2g/b2g.bin 2>/data/b2g.log
else
	exec /system/b2g/b2g.bin
fi
if [ $? != 0 ]; then
	export OLD_LD_PRELOAD="${LD_PRELOAD}"
	exec /system/b2g/b2g.bin
fi

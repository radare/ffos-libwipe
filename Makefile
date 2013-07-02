NDK_PATH=${HOME}/Downloads/android-ndk-r7b
NDK_CC=${NDK_PATH}/ndk-gcc
CFLAGS+=-Wall -Os

all:
	NDK_ARCH=arm ${NDK_CC} ${CFLAGS} ${LDFLAGS} -shared -fPIC libwipe.c -o libwipe.so

restart:
	${MAKE} stop
	sleep 1
	${MAKE} kill
	sleep 1
	${MAKE} start

start:
	adb shell start b2g

# somewhat buggy coz procname #
stop:
	adb shell stop b2g

kill:
	adb shell killall b2g
	adb shell killall b2g.sh
	adb shell killall b2g.bin

copycat:
	ln -s blob/libwipe.so libwipe.so

install: copycat
	adb remount
	adb push libwipe.so /system/lib/libwipe.so
	adb shell chmod 0755 /system/lib/libwipe.so
	adb push b2g.sh /system/b2g/b2g.sh
	-[ -n "`adb shell 'test -e /system/b2g/b2g.bin || echo wat'`" ] && \
		adb shell mv /system/b2g/b2g /system/b2g/b2g.bin
	adb shell chmod 0755 /system/b2g/b2g.sh
	adb shell rm /system/b2g/b2g
	adb shell ln -s /system/b2g/b2g.sh /system/b2g/b2g
	adb shell chmod 0755 /system/b2g/b2g

uninstall:
	adb remount
	-[ -n "`adb shell 'test -e /system/b2g/b2g.bin || echo wat'`" ] && \
		adb shell mv /system/b2g/b2g /system/b2g/b2g.bin
	adb shell rm /system/b2g/b2g
	adb shell ln -s /system/b2g/b2g.bin /system/b2g/b2g
	adb shell mount -o remount,ro /system

log: debug
	adb shell tail -f /data/b2g.log

debug:
	adb shell touch /data/b2g.log

release:
	adb shell rm /data/b2g.log
	adb shell mount -o remount,ro /system

clean:
	rm *.so

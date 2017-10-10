if [ -d /etc/X11/xinit/xinitrc.d ] ; then
 for f in /etc/X11/xinit/xinitrc.d/?*.sh ; do
  [ -x "$f" ] && . "$f"
 done
 unset f
fi

setterm -blank 0 -powersave off -powerdown 0
xset -dpms
xset s off

~/xtalk/xtalk

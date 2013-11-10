#! /bin/sh
# /etc/init.d/ngrok

### BEGIN INIT INFO
# Provides:          ngrok
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Simple script to start a program at boot
# Description:       A simple script from www.stuffaboutcode.com which will start / stop a program a boot / shutdown.
### END INIT INFO

# If you want a command to always run, put it here

# Carry out specific functions when asked to by the system
case "$1" in
  start)
    echo "Starting ngrok"
    # run application you want to start
    screen -d -m /usr/local/bin/ngrok -config /home/pi/.ngrok start arduino
    ;;
  stop)
    echo "Stopping ngrok"
    # kill application you want to stop
    killall ngrok
    ;;
  *)
    echo "Usage: /etc/init.d/ngrok {start|stop}"
    exit 1
    ;;
esac

exit 0
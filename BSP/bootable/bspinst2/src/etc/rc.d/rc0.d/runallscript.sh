# Copyright (c) VIA Technologies, Inc. All rights reserved.

for i in /etc/rc.d/rc0.d/S??*
do
        # Ignore dangling symlinks for now.
        [ ! -f "$i" ] && continue
        case "$i" in
                *.sh)
                        # Source shell script for speed.
                        (
                                trap - INT QUIT TSTP
                                set start
                                . $i 2>/dev/null
                        )
                        ;;
                *)
                        # No sh extension, so fork subprocess.
                        $i start 2>/dev/null
                        ;;
        esac
done

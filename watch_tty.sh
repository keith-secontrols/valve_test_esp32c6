#!/bin/bash
prev=$(ls /dev/tty* 2>/dev/null)

echo "Watching /dev/tty* for changes (Ctrl+C to stop)..."
echo "Current devices:"
echo "$prev"
echo "---"

while true; do
    sleep 5
    curr=$(ls /dev/tty* 2>/dev/null)
    if [ "$curr" != "$prev" ]; then
        added=$(comm -13 <(echo "$prev") <(echo "$curr"))
        removed=$(comm -23 <(echo "$prev") <(echo "$curr"))
        [ -n "$added" ]   && echo "[+] $(date +%T) appeared:  $added"
        [ -n "$removed" ] && echo "[-] $(date +%T) removed:   $removed"
        prev="$curr"
    fi
done

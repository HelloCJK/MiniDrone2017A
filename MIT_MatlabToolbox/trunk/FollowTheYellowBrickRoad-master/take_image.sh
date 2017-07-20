while true; do
    rm /data/edu/image.uyvy
    yavta  --file=/data/edu/image.uyvy  -fYUYV -s 160x120  /dev/video0 --capture=4 --skip 3 \r
    sleep 2
done

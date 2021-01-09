docker run \
    --name durst_st_db \
    --rm -dit \
    --mount type=bind,source="$(pwd)"/q,target=/home/durst/q \
    durst/space-time-db:0.1 bash

docker run \
    --name durst_st_db \
    --rm -dit \
    --mount type=bind,source="$(pwd)"/q,target=/home/durst/q \
    --mount type=bind,source="$(pwd)"/data,target=/home/durst/big_dev/nba-movement-data/data \
    durst/space-time-db:0.1 bash

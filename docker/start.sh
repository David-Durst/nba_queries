docker run \
    --name durst_st_db_${USER} \
    --rm -dit \
    --mount type=bind,source="$(pwd)"/q,target=/home/durst/q \
    --mount type=bind,source="$(pwd)"/data,target=/home/durst/big_dev/nba-movement-data/data \
    --mount type=bind,source=/home/durst/big_dev/fiodar_data/2_6_21,target=/home/durst/big_dev/fiodar_data/2_6_21 \
    --cap-add=SYS_PTRACE \
    --cap-add=SYS_ADMIN \
    durst/space-time-db:0.1 bash

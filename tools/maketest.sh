# maketest.sh
# Copyright (c) 2024 Ishan Pranav
# Licensed under the MIT license.

# References:
#  - https://unix.stackexchange.com/questions/454962/mounting-volume-partition-with-permissions-for-user
#  - https://superuser.com/questions/320415/mount-device-with-specific-user-rights

if [ -z "$1" ] || [ -z "$2" ]; then
  echo "Usage: $0 <OUT_FILE> <MOUNT_DIR>"
  exit 1
fi

rm -f $1
dd if=/dev/zero of=$1 bs=256k count=1
mkfs.fat -F 32 -f 2 -S 512 -s 1 -R 32 $1
fsck.fat -v $1
mkdir -p $2
mount $1 $2 -o uid=1000,gid=1000
cd $2

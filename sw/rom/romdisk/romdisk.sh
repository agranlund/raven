#!/bin/bash -e
#
# Create a FAT filesystem image suitable for ROM'ing.
#

usage() {
    echo "error: $1"
    cat >/dev/stderr << END

usage: $0 --diskfile <romdisk-file-name> [<file or directory>...]"

END
    exit 1
}

fail() {
    echo "error: $1"
    exit 1
}

require_tool () {
    if [ -z `which $1` ]; then
        fail "required tool $1 not found on your path, maybe it needs to be installed?"
    fi
}

require_tool mformat
require_tool mcopy

copyfiles=()
diskfile=()

while [[ $# -gt 0 ]]; do
    case $1 in
        --diskfile)
            diskfile=$2
            shift
            ;;
        --*)
            usage "unknown option $1"
            ;;
        *)
            copyfiles+=("$1")
            ;;
    esac
    shift
done

#
# Remove any existing disk image file and create a new one.
#
if [ -z ${diskfile} ]; then
    usage "Missing disk image filename."
fi
rm -f ${diskfile}

#
# Size the disk image.
#
# Assuming FAT12:
#   1 boot sector
#  24 FAT sectors max (4096 entries * 12b * 2)
#   2 sectors for the root directory (32 entries max)
# ---
#  27 sectors overhead + 1 spare
#
files_size=`du -cB 512 ${copyfiles[*]} | tail -1 | cut -f 1`
disk_nsectors=$((${files_size} + 28))

if [ ${disk_nsectors} -gt 4090 ]; then
    fail "cluster size > 512B not currently supported, adjust disk_nsectors calculation"
fi

#
# preemptively nuke .DS_Store turds from the source before copying
#
for f in ${copyfiles[*]}; do
    if [ -d $f ]; then
        find $f -name .DS_Store -delete
    fi
done

#
# create the image and copy files
#
rm -f ${diskfile}
export MTOOLS_NO_VFAT=1
mformat -i ${diskfile} -r 2 -C -T ${disk_nsectors}
for f in ${copyfiles[*]}; do
    mcopy -vbsQ -i ${diskfile} $f ::
done

echo done
exit 0

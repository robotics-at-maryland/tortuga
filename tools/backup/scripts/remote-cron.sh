# Only these need to be changed if server configuration changes
BACKUP_LOC=/var/backups/ram
BACKUP_USER=rambackup
BACKUP_TARGET=ram.umd.edu
BACKUP_TOOL_DIR=/home/backup/backup

BACKUP_REMOTE_DIR=/var/backups/ram/current
BACKUP_DIRS="svn trac"

RSYNC=/usr/bin/rsync

# Wait for the remove server to do its backup (15 minutes)
#sleep 900 not needed current set the time manually

# Sync the current backup from the remove computer to this one
$RSYNC -ave ssh --delete $BACKUP_USER@$BACKUP_TARGET:$BACKUP_REMOTE_DIR $BACKUP_LOC

# Run backup script here....
$BACKUP_TOOL_DIR/backup.py -c $BACKUP_TOOL_DIR/backup.cfg
# Only these need to be changed if server configuration changes
BACKUP_LOC=/var/backups/ram
BACKUP_USER=rambackup
BACKUP_TARGET=ram.umd.edu

BACKUP_REMOTE_DIR=/var/backup/ram
BACKUP_DIRS="svn trac"

# Wait for the remove server to do its backup (15 minutes)
sleep 900

for i in $BACKUP_DIRS; do
    rsync -ave ssh --delete $BACKUP_USER@$BACKUP_TARGET:$BACKUP_REMOTE_DIR/$i $BACKUP_LOC
done

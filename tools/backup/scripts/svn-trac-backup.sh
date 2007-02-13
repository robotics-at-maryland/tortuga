# Only these need to be changed if server configuration changes
LOCAL_DIR=/var/backups/ram/current
TRAC_DIR=/var/trac
SVN_DIR=/var/svn
PROJ=mrbc
BACKUP_TOOL_DIR=/home/rambackup/backup-tool
SVNADMIN=/usr/bin/svnadmin
TRACADMIN/usr/bin/trac-admin
#BACKUP_USER=mrbc-backup

# Helper variables
# DATE=`date '+%T_%F'`
SVN_BACKUP_FILE=$LOCAL_DIR/svn
TRAC_BACKUP_FILE=$LOCAL_DIR/trac

# Remove Older backups
rm -rf $SVN_BACKUP_FILE $TRAC_BACKUP_FILE

# Copy files
$SVNADMIN hotcopy $SVN_DIR/$PROJ $SVN_BACKUP_FILE
$TRACADMIN $TRAC_DIR/$PROJ hotcopy $TRAC_BACKUP_FILE

# Run backup script here....
$BACKUP_TOOL_DIR/backup.py -c $BACKUP_TOOL_DIR/backup.cfg

# Change Owner
#chown -R $BACKUP_USER $SVN_BACKUP_FILE $TRAC_BACKUP_FILE

# Set up removal of backups
#echo "rm -rf $SVN_BACKUP_FILE $TRAC_BACKUP_FILE" | at 4:30AM today

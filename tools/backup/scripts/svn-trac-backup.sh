# Only these need to be changed if server configuration changes
TMP_DIR=/tmp
TRAC_DIR=/var/trac
SVN_DIR=/var/svn
PROJ=mrbc
BACKUP_TOOL_DIR=/home/rambackup/backup-tool
#BACKUP_USER=mrbc-backup

# Helper variables
# DATE=`date '+%T_%F'`
SVN_BACKUP_FILE=$TMP_DIR/svn-$PROJ-backup 
TRAC_BACKUP_FILE=$TMP_DIR/trac-$PROJ-backup

# Remove Older backups
rm -rf $SVN_BACKUP_FILE $TRAC_BACKUP_FILE

# Copy files
svnadmin hotcopy $SVN_DIR/$PROJ $SVN_BACKUP_FILE
trac-admin $TRAC_DIR/$PROJ hotcopy $TRAC_BACKUP_FILE

# Run backup script here....
$BACKUP_TOOL_DIR/backup.py -c $BACKUP_TOOL_DIR/backup.cfg

# Remove temporay files
rm -rf $SVN_BACKUP_FILE $TRAC_BACKUP_FILE

# Change Owner
#chown -R $BACKUP_USER $SVN_BACKUP_FILE $TRAC_BACKUP_FILE

# Set up removal of backups
#echo "rm -rf $SVN_BACKUP_FILE $TRAC_BACKUP_FILE" | at 4:30AM today

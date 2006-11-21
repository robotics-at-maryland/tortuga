# Only these need to be changed if server configuration changes
BACKUP_DIR=/tmp
PROJ=mrbc
BACKUP_USER=mrbc-backup
BACKUP_TARGET=nearspace.net
BACKUP_TOOL_DIR=/home/backup/backup

# Helper variables
SVN_BACKUP_FILE=$BACKUP_DIR/svn-$PROJ-backup
TRAC_BACKUP_FILE=$BACKUP_DIR/trac-$PROJ-backup

# Copy files
#scp -r "$BACKUP_USER@$BACKUP_TARGET:$SVN_BACKUP_FILE*" $SVN_BACKUP_FILE
#scp -r "$BACKUP_USER@$BACKUP_TARGET:$TRAC_BACKUP_FILE*" $TRAC_BACKUP_FILE

# Run backup script here....
$BACKUP_TOOL_DIR/backup.py -c $BACKUP_TOOL_DIR/backup.cfg

# Clean up temporary files at 5:00AM
#echo rm -rf $SVN_BACKUP_FILE $TRAC_BACKUP_FILE | at 5:30AM
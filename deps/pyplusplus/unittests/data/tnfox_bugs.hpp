#include <string>
#include <iostream>

namespace FX {

struct FXWinShellLink
{
	struct Header
	{
		unsigned int length;
		char guid[16];
		union Flags
		{
			struct
			{
				unsigned int hasItemIdList : 1;
				unsigned int pointsToFileOrDir : 1;
				unsigned int hasDescription : 1;
				unsigned int hasRelativePath : 1;
				unsigned int hasWorkingDir : 1;
				unsigned int hasCmdLineArgs : 1;
				unsigned int hasCustomIcon : 1;
				unsigned int useWorkingDir : 1;		// Seems to need to be set to enable working dir
				unsigned int unused : 24;
			};
			unsigned int raw;
		} flags;
		union FileAttribs
		{	// = return from GetFileAttributes()
			struct
			{
				unsigned int isReadOnly : 1;
				unsigned int isHidden : 1;
				unsigned int isSystem : 1;
				unsigned int isVolumeLabel : 1;
				unsigned int isDir : 1;
				unsigned int isModified : 1;	// =archive bit set, ie; is a file normally
				unsigned int isEncrypted : 1;
				unsigned int isNormal : 1;	// Doesn't seem to get set
				unsigned int isTemporary : 1;
				unsigned int isSparse : 1;
				unsigned int hasReparsePoint : 1;
				unsigned int isCompressed : 1;
				unsigned int isOffline : 1;
				unsigned int unused : 19;
			};
			unsigned int raw;
		} fileattribs;		// in GetFileAttributes() format
		unsigned long creation, modified, lastAccess;	// in FILETIME format
		unsigned int filelength;
		unsigned int iconno;
		enum ShowWnd
		{
			HIDE=0,
			NORMAL,
			SHOWMINIMIZED,
			SHOWMAXIMIZED
		};
		ShowWnd showWnd;
		unsigned int hotkey;
		unsigned int unknown1, unknown2;

		Header();
        
		friend std::ostream &operator<<(std::ostream &s, const Header &i);
		friend std::istream &operator>>(std::istream &s, Header &i);
	} header;
	
    struct ItemIdListTag
	{
		unsigned short length;
		char path1[260];			// In ASCII
		char path2[260];		// In unicode
		ItemIdListTag();
		char originalPath[260];		// [not in lnk file] Used so code knows the non-decoded path
	} itemIdList;
	struct FileLocationTag
	{
		unsigned int length;			// to end of whole tag
		unsigned int firstOffset;		// to end of this tag header
		union Flags
		{
			struct
			{
				unsigned int onLocalVolume : 1;
				unsigned int onNetworkShare : 1;
				unsigned int unused : 30;
			};
			unsigned int raw;
		} flags;
		struct LocalVolume
		{
			unsigned int length;
			enum Type
			{
				Unknown=0,
				NoRoot,
				Removable,	// ie; floppy, usb drive etc.
				Fixed,		// ie; hard disc
				Remote,		// ie; network share
				CDROM,
				RamDrive
			};
			Type type;
			unsigned int serialNo;
			char volumeLabel[64];
			LocalVolume();
		} localVolume;
		char basePath[260];
		struct NetworkVolume
		{
			unsigned int length;
			unsigned int type;
			char shareName[260];
			NetworkVolume();
		} networkVolume;
		char remainingPath[64];

		FileLocationTag();
	} fileLocation;
	struct StringTag
	{
		unsigned short length;			// in characters
		char string[260];		// Unicode string
		StringTag();
	};
	StringTag description;
	StringTag relativePath;
	StringTag workingDir;
	StringTag cmdLineArgs;
	StringTag customIcon;

};

} // namespace


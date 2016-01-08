//@ Abhishek Dey
//@ 1/6/2007
//@ Sept 1 2007
//@ Sept 22 2007

#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include <ifilesystem.h>
#include <ifilereader.h>
#include <ifilewriter.h>
#include <vector>
// 3rd party library zziplib
#include <zzip/zzip.h>

namespace nextar
{

class cFileSystem : public iFileSystem
{
private:
	
	struct File
	{
		cStr		mkFullName;
		cStr		mkName;
		tchar		msExt[8]; //@ max 8 
		u32			muFlags;
		u32			muSize;
	};

	tchar							msWorkingDir[_MAX_PATH];
	bool							mbDirectoryValid;
	bool							mbListOk;
	ZZIP_DIR*						mpkArchive;

	std::vector<File>				mkFiles;

public:

	cFileSystem( const tchar* wd, bool bArchive = false );
	virtual ~cFileSystem();

	/* getDirectory	*/
	virtual const tchar* getDirectory();
	/* isWorkingDirArchive */
	virtual bool isWorkingDirArchive();
	/* setDirectory */
	virtual bool setDirectory(const tchar*,bool bArchive);
	/* openFile */
	virtual iFileReader* openFile( const tchar* );
	/* createFile */
	virtual iFileWriter* createFile( const tchar*, bool append = false );
	/* returns the total number of files in the current directory */
	virtual const s32	 getFileCount();
	/* returns flags that specify file type. Test with the flags given above */
	/* The only thing is that if the file is an archive 
	/* unlike iFileSystem, it respects it as a file */
	virtual u32 getFileFlags(s32 idx);
	/* returns the file extension */
	virtual const tchar* getFileExt(s32 idx);
	/* returns the file name */
	virtual const tchar* getFileName(s32 idx);
	/* returns the full name of the file. i.e. along with path */
	virtual const tchar* getFileFullName(s32 idx);
	/* returns the file size */
	virtual const u32	 getFileSize(s32);
	/* construct list_t with filter */
	virtual void		 buildFileList( const tchar* sFilter );

private:

	void _buildList(const tchar* sFilter);
	void _getFileExtension(tchar* store, const cStr &rkName);
};

}


#endif //__FILESYSTEM_H__
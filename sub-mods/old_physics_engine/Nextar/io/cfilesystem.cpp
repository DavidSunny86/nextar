//@ Abhishek Dey
//@ 1/6/2007
//@ Sept 1 2007
//@ Sept 22 2007

#include <cfilesystem.h>
#include <cfilewriter.h>
#include <cfilereader.h>
#include <czipfilereader.h>
#include <isystem.h>

#ifdef X_WIN32
#	include <direct.h>
#	include <io.h>
#elif (defined(LINUX) || defined(MACOSX))
#	include <unistd.h>
#	include <dirent.h>
#	include <sys/types.h> 
#	include <sys/stat.h> 
#endif

namespace nextar
{

cFileSystem::cFileSystem( const tchar* wd, bool arch ) : mpkArchive(0), mbDirectoryValid(false), mbListOk(false)
{
	RefObjectInit(cFileSystem);
	setDirectory(wd,arch);
}

cFileSystem::~cFileSystem()
{
}

const tchar* cFileSystem::getDirectory() 
{
#ifdef X_WIN32
	_tgetcwd( msWorkingDir, _MAX_PATH);
	X_SharedPtr(iSystem)->getNextarFilePath(msWorkingDir);
	return msWorkingDir;
#elif (defined(LINUX) || defined(MACOSX))
#	if !defined(UNICODE)
	getcwd(msWorkingDir, (size_t)_MAX_PATH);
	X_SharedPtr(iSystem)->getNextarFilePath(msWorkingDir);
	return msWorkingDir;
#	else
		/* TODO: wide char version */
	return 0;
#	endif
#else
	return 0;
#endif
}

bool cFileSystem::isWorkingDirArchive()
{
	return mpkArchive!=NULL;
}

bool cFileSystem::setDirectory( const tchar* sNewDirX, bool bArchive )
{
	ZZIP_DIR* bkupArchive = mpkArchive;
	mpkArchive = 0;

	mbDirectoryValid = false;
	mbListOk = false;

	tchar sNewDir[_MAX_PATH];
	_tcscpy_s(sNewDir,_MAX_PATH,sNewDirX);
	X_SharedPtr(iSystem)->getRealFilePath(sNewDir);

	if( !bArchive )
	{
#ifdef X_WIN32
		
		mbDirectoryValid = (_tchdir(sNewDir) == 0);
#elif (defined(LINUX) || defined(MACOSX))
#	if !defined(UNICODE)
		mbDirectoryValid = !(chdir(sNewDir) == 0);
#	else
	/* TODO: wide char version */
		return 0;
#	endif
#endif
		/* create the list_t of files */
		getDirectory();
	}
	else
	{
		// we use the zzlib here
		mpkArchive = zzip_opendir(sNewDir);
		if(mpkArchive) mbDirectoryValid = true;
		else { mbDirectoryValid = false; mpkArchive = bkupArchive; }
	}

	if(mbDirectoryValid && bkupArchive)
		zzip_closedir(bkupArchive);

	_buildList(0);
	return mbDirectoryValid;
}

/* openFile */
iFileReader* cFileSystem::openFile( const tchar* sName )
{
	if(mpkArchive)
	{
		cZipFileReader* pkRead = new cZipFileReader(sName,mpkArchive);
		if( pkRead->isValid() )
			return pkRead;
		pkRead->release();
	}
	
	cStr kName = sName;
	X_SharedPtr(iSystem)->getRealFilePath(kName.str());
	cFileReader* pkRead = new cFileReader( kName );

	if( pkRead->isValid() )
		return pkRead;

	pkRead->release();
	return 0;
}

/* createFile */
iFileWriter* cFileSystem::createFile( const tchar* sName, bool bApp )
{
	cStr kName = sName;
	X_SharedPtr(iSystem)->getRealFilePath(kName.str());
	cFileWriter* pkWrite = new cFileWriter( kName, bApp );
	if( pkWrite->isValid() )
		return pkWrite;
	pkWrite->release();
	return 0;
}

/* build file list_t */
void cFileSystem::buildFileList( const tchar* sFilter )
{
	_buildList(sFilter);
}

const s32 cFileSystem::getFileCount() 
{
	if( mbListOk )
		return (s32)mkFiles.size();
	return -1;
}

const tchar* cFileSystem::getFileExt( s32 idx )
{
	X_BreakOutIf(idx < 0);
	if( mbListOk && (s32)mkFiles.size() > idx )
		return mkFiles[idx].msExt;
	return 0;
}

const tchar* cFileSystem::getFileName( s32 idx )
{
	X_BreakOutIf(idx < 0);
	if( mbListOk && (s32)mkFiles.size() > idx )
		return mkFiles[idx].mkName;
	return 0;
}

u32 cFileSystem::getFileFlags( s32 idx )
{
	X_BreakOutIf(idx < 0);
	if( mbListOk && (s32)mkFiles.size() > idx )
		return mkFiles[idx].muFlags;
	return -1;
}

const tchar* cFileSystem::getFileFullName( s32 idx )
{
	X_BreakOutIf(idx < 0);
	if( mbListOk && (s32)mkFiles.size() > idx )
	{
		if( mkFiles[idx].mkFullName.strlen() < mkFiles[idx].mkName.strlen() )
		{
			mkFiles[idx].mkFullName = msWorkingDir;
			mkFiles[idx].mkFullName.append(N3DFileSeparator);
			mkFiles[idx].mkFullName.append(mkFiles[idx].mkName);
		}

		return mkFiles[idx].mkFullName;
	}
	return 0;
}

const u32 cFileSystem::getFileSize( s32 idx )
{
	X_BreakOutIf(idx < 0);
	if( mbListOk && (s32)mkFiles.size() > idx )
		return mkFiles[idx].muSize;
	return 0;
}

void cFileSystem::_getFileExtension( tchar* st, const cStr &rkName )
{	
	tchar store[8];
	s32 k = 0;
	for( s32 i = rkName.strlen()-1; i >= 0 && rkName[i] != '.' && k < 7; i--, k++ )
		store[k] = rkName[i];
	if(k>=7) k = 0;
	store[k] = 0;
	for( s32 i = 0; i < k; i++ )
		st[i] = store[(k-1)-i];
	st[k] = 0;
}

void cFileSystem::_buildList( const tchar* sFilter )
{	
	mkFiles.clear();
	if( !mpkArchive )
	{
		mbListOk = false;
		
#ifdef X_WIN32

		_tfinddata_t	kFileData;
		intptr_t		iFile;
		File			kFileEntry;

		cStr kFilter = sFilter == NULL ? X_Text("*") : sFilter;

		if( (iFile = _tfindfirst( kFilter.c_str(), &kFileData )) != -1L )
		{
			do
			{
				kFileEntry.msExt[0] = 0;
				kFileEntry.mkName = kFileData.name;
				kFileEntry.muSize =	kFileData.size;
				kFileEntry.muFlags = 0;

				if( (_A_NORMAL & kFileData.attrib) )
					kFileEntry.muFlags =  0;

				if( (_A_ARCH & kFileData.attrib) )
					kFileEntry.muFlags |= iFileSystem::ARCHIEVE;

				if( (_A_RDONLY & kFileData.attrib) )
					kFileEntry.muFlags |= iFileSystem::READ_ONLY;

				if( (_A_SUBDIR & kFileData.attrib) )
					kFileEntry.muFlags |= iFileSystem::DIRECTORY;

				if( !(kFileEntry.muFlags & iFileSystem::DIRECTORY) )
					_getFileExtension(kFileEntry.msExt,kFileEntry.mkName);

				mkFiles.push_back(kFileEntry);
			}
			while( _tfindnext( iFile, &kFileData ) == 0 );

			_findclose( iFile );
		}

		mbListOk = true;

#elif (defined(LINUX) || defined(MACOSX))

		mbListOk = false;
		/* TODO: Implement this later */

#endif

	}
	else
	{
		zzip_rewinddir(mpkArchive);
		File	kFileEntry;
		kFileEntry.muFlags = 0;
		ZZIP_DIRENT* kD = 0;
		// use zzip to load up the file nfos
		while ((kD = zzip_readdir (mpkArchive)))
		{
			kFileEntry.msExt[0] = 0;
			kFileEntry.muSize = kD->st_size;
			kFileEntry.mkName = kD->d_name;
			_getFileExtension(kFileEntry.msExt,kFileEntry.mkName);
		}
	}
}


}
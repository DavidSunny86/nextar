#include <BaseHeaders.h>
#include <DataStream.h>
#define _NexZipApi

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <ZipArchive.h>
#include <OsDefs.h>
#include <Archive.h>

#if defined(NEX_WINDOWS)
#elif defined(NEX_LINUX)
#include <stdlib.h>
#else
#endif

#ifndef DEFAULT_COMPRESSION_METHOD
#define DEFAULT_COMPRESSION_METHOD ZIP_DEFLATE
#endif

#ifndef DEFAULT_COMPRESSION_LEVEL
#define DEFAULT_COMPRESSION_LEVEL Z_DEFAULT_COMPRESSION
#endif

// ----------------- hdrs

// ----------------- endianness
#if NEX_ENDIANNESS == NEX_ENDIAN_BIG

#define BUFF_GET_(ofs, Type)	Endian::Swap##Type(*(uint##Type*)&buff[ofs])
#define BUFF_GET_SHORT(ofs)     BUFF_GET_(ofs, 16)
#define BUFF_GET_LONG(ofs)      BUFF_GET_(ofs, 32)

#define BUFF_SET_(ofs, val, Type) (*(uint##Type*)&buff[ofs]) = Endian::Swap##Type((uint##Type)(val))
#define BUFF_SET_SHORT(ofs,val) BUFF_SET_(ofs, val, 16)
#define BUFF_SET_LONG(ofs,val)  BUFF_SET_(ofs, val, 32)

#else

#define BUFF_GET_(ofs, Type)	(*(uint##Type*)&buff[ofs])
#define BUFF_GET_SHORT(ofs)     BUFF_GET_(ofs, 16)
#define BUFF_GET_LONG(ofs)      BUFF_GET_(ofs, 32)

#define BUFF_SET_(ofs, val, Type) (*(uint##Type*)&buff[ofs]) = ((uint##Type)(val))
#define BUFF_SET_SHORT(ofs,val) BUFF_SET_(ofs, val, 16)
#define BUFF_SET_LONG(ofs,val)  BUFF_SET_(ofs, val, 32)

#endif

// ----------------- endianness

namespace nextar {

	static void GetTempFilename(String& name) {
		static uint32 callCnt = 1;
#if defined(NEX_WINDOWS)
		char filename[MAX_PATH];
		if (GetTempFileName(".", "nex", 0, filename)) {
			name = filename;
		} else
#elif defined(NEX_LINUX)

		char templ[16] = "temp.XXXXXX";
		if (mkdtemp(templ))
			name = templ;
		else
#else
#error todo
#endif
		{
			name = "nexx.";
			name += Convert::ToString(callCnt++);
			name += ".temp";
		}
	}

	static void SafeDeleteFile(const String& name) {
#if defined(NEX_WINDOWS)
		_unlink(name.c_str());
#elif defined(NEX_LINUX)
		remove(name.c_str());
#else
#error todo
#endif

	}

	namespace zip {
// ----------------- hdrs
		static const char hdrCentral[4] = { 'P', 'K', CENTRAL_HDR_SIG };
		static const char hdrLocal[4] = { 'P', 'K', LOCAL_HDR_SIG };
		static const char hdrEndcentral[4] = { 'P', 'K', END_CENTRAL_SIG };
//static const char hdrExtlocal[4] = { 'P', 'K', EXTD_LOCAL_SIG };

		static bool ReadExtraField(ZipFile* zipf, InputStreamPtr& infile,
				size_t extraFieldLength) {
			if (zipf->extrafield
					&& (zipf->info.extraFieldLength != extraFieldLength)) {
				NEX_FREE(zipf->extrafield, MEMCAT_GENERAL);
				zipf->extrafield = 0;
			}

			zipf->info.extraFieldLength = (ush) extraFieldLength;
			if (extraFieldLength) {
				if (!zipf->extrafield)
					zipf->extrafield =
							(char*) NEX_ALLOC(extraFieldLength, MEMCAT_GENERAL);
							return (infile->Read(zipf->extrafield, extraFieldLength)
									== extraFieldLength) != 0;
						} else
						return true;
					}

		static bool ReadFileComment(ZipFile* zipf, InputStreamPtr& infile,
				size_t fileCommentLength) {
			if (zipf->comment
					&& (zipf->info.fileCommentLength != fileCommentLength)) {
				NEX_FREE(zipf->comment, MEMCAT_GENERAL);
				zipf->comment = 0;
			}

			zipf->info.fileCommentLength = (ush) fileCommentLength;
			if (fileCommentLength) {
				if (!zipf->comment)
					zipf->comment =
							(char*) NEX_ALLOC(fileCommentLength, MEMCAT_GENERAL);
							return (infile->Read(zipf->comment, fileCommentLength)
									== fileCommentLength) != 0;
						} else
						return true;
					}

		static ZipFile* InsertEntry(ZipArchive* arch, const char *name,
				zCentralDirFileHdr *cdfh) {
			ZipFile *e = (ZipFile*) NEX_ALLOC(sizeof (ZipFile), MEMCAT_GENERAL);
			std::memset(e, 0, sizeof(ZipFile));
			e->filename = StringUtils::NewStr(name);
			memcpy(&e->info, cdfh, sizeof(zCentralDirFileHdr));

			e->next = 0;
			e->prev = arch->entriesTail;
			if (arch->entriesTail)
			arch->entriesTail->next = e;
			else
			arch->entriesHead = e;
			arch->entriesTail = e;
			arch->numFiles++;
			return e;
		}

		static bool ReadLFH(zLocalFileHdr *lfh, InputStreamPtr& infile) {
			char buff[ZIP_LOCAL_FILE_HEADER_SIZE];

			if (infile->Read(buff,
					ZIP_LOCAL_FILE_HEADER_SIZE) < ZIP_LOCAL_FILE_HEADER_SIZE)
				return false;

			lfh->versionNeededToExtract[0] =
					buff[L_VERSION_NEEDED_TO_EXTRACT_0];
			lfh->versionNeededToExtract[1] =
					buff[L_VERSION_NEEDED_TO_EXTRACT_1];
			lfh->generalPurposeBitFlag =
					BUFF_GET_SHORT(L_GENERAL_PURPOSE_BIT_FLAG);
			lfh->compressionMethod = BUFF_GET_SHORT(L_COMPRESSION_METHOD);
			lfh->lastModFileTime = BUFF_GET_SHORT(L_LAST_MOD_FILE_TIME);
			lfh->lastModFileDate = BUFF_GET_SHORT(L_LAST_MOD_FILE_DATE);
			lfh->crc32 = BUFF_GET_LONG(L_CRC32);
			lfh->csize = BUFF_GET_LONG(L_COMPRESSED_SIZE);
			lfh->ucsize = BUFF_GET_LONG(L_UNCOMPRESSED_SIZE);
			lfh->filenameLength = BUFF_GET_SHORT(L_FILENAME_LENGTH);
			lfh->extraFieldLength = BUFF_GET_SHORT(L_EXTRA_FIELD_LENGTH);

			return true;
		}

		static bool ReadCDFH(zCentralDirFileHdr* cdfh, InputStreamPtr& infile) {
			char buff[ZIP_CENTRAL_DIRECTORY_FILE_HEADER_SIZE];

			if (infile->Read(buff,
					ZIP_CENTRAL_DIRECTORY_FILE_HEADER_SIZE) < ZIP_CENTRAL_DIRECTORY_FILE_HEADER_SIZE)
				return false;

			cdfh->versionMadeBy[0] = buff[C_VERSION_MADE_BY_0];
			cdfh->versionMadeBy[1] = buff[C_VERSION_MADE_BY_1];
			cdfh->versionNeededToExtract[0] =
					buff[C_VERSION_NEEDED_TO_EXTRACT_0];
			cdfh->versionNeededToExtract[1] =
					buff[C_VERSION_NEEDED_TO_EXTRACT_1];
			cdfh->generalPurposeBitFlag =
					BUFF_GET_SHORT(C_GENERAL_PURPOSE_BIT_FLAG);
			cdfh->compressionMethod = BUFF_GET_SHORT(C_COMPRESSION_METHOD);
			cdfh->lastModFileTime = BUFF_GET_SHORT(C_LAST_MOD_FILE_TIME);
			cdfh->lastModFileDate = BUFF_GET_SHORT(C_LAST_MOD_FILE_DATE);
			cdfh->crc32 = BUFF_GET_LONG(C_CRC32);
			cdfh->csize = BUFF_GET_LONG(C_COMPRESSED_SIZE);
			cdfh->ucsize = BUFF_GET_LONG(C_UNCOMPRESSED_SIZE);
			cdfh->filenameLength = BUFF_GET_SHORT(C_FILENAME_LENGTH);
			cdfh->extraFieldLength = BUFF_GET_SHORT(C_EXTRA_FIELD_LENGTH);
			cdfh->fileCommentLength = BUFF_GET_SHORT(C_FILE_COMMENT_LENGTH);
			cdfh->diskNumberStart = BUFF_GET_SHORT(C_DISK_NUMBER_START);
			cdfh->internalFileAttributes =
					BUFF_GET_SHORT(C_INTERNAL_FILE_ATTRIBUTES);
			cdfh->externalFileAttributes =
					BUFF_GET_LONG(C_EXTERNAL_FILE_ATTRIBUTES);
			cdfh->relativeOffsetLocalHeader =
					BUFF_GET_LONG(C_RELATIVE_OFFSET_LOCAL_HEADER);
			return true;
		}

		static void LoadECDR(zEndCentralDirRecord* ecdr, char *buff) {
			ecdr->numberThisDisk = BUFF_GET_SHORT(E_NUMBER_THIS_DISK);
			ecdr->numDiskStartCdir =
					BUFF_GET_SHORT(E_NUM_DISK_WITH_START_CENTRAL_DIR);
			ecdr->numEntriesCentrlDirThsDisk =
					BUFF_GET_SHORT(E_NUM_ENTRIES_CENTRL_DIR_THS_DISK);
			ecdr->totalEntriesCentralDir =
					BUFF_GET_SHORT(E_TOTAL_ENTRIES_CENTRAL_DIR);
			ecdr->sizeCentralDirectory =
					BUFF_GET_LONG(E_SIZE_CENTRAL_DIRECTORY);
			ecdr->offsetStartCentralDirectory =
					BUFF_GET_LONG(E_OFFSET_START_CENTRAL_DIRECTORY);
			ecdr->zipfileCommentLength =
					BUFF_GET_SHORT(E_ZIPFILE_COMMENT_LENGTH);
		}

		static bool ReadArchiveComment(ZipArchive* ar, InputStreamPtr& infile,
				size_t commentLength) {
			if (ar->comment && ar->commentLength != commentLength) {
				NEX_FREE(ar->comment, MEMCAT_GENERAL);
				ar->comment = 0;
			}

			ar->commentLength = commentLength;
			if (!ar->commentLength)
				return true;

			if (!ar->comment)
				ar->comment =
						(char*) (char*) NEX_ALLOC(commentLength, MEMCAT_GENERAL);
			return (infile->Read(ar->comment, commentLength) == commentLength)
					!= 0;
		}

		static void ReadZipEntries(ZipArchive* arch, InputStreamPtr& infile) {
			size_t curOffs, newOffs;
			char buff[1024];
			zCentralDirFileHdr cdfh;
			zLocalFileHdr lfh;

			curOffs = 0;
			while ((infile->Read(buff, sizeof(hdrLocal)) >= sizeof(hdrLocal))
					&& (memcmp(buff, hdrLocal, sizeof(hdrLocal)) == 0)
					&& (ReadLFH(&lfh, infile))) {
				newOffs = curOffs + sizeof(hdrLocal)
						+ ZIP_LOCAL_FILE_HEADER_SIZE + lfh.filenameLength
						+ lfh.extraFieldLength + lfh.csize;
				if ((lfh.filenameLength >= sizeof(buff))
						|| (infile->Read(buff, lfh.filenameLength)
								< lfh.filenameLength))
					return; /* Broken zipfile? */
				buff[lfh.filenameLength] = 0;

				if ((buff[lfh.filenameLength - 1] != '/')) {
					ZipFile *curentry;
					/* Partialy convert lfh to cdfh */
					memset(&cdfh, 0, sizeof(cdfh));
					cdfh.versionNeededToExtract[0] =
							lfh.versionNeededToExtract[0];
					cdfh.versionNeededToExtract[1] =
							lfh.versionNeededToExtract[1];
					cdfh.generalPurposeBitFlag = lfh.generalPurposeBitFlag;
					cdfh.compressionMethod = lfh.compressionMethod;
					cdfh.lastModFileTime = lfh.lastModFileTime;
					cdfh.lastModFileDate = lfh.lastModFileDate;
					cdfh.crc32 = lfh.crc32;
					cdfh.csize = lfh.csize;
					cdfh.ucsize = lfh.ucsize;
					cdfh.relativeOffsetLocalHeader = (uint32) curOffs;

					curentry = InsertEntry(arch, buff, &cdfh);

					if (!ReadExtraField(curentry, infile, lfh.extraFieldLength))
						return; /* Broken zipfile */
				} /* endif */
				curOffs = newOffs;
				if (!infile->Seek((long) newOffs, std::ios_base::beg))
					return; /* Broken zipfile */
			} /* endwhile */
		}

		static ZipArchive* ReadZipDir(InputStreamPtr& infile) {
			zEndCentralDirRecord ecdr;
			zCentralDirFileHdr cdfh;
			char buff[1024]; /* Read ZIPfile from end in 1K chunks */
			size_t curOffs, minOffs, centralDirectoryOffset;
			ZipArchive* arch = 0;
			size_t step = ZIP_END_CENTRAL_DIR_RECORD_SIZE
					+ sizeof(hdrEndcentral);

			if (!infile)
				return 0; /* File not open */
			infile->Seek(0, std::ios_base::end);
			if ((long) (curOffs = (size_t) infile->Tell()) == -1)
				return 0; /* Can't get file position */

			arch = NEX_NEW(ZipArchive());
			//std::memset(arch, 0, sizeof(ZipArchive));

			if (curOffs
					>= (65535 + ZIP_END_CENTRAL_DIR_RECORD_SIZE
							+ sizeof(hdrEndcentral)))
				minOffs = curOffs
						- (65535 + ZIP_END_CENTRAL_DIR_RECORD_SIZE
								+ sizeof(hdrEndcentral));
			else
				minOffs = 0;

			/* Try to Find ZIPfile central directory structure */
			/* For this we have to search from end of file the signature "PK" */
			/* after which follows a two-byte END_CENTRAL_SIG */
			while (curOffs > minOffs) {
				size_t searchPos;

				if (curOffs >= sizeof(buff) - step)
					curOffs -= sizeof(buff) - step;
				else
					curOffs = 0;

				infile->Seek((long) curOffs, std::ios_base::beg);
				searchPos = infile->Read(buff, sizeof(buff));
				if (searchPos >= step) {
					register char *searchPtr;

					for (searchPtr = &buff[searchPos - step]; searchPtr > buff;
							searchPtr--)
						if ((*searchPtr == 'P')
								&& (memcmp(searchPtr, hdrEndcentral,
										sizeof(hdrEndcentral)) == 0)) {
							/* Central directory structure found */
							centralDirectoryOffset = curOffs
									+ (size_t) (searchPtr - buff);
							LoadECDR(&ecdr, &searchPtr[sizeof(hdrEndcentral)]);
							if (!infile->Seek(
									(long) (centralDirectoryOffset
											+ sizeof(hdrEndcentral)
											+ ZIP_END_CENTRAL_DIR_RECORD_SIZE),
									std::ios_base::beg)
									|| !ReadArchiveComment(arch, infile,
											ecdr.zipfileCommentLength)
									|| !infile->Seek(
											ecdr.offsetStartCentralDirectory,
											std::ios_base::beg))
								goto rebuildCdr;
							/* Broken central directory */

							/* Now read central directory structure */
							for (;;) {
								ZipFile *curentry;
								if ((infile->Read(buff, sizeof(hdrCentral))
										< sizeof(hdrCentral))
										|| (memcmp(buff, hdrCentral,
												sizeof(hdrCentral)) != 0)) {
									if (arch->numFiles)
										return arch; /* Finished reading central directory */
									else
										goto rebuildCdr;
									/* Broken central directory */
								}
								if ((!ReadCDFH(&cdfh, infile))
										|| (cdfh.filenameLength > sizeof(buff))
										|| (infile->Read(buff,
												cdfh.filenameLength)
												< cdfh.filenameLength))
									return arch; /* Broken zipfile? */
								buff[cdfh.filenameLength] = 0;

								curentry = InsertEntry(arch, buff, &cdfh);
								if (!ReadExtraField(curentry, infile,
										cdfh.extraFieldLength)
										|| !ReadFileComment(curentry, infile,
												cdfh.fileCommentLength))
									return arch; /* Broken zipfile? */
							} /* endfor */
						} /* endif */
				} /* endif */
			} /* endwhile */

			rebuildCdr:
			/* If we are here, we did not succeeded to read central directory */
			/* If so, we have to rebuild it by reading each ZIPfile member separately */
			if (!infile->Seek(0, std::ios_base::beg))
				return arch;
			ReadZipEntries(arch, infile);
			return arch;
		}

		static ZipArchive* ReadDir(InputStreamPtr& file) {
			ZipArchive* ziparch;
			if (!(ziparch = ReadZipDir(file)))
				return 0;
			ziparch->infile = file;
			ziparch->numRefs = 1;
			return ziparch;
		}

		_NexZipApi ZipArchive* OpenArchive(const String& fullname) {
			URL fileURL = fullname;
			FileSystem& fileSys =
				FileSystem::Instance();
			InputStreamPtr file = fileSys.OpenRead(fileURL);
			if (!file)
				return 0;
			ZipArchive* za = ReadDir(file);
			za->fileName = fileURL;
			return za;
		}

		_NexZipApi ZipArchive* CreateArchive(const String& fullname) {
			URL fileURL = fullname;
			FileSystem& fileSys =
					FileSystem::Instance();
			OutputStreamPtr file = fileSys.OpenWrite(fileURL);
			if (!file)
				return 0;
			ZipArchive* z = NEX_NEW(ZipArchive());
			z->flags |= (NEX_ZIP_DIRTY);
			z->outfile = file;
			z->numRefs = 1;
			z->fileName = fileURL;
			return z;
		}

		static void DeleteEntry(ZipFile* f) {
			// simply delete this entry from
			// exitance.
			if (f->comment)
				NEX_FREE(f->comment, MEMCAT_GENERAL);
			if (f->extrafield)
				NEX_FREE(f->extrafield, MEMCAT_GENERAL);
			if (f->filename)
				NEX_FREE(f->filename, MEMCAT_GENERAL);
			NEX_FREE(f, MEMCAT_GENERAL);
		}

		_NexZipApi void CloseArchive(ZipArchive* arch) {
			if (arch) {
				if (!(--arch->numRefs)) {

					ZipFile* f;
					FlushArchive(arch);

					arch->infile.Clear();
					arch->outfile.Clear();

					if (arch->comment)
						NEX_FREE(arch->comment, MEMCAT_GENERAL);

					f = arch->entriesHead;
					while (f) {
						ZipFile* n = f->next;
						DeleteEntry(f);
						f = n;
					}
				}
			}
		}

		static bool IsDeleted(ZipArchive* arch, const char* filename) {
			StrNode* z = arch->delHead;
			while (z) {
				if (!StringUtils::NoCaseCompare(z->name, filename))
					return true;
				z = z->next;
			}
			return false;
		}

		static bool WriteLFH(ZipFile* thisFile, OutputStreamPtr& outfile) {
			char buff[ZIP_LOCAL_FILE_HEADER_SIZE];
			zCentralDirFileHdr* info = &thisFile->info;
			size_t lfhpos = (size_t) outfile->Tell();

			buff[L_VERSION_NEEDED_TO_EXTRACT_0] =
					info->versionNeededToExtract[0];
			buff[L_VERSION_NEEDED_TO_EXTRACT_1] =
					info->versionNeededToExtract[1];
			BUFF_SET_SHORT(L_GENERAL_PURPOSE_BIT_FLAG,
					info->generalPurposeBitFlag);
			BUFF_SET_SHORT(L_COMPRESSION_METHOD, info->compressionMethod);
			BUFF_SET_SHORT(L_LAST_MOD_FILE_TIME, info->lastModFileTime);
			BUFF_SET_SHORT(L_LAST_MOD_FILE_DATE, info->lastModFileDate);
			BUFF_SET_LONG(L_CRC32, info->crc32);
			BUFF_SET_LONG(L_COMPRESSED_SIZE, info->csize);
			BUFF_SET_LONG(L_UNCOMPRESSED_SIZE, info->ucsize);
			info->filenameLength = (ush) StringUtils::Length(
					thisFile->filename);
			BUFF_SET_SHORT(L_FILENAME_LENGTH, info->filenameLength);
			info->extraFieldLength =
					thisFile->extrafield ? info->extraFieldLength : 0;
			BUFF_SET_SHORT(L_EXTRA_FIELD_LENGTH, info->extraFieldLength);

			outfile->Write(hdrLocal, sizeof(hdrLocal));
			outfile->Write(buff, ZIP_LOCAL_FILE_HEADER_SIZE);
			outfile->Write(thisFile->filename, info->filenameLength);
			outfile->Write(thisFile->extrafield, info->extraFieldLength);

			info->relativeOffsetLocalHeader = (uint32) lfhpos;
			return true;
		}

		static bool WriteCDFH(ZipFile* file, OutputStreamPtr& outfile) {
			char buff[ZIP_CENTRAL_DIRECTORY_FILE_HEADER_SIZE];
			zCentralDirFileHdr* info = &file->info;

			/* Convert endianess if needed */
			buff[C_VERSION_MADE_BY_0] = info->versionMadeBy[0];
			buff[C_VERSION_MADE_BY_1] = info->versionMadeBy[1];
			buff[C_VERSION_NEEDED_TO_EXTRACT_0] =
					info->versionNeededToExtract[0];
			buff[C_VERSION_NEEDED_TO_EXTRACT_1] =
					info->versionNeededToExtract[1];

			BUFF_SET_SHORT(C_GENERAL_PURPOSE_BIT_FLAG,
					info->generalPurposeBitFlag);
			BUFF_SET_SHORT(C_COMPRESSION_METHOD, info->compressionMethod);
			BUFF_SET_SHORT(C_LAST_MOD_FILE_TIME, info->lastModFileTime);
			BUFF_SET_SHORT(C_LAST_MOD_FILE_DATE, info->lastModFileDate);
			BUFF_SET_LONG(C_CRC32, info->crc32);
			BUFF_SET_LONG(C_COMPRESSED_SIZE, info->csize);
			BUFF_SET_LONG(C_UNCOMPRESSED_SIZE, info->ucsize);

			info->filenameLength = (ush) StringUtils::Length(file->filename);
			BUFF_SET_SHORT(C_FILENAME_LENGTH, info->filenameLength);
			/* We're ignoring extra field for central directory, although InfoZIP puts
			 * there a field containing EF_TIME - universal timestamp - but for example
			 * DOS pkzip/pkunzip does not put nothing there. */
			BUFF_SET_SHORT(C_EXTRA_FIELD_LENGTH, 0);
			info->fileCommentLength =
					file->comment ? info->fileCommentLength : 0;
			BUFF_SET_SHORT(C_FILE_COMMENT_LENGTH, info->fileCommentLength);
			BUFF_SET_SHORT(C_DISK_NUMBER_START, info->diskNumberStart);
			BUFF_SET_SHORT(C_INTERNAL_FILE_ATTRIBUTES,
					info->internalFileAttributes);
			BUFF_SET_LONG(C_EXTERNAL_FILE_ATTRIBUTES,
					info->externalFileAttributes);
			BUFF_SET_LONG(C_RELATIVE_OFFSET_LOCAL_HEADER,
					info->relativeOffsetLocalHeader);

			outfile->Write(hdrCentral, sizeof(hdrCentral));
			outfile->Write(buff, ZIP_CENTRAL_DIRECTORY_FILE_HEADER_SIZE);
			outfile->Write(file->filename, info->filenameLength);
			outfile->Write(file->comment, info->fileCommentLength);

			return true;
		}

		static bool WriteECDR(ZipArchive* arch, zEndCentralDirRecord* ecdr,
				OutputStreamPtr& outfile) {

			char buff[ZIP_END_CENTRAL_DIR_RECORD_SIZE];

			outfile->Write(hdrEndcentral, sizeof(hdrEndcentral));

			BUFF_SET_SHORT(E_NUMBER_THIS_DISK, ecdr->numberThisDisk);
			BUFF_SET_SHORT(E_NUM_DISK_WITH_START_CENTRAL_DIR,
					ecdr->numDiskStartCdir);
			BUFF_SET_SHORT(E_NUM_ENTRIES_CENTRL_DIR_THS_DISK,
					ecdr->numEntriesCentrlDirThsDisk);
			BUFF_SET_SHORT(E_TOTAL_ENTRIES_CENTRAL_DIR,
					ecdr->totalEntriesCentralDir);
			BUFF_SET_LONG(E_SIZE_CENTRAL_DIRECTORY, ecdr->sizeCentralDirectory);
			BUFF_SET_LONG(E_OFFSET_START_CENTRAL_DIRECTORY,
					ecdr->offsetStartCentralDirectory);
			BUFF_SET_SHORT(E_ZIPFILE_COMMENT_LENGTH,
					ecdr->zipfileCommentLength);

			outfile->Write(buff, ZIP_END_CENTRAL_DIR_RECORD_SIZE);
			outfile->Write(arch->comment, arch->commentLength);

			return true;

		}

		static bool WriteFile(ZipFile* file, OutputStreamPtr& outfile) {
			size_t lfhoffs;
			bool finished = false;

			lfhoffs = (size_t) outfile->Tell();

			file->info.crc32 = crc32(CRCVAL_INITIAL, (Bytef*) file->buffer,
					(uInt) file->bufferPos);

			while (!finished) {
				if (!outfile->Seek(
						(long) (lfhoffs + sizeof(hdrLocal)
								+ ZIP_LOCAL_FILE_HEADER_SIZE
								+ StringUtils::Length(file->filename)
								+ (file->extrafield ?
										file->info.extraFieldLength : 0)),
						std::ios_base::beg))
					return false;

				switch (file->info.compressionMethod) {
				case ZIP_STORE: {
					outfile->Write(file->buffer, file->bufferPos);
					file->info.csize = file->info.ucsize =
							(uint32) file->bufferPos;
					finished = true;
					break;
				}
				case ZIP_DEFLATE: {
					z_stream zs;
					char *buff = (char*) NEX_ALLOC(16384, MEMCAT_GENERAL);
					int32 buffofs = 2; /* Skip inflated data header */

					zs.zalloc = (alloc_func) 0;
					zs.zfree = (free_func) 0;
					zs.next_in = (Bytef*) file->buffer;
					zs.avail_in = (uInt) file->bufferPos;
					if (deflateInit(&zs, DEFAULT_COMPRESSION_LEVEL) != Z_OK) {
						NEX_FREE(buff, MEMCAT_GENERAL);
						return false;
					}
					file->info.csize = 0;
					file->info.ucsize = (uint32) file->bufferPos;

					while (1) {
						int32 rc;
						size_t size;

						zs.next_out = (Bytef *) buff;
						zs.avail_out = sizeof(buff);

						rc = deflate(&zs, Z_FINISH); /* Do actual compression */
						size = sizeof(buff) - zs.avail_out - buffofs;

						file->info.csize += (uint32) size;

						try {
							outfile->Write(&buff[buffofs], size);
						} catch (Exception& e) {
							deflateEnd(&zs);
							NEX_FREE(buff, MEMCAT_GENERAL);
							throw e;
						}
						if (rc == Z_STREAM_END)
							break; /* finished */
						buffofs = 0;
					} /* endwhile */
					deflateEnd(&zs);
					if (file->info.csize < file->info.ucsize)
						finished = true;
					else
						file->info.compressionMethod = ZIP_STORE;
					NEX_FREE(buff, MEMCAT_GENERAL);
					break;
				}
				default:
					return false;
				} /* endswitch */
			} /* endwhile */

			outfile->Seek((long) lfhoffs, std::ios_base::beg);
			if (!WriteLFH(file, outfile))
				return false;
			outfile->Seek(file->info.csize, std::ios_base::beg);
			return true;
		}

		static bool WriteCentralDirectory(ZipArchive* arch,
				OutputStreamPtr& temp) {
			size_t count = 0;
			ZipFile* f;
			size_t cdroffs = (size_t) temp->Tell();
			zEndCentralDirRecord ecdr;

			f = arch->entriesHead;

			while (f) {

				// Don't write deleted or faked entries
				if (IsDeleted(arch, f->filename))
					continue;
				if (!WriteCDFH(f, temp))
					return false;
				count++;
				f = f->next;
			} /* endwhile */

			f = arch->entriesAdded;
			while (f) {
				if (!WriteCDFH(f, temp))
					return false;
				f = f->next;
				count++;
			} /* endwhile */

			/* Write end-of-central-directory record */

			memset(&ecdr, 0, sizeof(ecdr));
			ecdr.numEntriesCentrlDirThsDisk = (ush) count;
			ecdr.totalEntriesCentralDir = (ush) count;
			ecdr.sizeCentralDirectory = (uint32) ((size_t) temp->Tell() - cdroffs);
			ecdr.offsetStartCentralDirectory = (uint32) cdroffs;
			ecdr.zipfileCommentLength = (ush) arch->commentLength;
			if (!WriteECDR(arch, &ecdr, temp))
				return false;
			return true;
		}

		static void UpdateDirectory(ZipArchive* arch) {
			StrNode* s;
			ZipFile* f;

			if (!(arch->flags & NEX_ZIP_DIRTY))
				return;

			s = arch->delHead;
			f = arch->entriesAdded;
			while (s) {
				StrNode* d;
				ZipFile* f2 = arch->entriesHead;
				while (f2) {
					if (!StringUtils::NoCaseCompare(f2->filename, s->name)) {
						ZipFile* fnx;
						// delete f2
						fnx = f2->prev;
						if (fnx)
							fnx->next = f2->next;
						else
							arch->entriesHead = f2->next;

						arch->numFiles--;
						fnx = f2->next;
						if (fnx)
							fnx->prev = f2->prev;
						else
							arch->entriesTail = f2->prev;
						f2 = fnx;
					} else
						f2 = f2->next;
				}
				d = s;
				s = s->next;
				StringUtils::FreeStr(d->name);
				NEX_FREE(d, MEMCAT_GENERAL);
			}
			arch->delHead = 0;
			while (f) {
				ZipFile* f2 = f->next;

				f->prev = arch->entriesTail;
				if (arch->entriesTail)
					arch->entriesTail->next = f;
				else
					arch->entriesHead = f;

				arch->entriesTail = f;
				f->next = 0;
				arch->numFiles++;

				f = f2;
			}
			arch->entriesAdded = 0;
			arch->flags &= ~NEX_ZIP_DIRTY;
		}

		static ZipFile* GetArchiveDirFile(ZipArchive* arch, const char* name) {
			ZipFile* f = arch->entriesHead;
			while (f) {
				if (!StringUtils::NoCaseCompare(f->filename, name))
					return f;
				f = f->next;
			}
			return 0;
		}

		static ZipFile* GetArchiveAddedFile(ZipArchive* arch,
				const char* name) {
			ZipFile* f = arch->entriesAdded;
			while (f) {
				if (!StringUtils::NoCaseCompare(f->filename, name))
					return f;
				f = f->next;
			}
			return 0;
		}

		static bool WriteZipArchive(ZipArchive* arch) {
			String tmpFileName;

			char buff[4 * 1024];
			bool success = false;
			size_t n = 0;
			ZipFile* f;

			if (!(arch->flags & NEX_ZIP_DIRTY))
				return true;
			// Create a temporary file
			GetTempFilename(tmpFileName);
			std::FILE* tempFile = 0;

#if defined(NEX_MSVC)
			tempFile = 0;
			fopen_s(&tempFile, tmpFileName.c_str(), "wb");
#else
			tempFile = fopen(tmpFileName.c_str(), "wb");
#endif
			if (!(tempFile))
				return false;

			OutputStreamPtr temp = Assign(NEX_NEW(FileOutputStream(tempFile)));
			InputStreamPtr& file = arch->infile;
			file->Seek(0, std::ios_base::beg);
			while (file->Read(buff, sizeof(hdrLocal)) == sizeof(hdrLocal)) {
				size_t bytesToCopy, bytesToSkip;
				ZipFile* thisFile = 0;
				if (memcmp(buff, hdrLocal, sizeof(hdrLocal)) == 0) {
					zLocalFileHdr lfh;
					char* thisName;
					if (!ReadLFH(&lfh, file))
						goto tempFailed;
					thisName =
							(char*) NEX_ALLOC(lfh.filenameLength + 1, MEMCAT_GENERAL);
					if (file->Read(thisName, lfh.filenameLength)
							< lfh.filenameLength) {
						NEX_FREE(thisName, MEMCAT_GENERAL);
						goto tempFailed;
					}
					thisName[lfh.filenameLength] = 0;

					if (IsDeleted(arch, thisName)) {
						skipEntry: bytesToSkip = lfh.extraFieldLength
								+ lfh.csize;
						bytesToCopy = 0;
						NEX_FREE(thisName, MEMCAT_GENERAL);
					} else {
						thisFile = GetArchiveDirFile(arch, thisName);

						if (!thisFile)
							/* This means we found a entry in archive which is not
							 * present in our `dir' array: this means either the ZIP
							 * file has changed after we read the ZIP directory,
							 * or this is a `pure directory' entry (which we ignore
							 * during reading). In any case, just copy it unchanged
							 * into the output file.
							 */
							goto skipEntry;

						NEX_FREE(thisName, MEMCAT_GENERAL);
						if (thisFile->info.csize != lfh.csize)
							goto tempFailed;
						/* Broken archive */
						ReadExtraField(thisFile, file, lfh.extraFieldLength);
						bytesToSkip = 0;
						bytesToCopy = lfh.csize;
						if (!WriteLFH(thisFile, temp))
							goto tempFailed;
						/* Write error */
					}
				} else if (memcmp(buff, hdrCentral, sizeof(hdrCentral)) == 0) {
					// central directory header
					zCentralDirFileHdr cdfh;

					if (!ReadCDFH(&cdfh, file))
						goto tempFailed;

					bytesToCopy = 0;
					bytesToSkip = cdfh.filenameLength + cdfh.extraFieldLength
							+ cdfh.fileCommentLength;
				} else if (memcmp(buff, hdrEndcentral, sizeof(hdrEndcentral))
						== 0) {
					// end-of-central-directory header
					zEndCentralDirRecord ecdr;
					char buff[ZIP_END_CENTRAL_DIR_RECORD_SIZE];

					if (file->Read(buff,
							ZIP_END_CENTRAL_DIR_RECORD_SIZE) < ZIP_END_CENTRAL_DIR_RECORD_SIZE)
						goto tempFailed;
					LoadECDR(&ecdr, buff);

					bytesToCopy = 0;
					bytesToSkip = ecdr.zipfileCommentLength;
				} else {
					// Unknown chunk type
					goto tempFailed;
				} /* endif */

				if (bytesToSkip)
					file->Seek((long) bytesToSkip, std::ios_base::cur);
				while (bytesToCopy) {
					size_t size;

					if (bytesToCopy > sizeof(buff))
						size = sizeof(buff);
					else
						size = bytesToCopy;

					if ((file->Read(buff, size) < size))
						goto tempFailed;
					// todo Handle failure while writing
					temp->Write(buff, size);
					bytesToCopy -= size;
				}
			} /* endwhile */

			/* Now we have to append all files that were added to archive */
			f = arch->entriesAdded;

			while (f) {
				if (!WriteFile(f, temp))
					goto tempFailed;
				/* Write error */
				f = f->next;
			} /* endfor */

			/* And finaly write central directory structure */
			if (!WriteCentralDirectory(arch, temp))
				goto tempFailed;

			/* Now copy temporary file into archive. If we'll get a error in process, */
			/* we're lost! I don't know for a good solution for this without wasting */
			/* disk space for yet another copy of the archive :-( */
			{
				FileSystem& fileSys =
					FileSystem::Instance();
				// close file
				temp.Clear();
				file.Clear();

				// create an input and output stream
#if defined(NEX_MSVC)
				tempFile = 0;
				fopen_s(&tempFile, tmpFileName.c_str(), "rb");
#else
				tempFile = fopen(tmpFileName.c_str(), "rb");
#endif
				if (tempFile) {

					OutputStreamPtr outFile = fileSys.OpenWrite(
							arch->fileName);
					InputStreamPtr inpFile = Assign(
							NEX_NEW(FileInputStream(tempFile)));
					if (outFile)
						FileSystem::CopyStream(inpFile, outFile);
					outFile.Clear();
					inpFile.Clear();
				}

				file = fileSys.OpenRead(arch->fileName);
			}

			/* Now if we are here, all operations have been successful */
			UpdateDirectory(arch);
			arch->infile = file;
			success = true;

			tempFailed: SafeDeleteFile(tmpFileName);
			return success;
		}

		_NexZipApi bool FlushArchive(ZipArchive* arch) {
			if (!arch->infile)
				return false;
			return WriteZipArchive(arch);
		}

		_NexZipApi ZipFile* FindArchiveFile(ZipArchive* arch,
				const char* name) {
			ZipFile* f;
			f = GetArchiveDirFile(arch, name);
			if (f)
				return f;
			return GetArchiveAddedFile(arch, name);
		}

		static void ResetArchiveEntry(ZipFile* zf, size_t size, bool pack) {
			time_t t;
			FileTime ftm;

			zf->info.compressionMethod =
					(pack) ? DEFAULT_COMPRESSION_METHOD : ZIP_STORE;
			zf->info.ucsize = (uint32) size;
			zf->bufferPos = 0;

			t = time(0);

			NEX_SET_FILE_TIME(ftm, t);

			SetArchiveFileTime(zf, &ftm);
		}

		static ZipFile* CreateArchiveEntry(const char* filename, size_t size,
				bool pack) {
			zCentralDirFileHdr cdfh;
			ZipFile *f;

			std::memset(&cdfh, 0, sizeof(cdfh));
			cdfh.versionMadeBy[0] = 0x16; /* Zip version 2.2 rev 6 ??? */
			cdfh.versionMadeBy[1] = 0x06;
			cdfh.versionNeededToExtract[0] = 20; /* Unzip version 2.0 rev 0 */
			cdfh.versionNeededToExtract[1] = 00;

			f = (ZipFile*) NEX_ALLOC(sizeof (ZipFile), MEMCAT_GENERAL);
			std::memset(f, 0, sizeof(ZipFile));
			f->filename = StringUtils::NewStr(filename);
			memcpy(&f->info, &cdfh, sizeof(zCentralDirFileHdr));

			ResetArchiveEntry(f, size, pack);
			return f;
		}

		_NexZipApi ZipFile* AddNewFileToArchive(ZipArchive* arch,
				const char* filename, size_t size, bool pack) {
			ZipFile* zf;
			DeleteFileFromArchive(arch, filename);
			zf = GetArchiveAddedFile(arch, filename);
			if (zf)
				ResetArchiveEntry(zf, size, pack);
			else {
				zf = CreateArchiveEntry(filename, size, pack);
				zf->next = arch->entriesAdded;
				arch->entriesAdded = zf;
				arch->numFiles++;
			}
			arch->flags |= NEX_ZIP_DIRTY;
			return zf;
		}

		_NexZipApi bool DeleteFileFromArchive(ZipArchive* arch,
				const char* filename) {
			if (GetArchiveDirFile(arch, filename)) {
				StrNode* n =
						(StrNode*) NEX_ALLOC(sizeof (StrNode), MEMCAT_GENERAL);
						n->name = StringUtils::NewStr(filename);
						n->next = arch->delHead;
						arch->delHead = n;
						return true;
					}
					return false;
				}

				_NexZipApi bool FileExistsInArchive(ZipArchive* arch,
				const char* filename) {
			if (GetArchiveDirFile(arch, filename))
				return true;
			if (GetArchiveAddedFile(arch, filename))
				return true;
			return false;
		}

		static char* ReadEntry(InputStreamPtr& infile, ZipFile * f) {
			// This routine allocates one byte more than is actually needed
			// and fills it with zero. This can be used when reading text files
			size_t bytesLeft;
			char buff[1024];
			char *outBuff;
			int32 err;
			zLocalFileHdr lfh;

			outBuff = (char*) NEX_ALLOC(f->info.ucsize + 1, MEMCAT_GENERAL);
			if (!outBuff)
				return 0;
			outBuff[f->info.ucsize] = 0;

			if ((!infile->Seek(f->info.relativeOffsetLocalHeader,
					std::ios_base::beg))
					|| (infile->Read(buff, sizeof(hdrLocal)) < sizeof(hdrLocal))
					|| (memcmp(buff, hdrLocal, sizeof(hdrLocal)) != 0)
					|| (!ReadLFH(&lfh, infile))
					|| (!infile->Seek(lfh.filenameLength + lfh.extraFieldLength,
							std::ios_base::cur))) {
				NEX_FREE(outBuff, MEMCAT_GENERAL);
				return 0;
			}

			switch (f->info.compressionMethod) {
			case ZIP_STORE: {
				if (infile->Read(outBuff, f->info.csize) < f->info.csize) {
					NEX_FREE(outBuff, MEMCAT_GENERAL);
					return 0;
				} /* endif */
				break;
			}
			case ZIP_DEFLATE: {
				z_stream zs;

				bytesLeft = f->info.csize;
				zs.next_out = (Bytef*) outBuff;
				zs.avail_out = f->info.ucsize;
				zs.zalloc = (alloc_func) 0;
				zs.zfree = (free_func) 0;

				/* Undocumented: if wbits is negative, zlib skips header check */
				err = inflateInit2(&zs, -DEF_WBITS);
				if (err != Z_OK) {
					NEX_FREE(outBuff, MEMCAT_GENERAL);
					return 0;
				}
				while (bytesLeft) {
					size_t size;

					zs.next_in = (Bytef*) buff;
					if (bytesLeft > sizeof(buff))
						size = sizeof(buff);
					else
						size = bytesLeft;
					zs.avail_in = (uInt) infile->Read(buff, size);

					err = inflate(&zs,
							bytesLeft > size ? Z_PARTIAL_FLUSH : Z_FINISH);
					bytesLeft -= size;
				} /* endwhile */
				inflateEnd(&zs);

				/* discard dynamically allocated buffers on error */
				// Kludge warning: I've encountered a file where zlib 1.1.1 returned
				// Z_BUF_ERROR although everything was ok (a slightly compressed PNG
				// file)
				if ((err != Z_STREAM_END)
						&& ((err != Z_BUF_ERROR)|| bytesLeft || zs.avail_out)) {
					//delete [] outBuff;
					//return 0;
				}
				break;
			}
			default: {
				/* Can't handle this compression algorythm */
				NEX_FREE(outBuff, MEMCAT_GENERAL);
				return 0;
			}
			} /* endswitch */
			return outBuff;
		}

		_NexZipApi char* ReadFromArchiveFile(ZipArchive* a, ZipFile* f,
				size_t* s) {
			if (!f)
				return 0;
			if (s)
				*s = f->info.ucsize;
			if (!f->buffer) {
				f->buffer = ReadEntry(a->infile, f);
				f->bufferSize = f->bufferPos = f->info.ucsize;
			}
			return f->buffer;
		}

		static bool AppendData(ZipFile* f, const char* data, size_t size) {
			if (!f->buffer || (f->bufferPos + size > f->bufferSize)) {
				// Increase buffer size in 1K chunks
				size_t olsize = f->bufferSize;
				f->bufferSize += (size + 1023) & ~1023;
				// If the user has defined the uncompressed file size, take it
				if (f->bufferSize < f->info.ucsize)
					f->bufferSize = f->info.ucsize;
				char* oldata = f->buffer;
				f->buffer = (char *) NEX_ALLOC(f->bufferSize, MEMCAT_GENERAL);
				std::memcpy(f->buffer, oldata, std::min(olsize, f->bufferSize));
				if (oldata)
					NEX_FREE(oldata, MEMCAT_GENERAL);
				if (!f->buffer) {
					f->bufferPos = f->bufferSize = f->info.ucsize = 0;
					return false; /* Not enough memory */
				}
			} /* endif */

			if (f->info.ucsize < f->bufferPos + size)
				f->info.ucsize = (uint32) (f->bufferPos + size);

			std::memcpy(f->buffer + f->bufferPos, data, size);
			f->bufferPos += size;
			return true;
		}

		_NexZipApi bool WriteToArchiveFile(ZipArchive* a, ZipFile* f,
				const char* data, size_t s) {
			a->flags |= NEX_ZIP_DIRTY;
			return AppendData(f, data, s);
		}

		static void PackTime(const FileTime* tm, ush* rdate, ush* rtime) {
			*rdate = (ush) ((((int32) (tm->year - 1980) & 0x7f) << 9)
					| (((int32) (tm->mon & 0x0f) + 1) << 5)
					| (int32) (tm->day & 0x1f));
			*rtime = (ush) (((int32) (tm->hrs & 0x1f) << 11)
					| ((int32) (tm->min & 0x3f) << 5)
					| ((int32) (tm->sec >> 1) & 0x1f));
		}

		static void UnpackTime(ush zdate, ush ztime, FileTime* tm) {
			tm->year = (int16) (((zdate >> 9) & 0x7f) + 1980);
			tm->mon = (int8) ((zdate >> 5) & 0x0f) - 1;
			tm->day = (int8) (zdate & 0x1f);

			tm->hrs = (int8) ((ztime >> 11) & 0x1f);
			tm->min = (int8) ((ztime >> 5) & 0x3f);
			tm->sec = (int8) ((ztime & 0x1f) << 1);
		}

		_NexZipApi void GetArchiveFileTime(ZipFile* f, FileTime* tm) {
			if (f) {
				UnpackTime(f->info.lastModFileDate, f->info.lastModFileTime,
						tm);
			}
		}

		_NexZipApi void SetArchiveFileTime(ZipFile* f, const FileTime* tm) {
			if (f) {
				PackTime(tm, &(f->info.lastModFileDate),
						&(f->info.lastModFileTime));
			}
		}

		_NexZipApi bool AppendToReadArchiveFile(ZipArchive* arch,
				ZipFile* file) {
			size_t s = file->info.ucsize;
			if (!file->buffer) {
				file->buffer = ReadEntry(arch->infile, file);
				file->bufferSize = file->bufferPos = s;
			}
			arch->flags |= NEX_ZIP_DIRTY;
			return true;
		}

		_NexZipApi void SeekArchiveFile(ZipFile* file, std::ios_base::seekdir n,
				std::streamoff nb) {
			switch (n) {
			case std::ios_base::beg:
				file->bufferPos += static_cast<size_t>(nb);
				break;
			case std::ios_base::cur:
				file->bufferPos = static_cast<size_t>(nb);
				break;
			case std::ios_base::end:
				file->bufferPos =
						(nb) < 0 ?
								static_cast<size_t>((file->bufferSize) + nb) :
								file->bufferSize;
				break;
			}
			if (file->bufferPos > file->bufferSize)
				file->bufferPos = file->bufferSize;
		}

		_NexZipApi void GetFileList(ZipArchive* arch, zip::FileList& arr) {
			arr.reserve(arr.size() + arch->numFiles);
			ZipFile* f = arch->entriesHead;
			while (f) {
				arr.push_back(f);
				f = f->next;
			}
			f = arch->entriesAdded;
			while (f) {
				arr.push_back(f);
				f = f->next;
			}
		}

		_NexZipApi void GetFileNameList(ZipArchive* arch, StringVector& arr) {
			arr.reserve(arr.size() + arch->numFiles);
			ZipFile* f = arch->entriesHead;
			while (f) {
				arr.push_back(String(f->filename));
				f = f->next;
			}
			f = arch->entriesAdded;
			while (f) {
				arr.push_back(String(f->filename));
				f = f->next;
			}
		}
	}
}

